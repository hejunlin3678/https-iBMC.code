use anyhow::Result;
use bytes::{Bytes, BytesMut};
use core::multipart::AutoDeleteFile;
use core::{anit_dos, multipart::raw_body_multipart, multipart::FormData, InternalError, IntoResponse, ResponseError};
use futures::{ready, stream};
use handles::datas::RedfishReq;
use handles::post_update_firmware_inventory;
use handles::raw::DownloadStaticFile;
use handles::raw::ProxyRedfish;
use handles::raw::{AuthToken, UploadFileCheck, UploadFirmwareInventory, UploadFirmwareInventoryMemory};
use hyper::Body;
use redfish_base::{auth, getBasicAuth, response_error, AuthInfo, Context};
use regex::Regex;
use serde::Deserialize;
use serde_json::{json, map::Map as JsonMap, Value as JsonValue};
use std::convert::Infallible;
use std::pin::Pin;
use std::task::Poll;
use tokio::fs::File as TkFile;
use tokio_util::io::poll_read_buf;
use warp::filters::BoxedFilter;
use warp::http::{HeaderMap, Method, Response, StatusCode};
use warp::path::FullPath;
use warp::{header::optional, Filter, Rejection};

#[derive(Debug)]
struct ContentLenInvalid(String);
impl warp::reject::Reject for ContentLenInvalid {}

#[derive(Debug)]
struct PayloadTooLarge();
impl warp::reject::Reject for PayloadTooLarge {}

const SIZE_1M: u64 = 1024 * 1024;
const MAX_PAYLOAD: u64 = 1024 * 1024 * 10;
const MAX_IMPORT_FILE_SIZE: u64 = 1024 * 1024 * 96;

async fn handle_rejection(err: Rejection) -> Result<Response<String>, Rejection> {
  if let Some(_) = err.find::<ContentLenInvalid>() {
    return Ok(response_error(
      StatusCode::BAD_REQUEST,
      "BodyExceedsMaxLength",
      Some(JsonValue::Array(vec![])),
      Some(vec![]),
    ));
  } else if let Some(_) = err.find::<PayloadTooLarge>() {
    return Ok(response_error(
      StatusCode::BAD_REQUEST,
      "BodyExceedsMaxLength",
      Some(JsonValue::Array(vec![JsonValue::String(MAX_PAYLOAD.to_string())])),
      Some(vec![]),
    ));
  } else if let Some(bodyError) = err.find::<warp::body::BodyDeserializeError>() {
    let msg = bodyError.to_string();
    debug!("BodyDeserializeError: {}", &msg);
    return Ok(dispatch_deserialize_error(msg));
  } else if let Some(responseErr) = err.find::<ResponseError>() {
    let mut res = Response::new(responseErr.0.body().clone());
    *res.status_mut() = responseErr.0.status();
    *res.headers_mut() = responseErr.0.headers().clone();
    return Ok(res);
  } else if let Some(invalidHeadErr) = err.find::<warp::reject::InvalidHeader>() {
    return Ok(response_error(
      StatusCode::BAD_REQUEST,
      "PropertyMissing",
      Some(JsonValue::Array(vec![JsonValue::String(invalidHeadErr.name().into())])),
      Some(vec![]),
    ));
  } else if !err.is_not_found() {
    error!("handle_import_rejection: {:?}", err);
    return Ok(response_error(
      StatusCode::INTERNAL_SERVER_ERROR,
      "InternalError",
      Some(JsonValue::Array(vec![])),
      Some(vec![]),
    ));
  }
  Err(err)
}

async fn handle_static_file_rejection(err: Rejection) -> Result<Response<String>, Rejection> {
  if !err.is_not_found() {
    return Ok(response_error(
      StatusCode::FORBIDDEN,
      "InsufficientPrivilege",
      Some(JsonValue::Array(vec![])),
      Some(vec![]),
    ));
  }
  Err(err)
}

