use super::validator::{CheckServerAddressOrNone, RegexOrNone, Error as ValidatorErr, Result as ValidatorResult, Validator};
use super::IntoResponse;
use anyhow::Result;
use bytes::Bytes;
use lua::{FromLua, Index, State, Type};
use regex::Regex;
use serde::de::{self, DeserializeOwned, SeqAccess, Visitor};
use serde::{ser::SerializeStruct, Deserialize, Deserializer, Serialize, Serializer};
use serde_json::Value as JsonValue;
use warp::filters::BoxedFilter;
use warp::http::{header::CONTENT_TYPE, HeaderValue, Response, StatusCode};
use warp::{Filter, Rejection};

pub fn body_origin(limit: u64) -> impl Filter<Extract = (Bytes,), Error = Rejection> + Clone {
  warp::body::content_length_limit(limit).and(warp::body::bytes())
}

#[derive(Debug)]
pub struct JsonValidateError(ValidatorErr);
impl warp::reject::Reject for JsonValidateError {}

impl ToString for JsonValidateError {
  fn to_string(&self) -> String {
    self.0.to_string()
  }
}

async fn body_deserialize_error<T>(err: Rejection) -> Result<T, Rejection> {
    if let Some(e) = err.find::<warp::body::BodyDeserializeError>() {
      Err(warp::reject::custom(ResponseError(dispatch_deserialize_error(
        e.to_string(),
      ))))
    } else {
      Err(err)
    }
}

pub fn body_form<T>(limit: u64) -> impl Filter<Extract = (T,), Error = Rejection> + Clone
where
  T: DeserializeOwned + Send + Validator,
{
  warp::body::content_length_limit(limit)
    .and(warp::body::form::<T>().recover(body_deserialize_error).unify())
    .and_then(|t: T| async move {
      match t.validate() {
        Ok(()) => Ok(t),
        Err(e) => Err(warp::reject::custom(JsonValidateError(ValidatorErr::new_errs(
          "__body_form",
          e,
        )))),
      }
    })
}

lazy_static! {
    static ref DUPLICATE: Regex = Regex::new(r"duplicate field `(?P<name>[^`]+)`").unwrap();
    static ref UNKNOWFIELD: Regex = Regex::new(r"unknown field `(?P<name>[^`]+)`").unwrap();
    static ref INVALIDTYPE: Regex = Regex::new(r"invalid type: (?P<t1>\w+) .+, expected a (?P<t2>\w+)").unwrap();
    static ref MISSFIELD: Regex = Regex::new(r"missing field `(?P<name>[^`]+)`").unwrap();
  }
  
fn match_deserialize_error<'a>(
    msg: &str,
    rx: &Regex,
    err_code: &'static str,
    get_msg: fn(cap: &regex::Captures) -> Option<String>,
  ) -> Option<Response<String>> {
    let cap = rx.captures(&msg)?;
    let msg = get_msg(&cap)?;
    Some(response_error(StatusCode::BAD_REQUEST, err_code, msg))
}
  
fn dispatch_deserialize_error(msg: String) -> Response<String> {
    if let Some(v) = match_deserialize_error(&msg, &DUPLICATE, "PropertyDuplicate", |cap| {
      cap.name("name").map(|v| v.as_str().into())
    }) {
      return v;
    }
  
    if let Some(v) = match_deserialize_error(&msg, &UNKNOWFIELD, "PropertyUnknown", |cap| {
      cap.name("name").map(|v| v.as_str().into())
    }) {
      return v;
    }
  
    if let Some(v) = match_deserialize_error(&msg, &MISSFIELD, "PropertyMissing", |cap| {
      cap.name("name").map(|v| v.as_str().into())
    }) {
      return v;
    }
  
    if let Some(v) = match_deserialize_error(&msg, &INVALIDTYPE, "PropertyValueTypeError", |cap| {
      let got = cap.name("t1")?.as_str();
      let expected = cap.name("t2")?.as_str();
      Some(format!("expected {}, got {}", expected, got))
    }) {
      return v;
    }
  
    response_error(StatusCode::BAD_REQUEST, "UnrecognizedRequestBody", Default::default())
}

