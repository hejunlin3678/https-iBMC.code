use super::config::get_registries_file_path;
use anyhow::Result;
use core::{fmt_json, ErrorInfo};
use libc::c_int;
use lua::{luaL_Reg, lua_State, wrap_lua_func, State, ToLua};
use once_cell::sync::OnceCell;
use regex::{Captures, Regex};
use serde::{Deserialize, Deserializer};
use serde_json::{json, Value as JsonValue};
use std::collections::HashMap;
use std::sync::Mutex;

const MAX_MESSAGE_ARGS_LEN: usize = 512;
const OMISSION_STR: &str = "...";

const ODataType: &'static str = "#Message.v1_0_0.Message";

#[derive(Deserialize)]
struct Message {
  Description: Option<String>,
  Message: String,
  Severity: String,
  NumberOfArgs: u32,
  ParamTypes: Option<Vec<String>>,
  Resolution: Option<String>,
}

#[derive(Deserialize)]
struct Registry {
  #[serde(rename(deserialize = "@Redfish.Copyright"))]
  Copyright: Option<String>,
  #[serde(rename(deserialize = "@odata.type"))]
  Type: String,
  Id: String,
  Name: String,
  Language: String,
  Description: String,
  RegistryPrefix: String,
  RegistryVersion: String,
  OwningEntity: String,

  #[serde(deserialize_with = "de_message")]
  Messages: HashMap<String, Message>,

  #[serde(skip)]
  MessageIdPrefix: String,
}

fn de_message<'de, D>(deserializer: D) -> Result<HashMap<String, Message>, D::Error>
where
  D: Deserializer<'de>,
{
  #[derive(Deserialize)]
  struct Wrapper(Message);

  let v = HashMap::<String, Wrapper>::deserialize(deserializer)?;
  Ok(v.into_iter().map(|(k, Wrapper(v))| (k, v)).collect())
}

fn limit_msg_arg(v: &JsonValue) -> String {
  let mut s = fmt_json(v);
  if s.len() >= MAX_MESSAGE_ARGS_LEN {
    s.truncate(MAX_MESSAGE_ARGS_LEN - OMISSION_STR.len() - 1);
    s.push_str(OMISSION_STR);
  }
  s
}

fn get_args(e: &ErrorInfo, msg: &Message, reg: &Registry) -> Vec<String> {
  if msg.NumberOfArgs > 0 {
    match e.message {
      Some(JsonValue::Array(ref args)) if args.len() as u32 == msg.NumberOfArgs => {
        return args.iter().map(|v| limit_msg_arg(v)).collect()
      }
      Some(ref message) => {
        error!(
          "{}{}: expect {} args, go {}",
          reg.MessageIdPrefix, e.code, msg.NumberOfArgs, message
        );
      }
      _ => {}
    }
  }
  vec![]
}

fn get_properties(e: &ErrorInfo) -> Vec<String> {
  match &e.properties {
    Some(props) => props.iter().map(|v| format!("#/{}", v)).collect(),
    None => vec![],
  }
}

fn get_message(args: &Vec<String>, numberOfArgs: u32, fmt: &str) -> String {
  if !args.is_empty() && args.len() as u32 == numberOfArgs {
    if let Ok(re) = Regex::new(r"%(\d+)") {
      return re
        .replace_all(fmt, |caps: &Captures| {
          if let Ok(id) = caps[1].parse::<usize>() {
            if id >= 1 && id <= args.len() {
              return args[id - 1].clone();
            }
          }
          format!("%{}", &caps[1])
        })
        .to_string();
    }
  }
  fmt.to_owned()
}

fn format(e: &ErrorInfo, reg: &Registry, msg: &Message) -> JsonValue {
  let args = get_args(&e, &msg, reg);
  let mut val = json!({
    "MessageArgs" : args,
    "RelatedProperties" : get_properties(&e),
    "Severity": msg.Severity,
    "MessageId": reg.MessageIdPrefix.clone() + &e.code,
    "Message": get_message(&args, msg.NumberOfArgs, &msg.Message),
    "@odata.type": ODataType,
  });
  if let Some(ref resolution) = msg.Resolution {
    val["Resolution"] = JsonValue::String(resolution.clone())
  }
  val
}

// 消息模板文件懒加载
struct RegistryFile {
  registries: Mutex<Option<Registry>>,
  filePath: std::path::PathBuf,
  len: u64,
}

impl RegistryFile {
  fn new(path: &std::path::Path, len: u64) -> Self {
    RegistryFile {
      registries: Mutex::new(None),
      filePath: path.into(),
      len,
    }
  }

  fn load_file(&self) -> Result<Registry> {
    let file = std::fs::File::open(self.filePath.as_path())?;
    let mut reg: Registry = serde_json::from_reader(file)?;
    let re = Regex::new(r"^(?P<major_minor>\d+\.\d+)(?P<aux_ver>\.\d+)$")?;
    let version = re.replace(&reg.RegistryVersion, ".$major_minor.");
    reg.MessageIdPrefix = reg.RegistryPrefix.clone() + &version;
    Ok(reg)
  }