lazy_static! {
  static ref DUPLICATE: Regex = Regex::new(r"duplicate field `(?P<name>[^`]+)`").unwrap();
  static ref UNKNOWFIELD: Regex = Regex::new(r"unknown field `(?P<name>[^`]+)`").unwrap();
  static ref INVALIDTYPE: Regex = Regex::new(r"invalid type: \w+ (?P<name>.+).,").unwrap();
}

fn dispatch_deserialize_error(msg: String) -> Response<String> {
  if let Some(v) = DUPLICATE.captures(&msg).and_then(|cap| {
    cap.name("name").map(|name| {
      response_error(
        StatusCode::BAD_REQUEST,
        "PropertyDuplicate",
        Some(JsonValue::Array(vec![JsonValue::String(name.as_str().into())])),
        None,
      )
    })
  }) {
    return v;
  }

  if let Some(v) = UNKNOWFIELD.captures(&msg).and_then(|cap| {
    cap.name("name").map(|name| {
      response_error(
        StatusCode::BAD_REQUEST,
        "PropertyUnknown",
        Some(JsonValue::Array(vec![JsonValue::String(name.as_str().into())])),
        None,
      )
    })
  }) {
    return v;
  }

  if let Some(v) = INVALIDTYPE.captures(&msg).and_then(|cap| {
    cap.name("name").map(|name| {
      response_error(
        StatusCode::BAD_REQUEST,
        "PropertyValueTypeError",
        Some(JsonValue::Array(vec![
          JsonValue::String(name.as_str()[1..].into()),
          "#".into(),
        ])),
        None,
      )
    })
  }) {
    return v;
  }

  response_error(StatusCode::BAD_REQUEST, "UnrecognizedRequestBody", None, None)
}

fn header_to_json(headers: HeaderMap) -> JsonValue {
  let mut hs = JsonMap::new();
  for (k, v) in headers.iter() {
    if let Ok(d) = v.to_str().map(|v| v.into()) {
      hs.insert(k.to_string(), d);
    }
  }
  JsonValue::Object(hs)
}

fn upload_file() -> BoxedFilter<(impl warp::reply::Reply,)> {
  warp::post()
    .and(
      // core::__path, case insensitive path
      __path!("v1" / "UpdateService" / "FirmwareInventory")
        .and(
          raw_body_multipart(
            auth(AuthToken).map(|ctx: Context| ctx.user),
            MAX_IMPORT_FILE_SIZE,
            &[
              "hpm", "cer", "pem", "cert", "crt", "pfx", "p12", "xml", "keys", "pub", "zip", "asc", "keytab",
              "crl", "pub",
            ],
            false,
            false,
          )
          .and_then(move |mut formdata: FormData| async move {
            match UploadFirmwareInventory((formdata.clone(),)).await {
              Ok(reply) if reply.isOk() => {
                formdata.release();
                Ok(reply.0.data().map(|v| v.val.clone()).unwrap_or(json!({})))
              }
              Ok(reply) => Err(warp::reject::custom(ResponseError(reply.response()))),
              Err(e) => {
                error!("upload file except: {}", e.to_string());
                Err(warp::reject::custom(InternalError(format!("{}", e))))
              }
            }
          }),
        )
        .and_then(post_update_firmware_inventory)
        .with(warp::wrap_fn(anit_dos))
        .recover(handle_rejection)
        .unify(),
    )
    .boxed()
}

