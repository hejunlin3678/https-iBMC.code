use bytes::Bytes;
use core::anit_dos;
use handles::datas::SmsReq;
use handles::ProxySms;
use serde_json::{map::Map as JsonMap, Value as JsonValue};
use std::convert::Infallible;
use warp::filters::BoxedFilter;
use warp::http::{HeaderMap, Method};
use warp::path::FullPath;
use warp::Filter;

#[derive(Debug)]
struct ContentLenInvalid(String);
impl warp::reject::Reject for ContentLenInvalid {}

#[derive(Debug)]
struct PayloadTooLarge();
impl warp::reject::Reject for PayloadTooLarge {}

pub fn routes() -> BoxedFilter<(impl warp::reply::Reply,)> {
  warp::method()
    .and(warp::path::full())
    .and(warp::header::headers_cloned())
    .and(warp::body::bytes())
    .and(warp::header::optional("X-Real-IP"))
    .and(warp::header::optional("X-Real-Port"))
    .and_then(
      |method: Method, path: FullPath, headers: HeaderMap, body: Bytes, ip: Option<String>, port: Option<String>| async move {
        let mut hs = JsonMap::new();
        for (k, v) in headers.iter() {
          if let Ok(d) = v.to_str().map(|v| v.into()) {
            hs.insert(k.to_string(), d);
          }
        }
        Ok::<_, Infallible>(SmsReq {
          Method: method.to_string(),
          Path: path.as_str().to_owned(),
          Headers: JsonValue::Object(hs),
          Body: body,
          Ip: ip,
          Port: port
        })
      },
    )
    .and_then(ProxySms)
    .with(warp::wrap_fn(anit_dos))
    .boxed()
}