pub fn body_json<T>(limit: u64) -> impl Filter<Extract = (T,), Error = Rejection> + Clone
where
  T: DeserializeOwned + Send + Validator,
{
  warp::body::content_length_limit(limit)
    .and(warp::body::json::<JsonValue>().recover(body_deserialize_error).unify())
    .and_then(|v: JsonValue| async move {
      match serde_json::from_value(v) {
        Ok(t) => Ok(t),
        Err(e) => Err(warp::reject::custom(ResponseError(dispatch_deserialize_error(
          e.to_string(),
        )))),
      }
    })
    .and_then(|t: T| async move {
      match t.validate() {
        Ok(()) => Ok(t),
        Err(e) => Err(warp::reject::custom(JsonValidateError(ValidatorErr::new_errs(
          "__body_json",
          e,
        )))),
      }
    })
}

fn check_host(host_port: &String) -> ValidatorResult {
  let host = match Regex::new(r"^(.+):\d+$") {
    Ok(re) => re.captures(host_port).map_or(None, |caps| caps.get(1)),
    Err(_) => None,
  };
  match host {
    Some(v) => CheckServerAddressOrNone("Host", &Some(v.as_str().trim_matches(|c| c == '[' || c == ']').into())),
    None => Err(ValidatorErr::new_ensure("CheckHost", "Host")),
  }
}

fn check_host_Rex(host_port: &String) -> ValidatorResult {
  match RegexOrNone(
    "Host",
    &Some(host_port.as_str().into()),
    r"^[\[\]a-zA-Z0-9:._-]+$",
  ) {
    Ok(_) => Ok(()),
    Err(_) => Err(ValidatorErr::new_ensure("CheckHost", "Host")),
  }
}

pub fn header_host() -> impl Filter<Extract = (Option<String>,), Error = Rejection> + Clone {
  warp::header::optional("Host").and_then(|host_port: Option<String>| async move {
    host_port.map_or(Ok(None), |v| match check_host_Rex(&v) {
      Ok(()) => Ok(Some(v)),
      Err(e) => Err(warp::reject::custom(JsonValidateError(e))),
    })
  })
}

pub fn query<T: DeserializeOwned + Send + 'static + Validator>(
) -> impl Filter<Extract = (T,), Error = Rejection> + Clone {
  warp::query::<T>().and_then(|t: T| async move {
    match t.validate() {
      Ok(()) => Ok(t),
      Err(e) => Err(warp::reject::custom(JsonValidateError(ValidatorErr::new_errs(
        "__query", e,
      )))),
    }
  })
}

pub trait PathSegmentTrait {
  fn segment() -> &'static str;
}
struct PathSegment<T>(std::marker::PhantomData<T>);
impl<T: PathSegmentTrait> std::str::FromStr for PathSegment<T> {
  type Err = ();
  fn from_str(s: &str) -> Result<Self, ()> {
    if s.as_bytes().eq_ignore_ascii_case(T::segment().as_bytes()) {
      Ok(PathSegment(std::marker::PhantomData))
    } else {
      Err(())
    }
  }
}

pub fn case_insensitive_path<T: PathSegmentTrait + Send + 'static>(
) -> impl Filter<Extract = (), Error = Rejection> + Clone {
  warp::path::param().map(|_: PathSegment<T>| {}).untuple_one()
}

pub fn string_or_bytes<'de, D>(deserializer: D) -> std::result::Result<Bytes, D::Error>
where
  D: Deserializer<'de>,
{
  struct StringOrBytes(std::marker::PhantomData<fn() -> Bytes>);

  impl<'de> Visitor<'de> for StringOrBytes {
    type Value = Bytes;

    fn expecting(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
      formatter.write_str("string or sequence")
    }

    fn visit_str<E>(self, value: &str) -> std::result::Result<Bytes, E>
    where
      E: de::Error,
    {
      Ok(value.as_bytes().to_vec().into())
    }

    fn visit_seq<V>(self, mut visitor: V) -> std::result::Result<Bytes, V::Error>
    where
      V: SeqAccess<'de>,
    {
      let mut vec = Vec::new();

      while let Some(element) = visitor.next_element()? {
        vec.push(element)
      }

      Ok(vec.into())
    }
  }

  deserializer.deserialize_any(StringOrBytes(std::marker::PhantomData))
}

