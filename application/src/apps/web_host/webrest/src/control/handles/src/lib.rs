#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate core;
extern crate anyhow;
extern crate lua;
#[macro_use]
extern crate serde_json;

mod datas;
mod handles;
mod utils;

use anyhow::Result;
use core::{fmt_json, body_json, response_error, Reply, ResponseError, User};
use lua::{FromLua, Index, State};
use privilege::{privileg_check, Privilege};
use serde::{Serialize, Serializer};
use serde_json::Value as JsonValue;
use warp::http::StatusCode;
use warp::{Filter, Rejection};

pub use handles::*;
pub use utils::*;

const MAX_BODY_LEN: u64 = 100;

pub struct WebValue {
  pub val: JsonValue,
}

impl ToString for WebValue {
  fn to_string(&self) -> String {
    fmt_json(&self.val)
  }
}

/*
Description: Web value serialization
*/
impl Serialize for WebValue {
  fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
  where
    S: Serializer,
  {
    self.val.serialize(serializer)
  }
}

impl FromLua for WebValue {
  fn from_lua(s: &mut State, index: Index) -> Result<Self> {
    let reply: JsonValue = s.to_type(index)?;
    match reply {
      // JsonValue::FromLua 把空表当做空数组，但 web 返回值一定是对象，这里强制转换一下
      JsonValue::Array(a) if a.len() == 0 => Ok(WebValue { val: json!({}) }),
      val => Ok(WebValue { val }),
    }
  }
}

pub(crate) type WebReply = Reply<WebValue>;

pub fn context(pri: Privilege) -> impl Filter<Extract = (User,), Error = Rejection> + Clone {
  auth(handles::raw::AuthUser).and_then(move |user: User| async move {
    match privileg_check(&user, pri) {
      Ok(_) => Ok(user),
      Err(e) => Err(e),
    }
  })
}

fn is_application_json(media_type: Option<String>) -> bool {
    media_type.map_or(true, |t| {
      t.to_lowercase().split(';').find(|s| *s == "application/json").is_some()
    })
  }

pub fn empty_body() -> impl Filter<Extract = (), Error = Rejection> + Clone {
    warp::header::optional("content-type")
      .and(warp::header::optional("content-length"))
      .and_then(|media_type: Option<String>, len: Option<u32>| async move {
        if len.map_or(false, |len| len == 0) && is_application_json(media_type) {
          return Ok(());
        }
        Err(warp::reject::reject())
      })
      .or(body_json(MAX_BODY_LEN).and_then(|val: JsonValue| async move {
        if val.as_object().map_or(false, |obj| obj.is_empty()) {
          return Ok(());
        }
        Err(warp::reject::custom(ResponseError(response_error(
          StatusCode::BAD_REQUEST,
          "InvalidParam",
          String::default(),
        ))))
      }))
      .unify()
      .untuple_one()
}