fn upload_file_memory() -> BoxedFilter<(impl warp::reply::Reply,)> {
  warp::post()
    .and(
      // core::__path, case insensitive path
      __path!("v1" / "UpdateService" / "FirmwareInventory" / "Memory")
        .and(
          raw_body_multipart(
            auth(AuthToken).map(|ctx: Context| ctx.user),
            SIZE_1M,
            &[
              "keytab",
            ],
            true,
            false,
          )
          .and_then(move |mut formdata: FormData| async move {
            match UploadFirmwareInventoryMemory((formdata.clone(),)).await {
              Ok(reply) if reply.isOk() => {
                formdata.release();
                Ok(reply.0.data().map(|v| v.val.clone()).unwrap_or(json!({})))
              }
              Ok(reply) => Err(warp::reject::custom(ResponseError(reply.response()))),
              Err(e) => {
                error!("upload file except: {}", e.to_string());
                Err(warp::reject::custom(InternalError(format!("{}", e))))
              }
            }
          }),
        )
        .and_then(post_update_firmware_inventory)
        .with(warp::wrap_fn(anit_dos))
        .recover(handle_rejection)
        .unify(),
    )
    .boxed()
}

fn upload_file_pre_check() -> BoxedFilter<(impl warp::reply::Reply,)> {
  warp::path!("upload_pre_check")
    .and(auth(AuthToken))
    .and(warp::path::full())
    .and(warp::header::headers_cloned())
    .and(warp::header::optional("X-Original-URI"))
    .and_then(
      move |ctx, path: FullPath, headers: HeaderMap, origUri: Option<String>| async move {
        let data = json!({
          "uri": path.as_str(),
          "headers": header_to_json(headers),
          "original_uri": origUri,
        });
        match UploadFileCheck((data, ctx)).await {
          Ok(reply) if reply.isOk() => Ok(Response::new("ok".to_owned())),
          Ok(reply) => Err(warp::reject::custom(ResponseError(reply.response()))),
          Err(e) => {
            error!("upload file check except: {}", e.to_string());
            Err(warp::reject::custom(InternalError(format!("{}", e))))
          }
        }
      },
    )
    .with(warp::wrap_fn(anit_dos))
    .recover(handle_rejection)
    .unify()
    .boxed()
}

fn download_static_file() -> BoxedFilter<(impl warp::reply::Reply,)> {
  warp::get()
    .and(
      // core::__path, case insensitive path
      __path!("bmc" / "tmpshare" / "tmp" / "web" / ..)
        .and(auth(AuthToken))
        .and_then(move |ctx: Context| async move {
          match DownloadStaticFile((ctx,)).await {
            Ok(reply) if reply.isOk() => Ok(Response::new(String::default())),
            Ok(reply) => Err(warp::reject::custom(ResponseError(reply.response()))),
            Err(e) => Err(warp::reject::custom(InternalError(e.to_string()))),
          }
        })
        .with(warp::wrap_fn(anit_dos))
        .map(|_| {})
        .untuple_one()
        .and(warp::fs::dir("/tmp/web/"))
        .recover(handle_static_file_rejection),
    )
    .boxed()
}

fn auth_static_resources() -> BoxedFilter<(impl warp::reply::Reply,)> {
  warp::get()
    .and(
      // core::__path, case insensitive path
      __path!("auth_static_resources" / ..)
        .and(auth(AuthToken))
        .and_then(move |_| async move { Ok::<_, Rejection>(Response::new(String::default()))})
        .with(warp::wrap_fn(anit_dos)),
    )
    .boxed()
}

const IS_DOWNLOAD_HEAD: &str = "is_download";
#[derive(Deserialize)]
struct DownloadFileBody {
  need_clean: bool,
  file_path: String,
}

async fn reply_file(mut f: AutoDeleteFile, mut reply: Response<String>) -> anyhow::Result<Response<Body>> {
  let mut buf = BytesMut::new();
  let body = Body::wrap_stream(stream::poll_fn(move |cx| {
    if buf.capacity() - buf.len() < DEFAULT_READ_BUF_SIZE {
      buf.reserve(DEFAULT_READ_BUF_SIZE);
    }

    match ready!(poll_read_buf(Pin::new(f.file.as_mut().unwrap()), cx, &mut buf)) {
      Ok(n) if n == 0 => Poll::Ready(None),
      Ok(_) => Poll::Ready(Some(Ok(buf.split().freeze()))),
      Err(err) => Poll::Ready(Some(Err(err))),
    }
  }));

  let mut response = Response::new(body);
  let headers = response.headers_mut();
  for (k, v) in reply.headers_mut().drain() {
    k.map(|h| headers.insert(h, v));
  }
  Ok(response)
}

