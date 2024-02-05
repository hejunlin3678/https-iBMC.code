use super::registries::create_message;
use anyhow::Result;
use core::{fmt_json, ErrorInfo, ErrorInfoVal, ErrorMessage, IntoResponse, Reply, ReplyFinish, ResponseError};
use itertools::join;
use lua::{FromLua, Index, State};
use serde::Serialize;
use serde_json::{json, Value as JsonValue};
use std::iter::FromIterator;
use warp::http::header::{ALLOW, CONTENT_TYPE, LINK};
use warp::http::{HeaderMap, HeaderValue, Response, StatusCode};
use warp::{reject, Rejection};

bitflags! {
  pub struct AllowHead: u8 {
    const GET     = 0b00000001;
    const PATCH   = 0b00000010;
    const POST    = 0b00000100;
    const DELETE  = 0b00001000;
    const ALL     = 0b00001111;
  }
}

#[derive(Serialize)]
pub struct RedfishVal {
  pub val: JsonValue,
}

impl FromLua for RedfishVal {
  fn from_lua(s: &mut State, index: Index) -> Result<Self> {
    let reply: JsonValue = s.to_type(index)?;
    match reply {
      // JsonValue::FromLua 把空表当做空数组，但 redfish 返回值一定是对象，这里强制转换一下
      JsonValue::Array(a) if a.len() == 0 => Ok(RedfishVal { val: json!({}) }),
      val => Ok(RedfishVal { val }),
    }
  }
}

impl ToString for RedfishVal {
  fn to_string(&self) -> String {
    match &self.val {
      JsonValue::Object(obj) => {
        let mut header = vec![None, None, None, None];
        let mut datas = Vec::with_capacity(obj.len());
        let mut Oem = None;
        for (k, v) in obj {
          let msg = format!(r#""{}":{}"#, k, v.to_string());
          if k == "@odata.context" {
            header[0] = Some(msg);
          } else if k == "@odata.id" {
            header[1] = Some(msg);
          } else if k == "@odata.type" {
            header[2] = Some(msg);
          } else if k == "Id" {
            header[3] = Some(msg);
          } else if k == "Oem" {
            Oem = Some(msg);
          } else {
            datas.push(msg);
          }
        }

        if let Some(msg) = Oem {
          datas.push(msg);
        }

        format!("{{{}}}", join(header.into_iter().filter_map(|x| x).chain(datas), ","))
      }
      val => fmt_json(val),
    }
  }
}

pub struct RedfishReply(pub Reply<RedfishVal>);

impl RedfishReply {
  fn generate_link_header(&mut self) -> Option<()> {
    let data = self.0.data()?;
    if let JsonValue::String(odata_type) = data.val.get("@odata.type")? {
      let pos = odata_type.find('.')?;
      let schema_name = &odata_type[1..pos];
      let Link = format!("</redfish/v1/SchemaStore/en/{}.json>;rel=describedby", schema_name);
      self.0.addHeader(LINK, &Link).ok()?;
    }
    Some(())
  }

  pub(crate) fn get_resource_class_name(&self) -> Option<String> {
    let data = self.0.data()?;
    if let JsonValue::String(odata_type) = data.val.get("@odata.type")? {
      let pos = odata_type.rfind("#")?;
      Some(odata_type[pos + 1..].into())
    } else {
      None
    }
  }

  pub(crate) fn generate_allow_header(&mut self, allow_val: u8) -> Option<()> {
    let allow = AllowHead::from_bits(allow_val)?;

    let mut header = vec![];
    if allow.contains(AllowHead::GET) {
      header.push("GET");
    }

    if allow.contains(AllowHead::PATCH) {
      header.push("PATCH");
    }

    if allow.contains(AllowHead::POST) {
      header.push("POST");
    }

    if allow.contains(AllowHead::DELETE) {
      header.push("DELETE");
    }

    if header.len() > 0 {
      let _ = self.0.addHeader(ALLOW, &header.join(","));
    }
    Some(())
  }
}

impl FromLua for RedfishReply {
  fn from_lua(s: &mut State, index: Index) -> Result<Self> {
    Ok(RedfishReply(s.to_type(index)?))
  }
}

impl ReplyFinish for RedfishReply {
  fn finish(&mut self) {
    self.generate_link_header();
  }
}

fn toErrorInfo(err: ErrorInfoVal) -> JsonValue {
  match err {
    ErrorInfoVal::Value(e) => JsonValue::Array(vec![create_message(e)]),
    ErrorInfoVal::Array(es) => JsonValue::Array(es.into_iter().map(create_message).collect()),
  }
}

fn toRedfishMessage(errMsg: ErrorMessage<RedfishVal>) -> String {
  if let Some(mut data) = errMsg.data {
    if let JsonValue::Object(ref mut obj) = data.val {
      let _ = obj.insert("@Message.ExtendedInfo".to_owned(), toErrorInfo(errMsg.error));
    }
    data.to_string()
  } else {
    json!({
      "error": {
        "@Message.ExtendedInfo": toErrorInfo(errMsg.error),
        "code": "Base.1.0.GeneralError",
        "message": "A general error has occurred. See ExtendedInfo for more information."
      }
    })
    .to_string()
  }
}

impl IntoResponse for RedfishReply {
  fn response(self) -> Response<String> {
    let mut res = match self.0.error {
      Some(e) => Response::new(toRedfishMessage(e)),
      None => match self.0.body {
        Some(b) => Response::new(b.to_string()),
        None => Response::new(String::default()),
      },
    };
    *res.status_mut() = self.0.status;
    match self.0.headers {
      Some(mut headers) => {
        if !headers.contains_key(CONTENT_TYPE) {
          headers.insert(CONTENT_TYPE, HeaderValue::from_static("application/json;charset=utf-8"));
        }
        *res.headers_mut() = headers
      }
      _ => {}
    }
    res
  }
}

impl RedfishReply {
  pub fn isOk(&self) -> bool {
    self.0.isOk()
  }
}

pub fn response_error(
  status: StatusCode,
  code: &'static str,
  msg: Option<JsonValue>,
  props: Option<Vec<String>>,
) -> Response<String> {
  let header = [(CONTENT_TYPE, HeaderValue::from_static("application/json;charset=utf-8"))];
  RedfishReply(Reply::<RedfishVal> {
    status: status,
    headers: Some(HeaderMap::from_iter(header.iter().map(|v| v.clone()))),
    body: None,
    error: Some(ErrorMessage::<RedfishVal>::newMessage(code.to_owned(), msg, props)),
  })
  .response()
}

pub fn response_error_info(status: StatusCode, errs: Vec<ErrorInfo>) -> Response<String> {
  let header = [(CONTENT_TYPE, HeaderValue::from_static("application/json;charset=utf-8"))];
  RedfishReply(Reply::<RedfishVal> {
    status: status,
    headers: Some(HeaderMap::from_iter(header.iter().map(|v| v.clone()))),
    body: None,
    error: Some(ErrorMessage::<RedfishVal> {
      error: ErrorInfoVal::Array(errs),
      data: None,
    }),
  })
  .response()
}

pub fn reject_error_info(
  status: StatusCode,
  code: &str,
  msgs: Option<Vec<String>>,
  props: Option<Vec<String>>,
) -> Rejection {
  reject::custom(ResponseError(response_error_info(
    status,
    vec![ErrorInfo::new(code, msgs, props)],
  )))
}
