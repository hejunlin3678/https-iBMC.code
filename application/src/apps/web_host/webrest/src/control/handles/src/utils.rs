use anyhow::Result;
use core::{InternalError, IntoResponse, Reply, ResponseError, User};
use std::future::Future;
use warp::http::Method;
use warp::{reject, Filter, Rejection};

pub fn auth<Fut>(
  handle: fn((String, String, String, Option<String>)) -> Fut,
) -> impl Filter<Extract = (User,), Error = Rejection> + Clone
where
  Fut: Future<Output = Result<Reply<User>>> + Send,
{
  warp::method()
    .and(warp::path::full())
    .and(warp::cookie("SessionId"))
    .and(warp::header::optional("X-CSRF-Token"))
    .and_then(
      move |method: Method, path: warp::path::FullPath, SessionId: String, csrf_token: Option<String>| async move {
        match handle((method.to_string(), path.as_str().into(), SessionId, csrf_token)).await {
          Ok(reply) => {
            if reply.isOk() {
              match reply.body {
                Some(b) => Ok(b),
                None => Err(reject::custom(InternalError("auth user is none".to_owned()))),
              }
            } else {
              Err(reject::custom(ResponseError(reply.response())))
            }
          }
          Err(e) => Err(reject::custom(InternalError(e.to_string()))),
        }
      },
    )
}