const DEFAULT_READ_BUF_SIZE: usize = 8_192;
async fn filter_download_req(mut reply: Response<String>) -> anyhow::Result<Box<dyn warp::reply::Reply>> {
  if reply.headers_mut().remove(IS_DOWNLOAD_HEAD).is_some() {
    let file_info = serde_json::from_str::<DownloadFileBody>(reply.body())?;
    let file = TkFile::open(file_info.file_path.clone()).await?;
    let f = AutoDeleteFile::new(file_info.file_path, file);
    return Ok(Box::new(reply_file(f, reply).await?));
  }

  Ok(Box::new(reply))
}

pub fn routes() -> BoxedFilter<(impl warp::reply::Reply,)> {
  let proxy = warp::method()
    .and(warp::path::full())
    .and(
      warp::query::raw()
        .recover(|_| async move { Ok::<_, std::convert::Infallible>(String::default()) })
        .unify(),
    )
    .and(warp::header::headers_cloned())
    .and(
      warp::header::optional("content-length")
        .and_then(|contentLength: Option<String>| async move {
          match contentLength.map_or(Ok(0), |v| v.parse::<u64>()) {
            Ok(v) if v < MAX_PAYLOAD => Ok(v),
            Ok(_) => Err(warp::reject::custom(PayloadTooLarge())),
            Err(e) => Err(warp::reject::custom(ContentLenInvalid(e.to_string()))),
          }
        })
        .and(warp::body::bytes())
        .and_then(|_: u64, body: Bytes| async move { Ok::<_, Infallible>(body) }),
    )
    .and(warp::header::optional("X-Real-IP"))
    .and(warp::header::optional("X-Real-Port"))
    .and(
      warp::any()
        .and(
          warp::header("X-Auth-Token")
            .or(warp::cookie("SessionId"))
            .unify()
            .map(|v| Some(v))
            .recover(|_| async move { Ok::<_, std::convert::Infallible>(None) })
            .unify(),
        )
        .and(optional("Authorization"))
        .and(optional("X-Real-IP"))
        .map(|token: Option<String>, basic: Option<String>, ip: Option<String>| {
          let basic = getBasicAuth(basic, ip).unwrap_or_default();
          AuthInfo { token, basic }
        }),
    )
    .and_then(
      |method: Method,
       path: FullPath,
       query: String,
       headers: HeaderMap,
       body: Bytes,
       ip: Option<String>,
       port: Option<String>,
       auth_info: AuthInfo| async move {
        Ok::<_, Infallible>(RedfishReq {
          Method: method.to_string(),
          Path: path.as_str().to_owned(),
          Query: query,
          Headers: header_to_json(headers),
          Body: body,
          Ip: ip,
          Port: port,
          AuthInfo: auth_info,
        })
      },
    )
    .and_then(|req: RedfishReq| async move {
      match ProxyRedfish((req,)).await {
        Ok(reply) => Ok(reply.response()),
        Err(e) => Err(warp::reject::custom(InternalError(format!("{}", e)))),
      }
    })
    .with(warp::wrap_fn(anit_dos))
    .and_then(|req: Response<String>| async move {
      match filter_download_req(req).await {
        Ok(reply) => Ok(reply),
        Err(e) => Err(warp::reject::custom(InternalError(format!("{}", e)))),
      }
    })
    .boxed();

  __path!("redfish" / ..)
    .and(upload_file().or(upload_file_memory()).or(upload_file_pre_check()).or(proxy))
    .or(download_static_file())
    .or(auth_static_resources())
    .boxed()
}
