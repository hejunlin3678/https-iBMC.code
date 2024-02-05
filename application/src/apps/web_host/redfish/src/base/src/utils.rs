use super::reply::{response_error, RedfishReply, RedfishVal};
use anyhow::{anyhow, Result};
use base64::{Engine as _, engine::general_purpose};
use bytes::Bytes;
use core::{InternalError, IntoResponse, Reply, ReplyFinish, ResponseError, User};
use lua::{FromLua, Index, State, ToLua};
use serde::Serialize;
use serde::Serializer;
use serde_json::{json, Value as JsonValue};
use std::future::Future;
use warp::http::{Response, StatusCode};
use warp::{header::optional, path::FullPath, reject, Filter, Rejection};

#[derive(Serialize, Clone)]
pub struct Context {
  pub user: User,
  pub uri: String,
}

impl ToLua for Context {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    self.user.to_lua(L)?;
    L.push_string("uri")?;
    self.uri.to_lua(L)?;
    L.set_table(-3)?;
    Ok(())
  }
}

#[derive(Clone, Debug)]
pub struct BasicAuth {
  UserName: String,
  Password: Bytes,
  ClientIp: String,
}
to_lua!(BasicAuth, UserName, Password, ClientIp);

#[derive(Clone)]
pub struct AuthInfo {
  pub token: Option<String>,
  pub basic: Option<BasicAuth>,
}
to_lua!(AuthInfo, token, basic);

fn parseBasicAuthInfo(basic: String, ip: Option<String>) -> Result<BasicAuth> {
  if basic.starts_with("Basic ") {
    let (_, v) = basic.split_at(6);
    if let Ok(out) = general_purpose::STANDARD.decode(v) {
      if let Some(pos) = out.iter().position(|&v| v == b':') {
        return Ok(BasicAuth {
          UserName: String::from_utf8(out[..pos].into())?,
          Password: Bytes::copy_from_slice(&out[pos + 1..]),
          ClientIp: ip.unwrap_or("127.0.0.1".to_owned()),
        });
      }
    }
  }
  Err(anyhow!("invalid basic authorization"))
}

pub fn getBasicAuth(basic: Option<String>, ip: Option<String>) -> std::result::Result<Option<BasicAuth>, warp::Rejection> {
  match basic {
    Some(v) => match parseBasicAuthInfo(v, ip) {
      Err(_) => {
        let rsp = response_error(StatusCode::BAD_REQUEST, "AuthorizationFailed", None, None);
        Err(reject::custom(ResponseError(rsp)))
      }
      Ok(v) => Ok(Some(v)),
    },
    None => Ok(None),
  }
}

impl From<User> for RedfishVal {
  fn from(v: User) -> Self {
    RedfishVal {
      val: json!({
        "Privilege": v.Privilege,
        "UserName": v.UserName,
        "ClientIp": v.ClientIp,
        "RoleId": v.RoleId,
        "SessionId": v.SessionId,
        "AuthType": v.AuthType,
      }),
    }
  }
}

pub fn auth<Fut>(handle: fn((AuthInfo,)) -> Fut) -> impl Filter<Extract = (Context,), Error = Rejection> + Clone
where
  Fut: Future<Output = Result<Reply<User>>> + Send + 'static,
{
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
    .and(warp::path::full())
    .and_then(
      move |token: Option<String>, basic: Option<String>, ip: Option<String>, uri: FullPath| async move {
        let basic = getBasicAuth(basic, ip)?;
        match handle((AuthInfo { token, basic },)).await {
          Ok(reply) if reply.isOk() => match reply.body {
            Some(user) => Ok(Context {
              user,
              uri: uri.as_str().to_owned(),
            }),
            None => Err(reject::custom(InternalError("auth user is none".to_owned()))),
          },
          Ok(reply) => Err(reject::custom(ResponseError(RedfishReply(reply.map()).response()))),
          Err(e) => Err(reject::custom(InternalError(e.to_string()))),
        }
      },
    )
}

pub struct DownloadReply(pub RedfishReply);

pub struct RawString(pub String);
impl FromLua for RawString {
  fn from_lua(state: &mut State, index: Index) -> Result<RawString> {
    state
      .to_bytes(index)
      .map(|v| RawString(unsafe { String::from_utf8_unchecked(v.to_vec()) }))
  }
}

impl ToString for RawString {
  fn to_string(&self) -> String {
    self.0.clone()
  }
}

impl Serialize for RawString {
  fn serialize<S>(&self, serializer: S) -> std::result::Result<S::Ok, S::Error>
  where
    S: Serializer,
  {
    self.0.serialize(serializer)
  }
}

impl From<RawString> for RedfishVal {
  fn from(v: RawString) -> Self {
    RedfishVal {
      val: JsonValue::String(v.0),
    }
  }
}

impl FromLua for DownloadReply {
  fn from_lua(s: &mut State, index: Index) -> Result<Self> {
    let reply: core::Reply<RawString> = s.to_type(index)?;
    Ok(DownloadReply(RedfishReply(reply.map())))
  }
}

impl IntoResponse for DownloadReply {
  fn response(self) -> Response<String> {
    self.0.response()
  }
}

impl ReplyFinish for DownloadReply {
  fn finish(&mut self) {}
}