pub fn bytes_to_string<S>(x: &Bytes, s: S) -> Result<S::Ok, S::Error>
where
  S: Serializer,
{
  s.serialize_bytes(x)
}

#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct ErrorInfo {
  pub code: String,
  #[serde(skip_serializing_if = "Option::is_none")]
  pub message: Option<JsonValue>,
  #[serde(skip_serializing_if = "Option::is_none")]
  pub properties: Option<Vec<String>>,
}

impl ErrorInfo {
  pub fn new(code: &str, message: Option<Vec<String>>, properties: Option<Vec<String>>) -> Self {
    ErrorInfo {
      code: code.into(),
      message: message.map(|msgs| {
        JsonValue::Array(
          msgs
            .into_iter()
            .map(|v| JsonValue::String(v))
            .collect::<Vec<JsonValue>>(),
        )
      }),
      properties,
    }
  }
}

from_lua!(ErrorInfo, code, message, properties);
to_lua!(ErrorInfo, code, message, properties);

impl ToString for ErrorInfo {
  fn to_string(&self) -> String {
    serde_json::to_string(self).unwrap_or("invalid error info".into())
  }
}

#[derive(Serialize, Deserialize)]
#[serde(untagged)]
pub enum ErrorInfoVal {
  Value(ErrorInfo),
  Array(Vec<ErrorInfo>),
}

impl FromLua for ErrorInfoVal {
  fn from_lua(state: &mut State, index: Index) -> Result<Self> {
    let len = state.raw_len(index);
    if len == 0 {
      Ok(ErrorInfoVal::Value(state.to_type::<ErrorInfo>(index)?))
    } else {
      Ok(ErrorInfoVal::Array(state.to_type::<Vec<ErrorInfo>>(index)?))
    }
  }
}

#[derive(Deserialize)]
pub struct ErrorMessage<T> {
  pub error: ErrorInfoVal,
  #[serde(skip_serializing_if = "Option::is_none")]
  pub data: Option<T>,
}

impl<T: FromLua> FromLua for ErrorMessage<T> {
  fn from_lua(state: &mut State, index: Index) -> Result<Self> {
    let abs_index = state.absindex(index);
    state.check_type(abs_index, Type::Table)?;
    state.push_string("error")?;
    state.get_table(abs_index);
    let error = state.to_type::<ErrorInfoVal>(-1)?;

    state.push_string("data")?;
    state.get_table(abs_index);
    let data = state.to_type::<Option<T>>(-1)?;

    Ok(ErrorMessage { error, data })
  }
}

impl<T> ErrorMessage<T> {
  pub fn new(code: String, message: String) -> Self {
    ErrorMessage {
      error: ErrorInfoVal::Value(ErrorInfo {
        code: code,
        message: Some(JsonValue::String(message)),
        properties: None,
      }),
      data: None,
    }
  }

  pub fn newArray(code: String, message: String) -> Self {
    ErrorMessage {
      error: ErrorInfoVal::Array(vec![ErrorInfo {
        code: code,
        message: Some(JsonValue::String(message)),
        properties: None,
      }]),
      data: None,
    }
  }

  pub fn newMessage(code: String, message: Option<JsonValue>, props: Option<Vec<String>>) -> Self {
    ErrorMessage {
      error: ErrorInfoVal::Array(vec![ErrorInfo {
        code: code,
        message: message,
        properties: props,
      }]),
      data: None,
    }
  }

  pub fn map<U: From<T>>(self) -> ErrorMessage<U> {
    ErrorMessage {
      error: self.error,
      data: self.data.map(|v| From::from(v)),
    }
  }
}

impl<T: Serialize> Serialize for ErrorMessage<T> {
  fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
  where
    S: Serializer,
  {
    let mut s = serializer.serialize_struct("ErrorMessage", 2)?;
    s.serialize_field("error", &self.error)?;
    if let Some(ref data) = self.data {
      s.serialize_field("data", data)?;
    }
    s.end()
  }
}

impl<T: Serialize> ToString for ErrorMessage<T> {
  fn to_string(&self) -> String {
    match serde_json::to_string(&self) {
      Ok(s) => s,
      Err(e) => e.to_string(),
    }
  }
}