  fn render(&self, e: &ErrorInfo) -> Option<JsonValue> {
    let mut reg = self.registries.lock().unwrap();
    if let Some(r) = &mut *reg {
      return r.Messages.get(&e.code).map(|msg| format(e, r, msg));
    }

    info!(
      "load registry file {}",
      self.filePath.file_name().map_or(None, |v| v.to_str()).unwrap_or("")
    );
    match self.load_file() {
      Ok(r) => {
        let msg = r.Messages.get(&e.code).map(|msg| format(e, &r, msg));
        *reg = Some(r);
        msg
      }
      Err(e) => {
        error!("load registry file failed: {}", e);
        None
      }
    }
  }
}

static REGISTRIES: OnceCell<Vec<RegistryFile>> = OnceCell::new();

pub fn load_registris() -> Result<()> {
  let mut tmp = vec![];
  match get_registries_file_path() {
    Some(dir) if dir.is_dir() => {
      for entry in std::fs::read_dir(dir)? {
        let path = entry?.path();
        if path.is_file() {
          if let Some(true) = path.extension().and_then(|ext| Some(ext == "json")) {
            let metadata = std::fs::metadata(&path)?;
            tmp.push(RegistryFile::new(&path, metadata.len()));
          }
        }
      }
    }
    Some(_) => {
      info!("registries file is not dir");
      return Ok(());
    },
    _ => {
      info!("registries file dir not set");
      return Ok(());
    }
  }

  // 按文件大小排序,优先加载小文件
  tmp.sort_by_key(|v| v.len);
  let _ = REGISTRIES.set(tmp);

  // 启动后台线程加载资源
  std::thread::spawn(|| {
    if let Some(regs) = REGISTRIES.get() {
      let emptyError = ErrorInfo {
        code: String::default(),
        message: None,
        properties: None,
      };
      for reg in regs.iter() {
        let _ = reg.render(&emptyError);
      }
    }
  });
  Ok(())
}

fn render_msg(e: &ErrorInfo) -> Option<JsonValue> {
  match REGISTRIES.get() {
    Some(_) => {},
    None => {load_registris().unwrap();}
 }

  if let Some(regs) = REGISTRIES.get() {
    for reg in regs.iter() {
      if let Some(msg) = reg.render(&e) {
        return Some(msg);
      }
    }
  }
  None
}

static DEFAULT_ERROR: OnceCell<JsonValue> = OnceCell::new();

fn default_error() -> JsonValue {
  if let Some(e) = DEFAULT_ERROR.get() {
    return e.clone();
  }

  let v = render_msg(&ErrorInfo {
    code: "InternalError".into(),
    message: None,
    properties: None,
  })
  .unwrap_or(json!({
    "@odata.type": ODataType,
    "MessageId": "Base.1.0.GeneralError",
    "Message": "A general error has occurred. See ExtendedInfo for more information.",
    "MessageArgs": Vec::<String>::default(),
    "RelatedProperties" : Vec::<String>::default(),
    "Severity": "Critical",
    "Resolution": "None."
  }));
  let _ = DEFAULT_ERROR.set(v.clone());
  v
}

pub fn create_message(e: ErrorInfo) -> JsonValue {
  if e.code == "FirmwareUploadError" {
    return json!({
      "@odata.type": ODataType,
      "MessageId": "iBMC.0.1.0.FirmwareUploadError",
      "Message": e.message,
      "MessageArgs": Vec::<String>::default(),
      "RelatedProperties" : Vec::<String>::default(),
      "Severity": "Warning",
      "Resolution": "Locate the cause based on error information, rectify the fault, and submit the request again."
    });
  }

  let mut err = match render_msg(&e) {
    Some(v) => v,
    _ => default_error(),
  };

  if e.code == "InternalError" {
    let msg = serde_json::to_string(&e).unwrap_or(String::default());
    error!("redfish InternalError: {}", msg);
    err.get_mut("Message").map(|Message| *Message = JsonValue::String(msg));
  }

  err
}

pub(super) extern "C" fn l_create_message(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let err: ErrorInfo = state.to_type(1)?;
    create_message(err).to_lua(state)?;
    Ok(1)
  })
}

pub fn open_redfish_registry(state: &mut State) -> Result<()> {
  let _stack = state.save_stack();
  let l = vec![
    luaL_Reg::new(b"create_message\0", Some(l_create_message)),
    luaL_Reg::null(),
  ];
  state.register(b"redfish.registry\0", &l);
  Ok(())
}

#[cfg(test)]
mod test {
  use super::*;
  use anyhow::Result;

  #[test]
  fn test_format_message() -> Result<()> {
    let tests: &[(Vec<&str>, &str, &str)] = &[
      (vec!["1", "2", "3"], "%1 + %2 = %3;", "1 + 2 = 3;"),
      (vec!["2", "1", "3"], "%1 + %1 = %2 + %3;", "2 + 2 = 1 + 3;"),
      (vec!["2", "1", "3"], "%1 + %4 = %2 + %3;", "2 + %4 = 1 + 3;"),
    ];
    for test in tests {
      let res = get_message(
        &test.0.iter().map(|v| v.to_string()).collect(),
        test.0.len() as u32,
        test.1,
      );
      assert_eq!(res, test.2);
    }
    Ok(())
  }
}
