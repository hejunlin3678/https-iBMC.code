use bytes::Bytes;
use redfish_base::AuthInfo;
use serde_json::Value as JsonValue;

define_param! {
  #[portal(ToLua)]
  pub struct SmsReq {
    Method: String,
    Path: String,
    Headers: JsonValue,
    Body: Bytes,
    Ip: Option<String>,
    Port: Option<String>,
  }
}

define_param! {
  #[portal(ToLua)]
  pub struct RedfishReq {
    Method: String,
    Path: String,
    Query: String,
    Headers: JsonValue,
    Body: Bytes,
    Ip: Option<String>,
    Port: Option<String>,
    AuthInfo: AuthInfo,
  }
}