pub fn response_error(status: StatusCode, code: &'static str, msg: String) -> Response<String> {
  let errMsg = &ErrorMessage::<JsonValue>::newArray(code.to_owned(), msg);
  let mut res = Response::new(errMsg.to_string());
  *res.status_mut() = status;
  res
    .headers_mut()
    .insert(CONTENT_TYPE, HeaderValue::from_static("application/json;charset=utf-8"));
  res
}

#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct User {
  pub Privilege: u8,
  pub UserName: String,
  pub ClientIp: String,
  pub RoleId: String,
  pub SessionId: String,
  pub AuthType: u8,
}

impl User {
  pub fn new() -> Self {
    User {
      Privilege: 0,
      UserName: String::default(),
      ClientIp: String::default(),
      RoleId: String::default(),
      SessionId: String::default(),
      AuthType: 0,
    }
  }
}

from_to_lua!(User, Privilege, UserName, ClientIp, RoleId, SessionId, AuthType);

impl ToString for User {
  fn to_string(&self) -> String {
    match serde_json::to_string(&self) {
      Ok(s) => s,
      Err(e) => e.to_string(),
    }
  }
}

#[derive(Debug)]
pub struct ResponseError(pub Response<String>);

#[derive(Debug)]
pub struct InternalError(pub String);

impl warp::reject::Reject for ResponseError {}
impl warp::reject::Reject for InternalError {}

pub trait TupleFilter {
  type Extract;
  fn and_then(self) -> BoxedFilter<(Self::Extract,)>;
}

/// tuple Filter.
///
/// At routes! This filter is used to help @ filter capture parameters.
///
pub fn tuple<F>(filter: F) -> internal::TupleWrap<F, F::Extract>
where
  F: Filter + Sized + Sync + Send + 'static,
{
  internal::TupleWrap(filter, std::marker::PhantomData)
}

mod internal {
  use super::{BoxedFilter, Filter, Rejection, TupleFilter};
  pub struct TupleWrap<F, T>(pub F, pub std::marker::PhantomData<T>);

  macro_rules! gen_tuple_filter {
    ($($tt: ident),*) => {
      impl<F, $($tt,)*> TupleFilter for TupleWrap<F, ($($tt,)*)>
      where
        $($tt: Sized + Clone + Send + Sync + 'static,)*
        F: Filter<Extract = ($($tt,)*), Error = Rejection> + Sized + Sync + Send + 'static
      {
        type Extract = ($($tt,)*);
        fn and_then(self) -> BoxedFilter<(Self::Extract,)> {
          self.0
            .and_then(move |$($tt: $tt),*| async move { Ok::<_, Rejection>(($($tt,)*)) })
            .boxed()
        }
      }
    };
  }

  gen_tuple_filter!();
  gen_tuple_filter!(T1);
  gen_tuple_filter!(T1, T2);
  gen_tuple_filter!(T1, T2, T3);
  gen_tuple_filter!(T1, T2, T3, T4);
  gen_tuple_filter!(T1, T2, T3, T4, T5);
}

// @then Filter.
//
// simplifies the routing table format.
// Equivalent to the warp::Filter::and_then
//
// # Example
//
// ```rust
// use crate::privilege::Privilege;
// use crate::handles as h;
// use crate::utils::context;
//
// ["Test"] => (context(Privilege::READONLY), @then(h::raw::Test));
//
// ["Test"] => (context(Privilege::READONLY)).and_then(h::Test);
// ```
pub struct then<T, H>(pub fn(T) -> H);

impl<T, H, R: IntoResponse> then<T, H>
where
  T: Sized + Clone + Send + Sync + 'static,
  H: std::future::Future<Output = Result<R>> + Sized + Send + 'static,
{
  pub fn with<F>(
    &'static self,
    filter: F,
  ) -> impl warp::Filter<Extract = ((Box<Response<String>>,),), Error = Rejection> + Sized + Send + Sync + 'static
  where
    F: warp::Filter<Extract = (T,), Error = Rejection> + Sized + Send + Sync + 'static,
  {
    filter.and_then(move |t: T| async move {
      match (self.0)(t).await {
        Ok(s) => Ok((Box::new(s.response()),)),
        Err(e) => Err(warp::reject::custom(InternalError(e.to_string()))),
      }
    })
  }
}
