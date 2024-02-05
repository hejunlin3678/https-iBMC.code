use super::webtask;
use core::{anit_dos, anit_dos_locked, body_form, body_json, header_host, query};
use core::{response_error, InternalError, JsonValidateError, ResponseError};
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::http::{Response, StatusCode};
use warp::{Filter, Rejection};
use webrest_handles as h;
use webrest_handles::{context, empty_body};

const SIZE_1K: u64 = 1024;
const SIZE_1M: u64 = 1024 * 1024;
const RO: Privilege = Privilege::READONLY;
const BS: Privilege = Privilege::BASICSETTING;
const SM: Privilege = Privilege::SECURITYMGNT;
const UM: Privilege = Privilege::USERMGNT;

async fn handle_rejection(err: Rejection) -> Result<Response<String>, Rejection> {
  if let Some(responseErr) = err.find::<ResponseError>() {
    let mut res = Response::new(responseErr.0.body().clone());
    *res.status_mut() = responseErr.0.status();
    *res.headers_mut() = responseErr.0.headers().clone();
    return Ok(res);
  } else if let Some(internalErr) = err.find::<InternalError>() {
    error!("INTERNAL_SERVER_ERROR: {:?}", internalErr);
    return Ok(response_error(
      StatusCode::INTERNAL_SERVER_ERROR,
      "InternalError",
      Default::default(),
    ));
  } else if let Some(missingCookieErr) = err.find::<warp::reject::MissingCookie>() {
    return Ok(response_error(
      StatusCode::UNAUTHORIZED,
      "InvalidSession",
      missingCookieErr.to_string(),
    ));
  } else if let Some(invalidHeaderErr) = err.find::<warp::reject::InvalidHeader>() {
    if invalidHeaderErr.name() == "cookie" {
      return Ok(response_error(
        StatusCode::UNAUTHORIZED,
        "InvalidSession",
        invalidHeaderErr.to_string(),
      ));
    }
  } else if let Some(jsonValidateErr) = err.find::<JsonValidateError>() {
    return Ok(response_error(
      StatusCode::BAD_REQUEST,
      "InvalidField",
      jsonValidateErr.to_string(),
    ));
  } else if let Some(_) = err.find::<warp::body::BodyDeserializeError>() {
    return Ok(response_error(
      StatusCode::BAD_REQUEST,
      "MalformedJSON",
      String::default(),
    ));
  } else if let Some(_) = err.find::<warp::reject::PayloadTooLarge>() {
    return Ok(response_error(
      StatusCode::PAYLOAD_TOO_LARGE,
      "InvalidParam",
      String::default(),
    ));
  } else if let Some(_) = err.find::<warp::reject::LengthRequired>() {
    return Ok(response_error(
      StatusCode::LENGTH_REQUIRED,
      "InvalidParam",
      String::default(),
    ));
  }

  Err(err)
}

fn web_routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    ["System" / .. ] => (system::routes());
    ["Maintenance" / .. ] => (maintenance::routes());
    ["AccessMgnt" / .. ] => (access_mgnt::routes());
    ["Services" / .. ] => (services::routes());
    ["BMCSettings" / .. ] => (bmc_settings::routes());
    ["Chassis" / .. ] => (chassis::routes());
    GET => {
      ["Login"] => and_then(h::GetLogin);
      ["SSOHandler"] => (query(), warp::header::optional("X-Real-IP"), header_host(), warp::cookie::optional("SessionId"), warp::cookie::optional("token")).and_then(h::SSOAccess);
      ["MutualHandler"] => (warp::header::optional("X-Real-IP"),
                            warp::header::optional("X-SSL_Client_serial"),
                            warp::header::optional("X-SSL-Client-issuer"),
                            warp::header::optional("X-SSL-Client-subject"),
                            warp::cookie::optional("logOutByCert"),
                            warp::cookie::optional("SessionId")).and_then(h::MutualLogin);
      ["KerberosHandler"] => (warp::header::optional("Authorization")).and_then(h::KBRSSOLogin);
      ["KVMHandler"] => (query(), warp::header::optional("User-Agent"), warp::cookie::optional("KvmSession"), warp::cookie::optional("authParam"), header_host()).and_then(h::AccessKVM);
      ["Sessions"] => (context(RO)).and_then(h::GetSessions);
      ["GenericInfo"] => (context(RO)).and_then(h::GetGenericInfo);
      ["Overview"] => (context(RO)).and_then(h::GetOverview);
      ["AuthJar"] => (warp::header::optional("X-Real-Url")).and_then(h::AuthJar);
      ["Enclosure" / "Overview"] => (context(RO)).and_then(h::GetChassisOverview)
    };
    POST => {
      ["Login"] => (body_json(SIZE_1M), warp::header::optional("X-Real-IP"), warp::cookie::optional("SessionId")).and_then(h::Login);
      ["SSOHandler"] => (body_form(SIZE_1K), warp::header::optional("X-Real-IP")).and_then(h::SSOLogin);
      ["KVMHandler"] => (warp::header::optional("X-Real-IP"), body_form(SIZE_1K)).and_then(h::AuthKVM);
      ["KeepAlive"] => (body_json(SIZE_1K), context(RO)).and_then(h::KeepAlive);
      ["Dump"] => (context(SM), empty_body()).and_then(h::Dump);
      ["RestoreFactory"] => (body_json(SIZE_1K), context(UM)).and_then(h::RestoreFactory)
    };
    PATCH => {
      ["GenericInfo"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateGenericInfo);
      ["Enclosure" / "Overview"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateChassisOverview)
    };
    DELETE => {
      ["Sessions" / String] => (context(RO)).and_then(h::DeleteSession)
    }
  )
}

pub fn routes() -> BoxedFilter<(impl warp::reply::Reply,)> {
  let web_routes = __path!("UI" / "Rest" / ..)
    .and(web_routes())
    .recover(handle_rejection)
    .unify()
    .with(warp::wrap_fn(anit_dos));

  let task = __path!("UI" / "Rest" / "task" / String)
    .and(anit_dos_locked().map(|_| {}).untuple_one())
    .and(context(RO).map(|_| {}).untuple_one())
    .and_then(webtask::QryDownloadTaskInfo);

  let download = __path!("UI" / "Rest" / "download" / ..)
    .and(anit_dos_locked().map(|_| {}).untuple_one())
    .and(context(RO).map(|_| {}).untuple_one())
    .and(webtask::download())
    .and_then(webtask::DownloadSuccessHandle)
    .with(warp::reply::with::header("content-type", "application/octet-stream"));

  web_routes
    .or(warp::get().and(task.or(download)))
    .recover(handle_rejection)
    .boxed()
}
