use super::ErrorMessage;
use anyhow::{anyhow, Result};
use base64::{Engine as _, engine::general_purpose};
use libc::c_int;
use libloading::os::unix::Library;
use lua::{luaL_Reg, lua_State, safe_call_closure, wrap_lua_func, FromLua, Index, Integer, State, ToLua, Type};
use once_cell::sync::OnceCell;
use serde::Serialize;
use serde_json::Value as JsonValue;
use std::sync::{Arc, Mutex, Weak};
use std::time::{Duration, SystemTime};
use warp;
use warp::http::{header::HeaderName, HeaderMap, HeaderValue, Response, StatusCode};

scoped_thread_local!(static STATE: Arc<Mutex<State>>);
scoped_thread_local!(static RAWSTATE: *mut lua_State);

pub fn lock_state_call<F, U>(r: &Arc<Mutex<State>>, func: F) -> Result<U>
where
  F: Fn(&mut State) -> Result<U>,
{
  if STATE.is_set() && STATE.with(|state| return Arc::ptr_eq(r, state)) {
    RAWSTATE.with(|L| safe_call_closure(&mut State::from(*L), 0, 0, func))
  } else {
    STATE.set(r, || {
      let state = r.lock().unwrap();
      RAWSTATE.set(&state.L, || {
        RAWSTATE.with(|L| safe_call_closure(&mut State::from(*L), 0, 0, func))
      })
    })
  }
}

#[inline(always)]
pub fn with_state<F, R>(f: F) -> R
where
  F: FnOnce(&Arc<Mutex<State>>) -> R,
{
  STATE.with(f)
}

pub trait Params {
  fn push(&self, state: &mut State) -> Result<i32>;
}

macro_rules! make_params {
  () => {
    impl Params for () {
      fn push(&self, _state: &mut State) -> Result<i32> {
        Ok(0)
      }
    }
  };
  ($p:ident $(, $tail:ident)*) => {
    impl<$p:ToLua $(, $tail:ToLua)*> Params for ($p $(, $tail)* ,) {
      fn push(&self, state: &mut State) -> Result<i32> {
        let ($p $(, $tail)*, ) = self;
        let top = state.get_top();
        $p.to_lua(state)?;
        $( $tail.to_lua(state)?; )*
        Ok(state.get_top() - top)
      }
    }
  };
}

make_params!();
make_params!(A);
make_params!(A, B);
make_params!(A, B, C);
make_params!(A, B, C, D);
make_params!(A, B, C, D, E);
make_params!(A, B, C, D, E, F);
make_params!(A, B, C, D, E, F, G);
make_params!(A, B, C, D, E, F, G, H);
make_params!(A, B, C, D, E, F, G, H, I);
make_params!(A, B, C, D, E, F, G, H, I, J);

pub trait IntoResponse {
  fn response(self) -> Response<String>;
}

pub trait ReplyFinish {
  fn finish(&mut self);
}

pub struct Reply<T> {
  pub status: StatusCode,
  pub headers: Option<HeaderMap>,
  pub body: Option<T>,
  pub error: Option<ErrorMessage<T>>,
}

impl<T> ReplyFinish for Reply<T> {
  fn finish(&mut self) {}
}

fn to_type<T: FromLua>(s: &mut State, index: Index, name: &str) -> Result<T> {
  let _stack = s.save_stack();
  match FromLua::from_lua(s, index) {
    Ok(r) => Ok(r),
    Err(e) => Err(anyhow!("get {} failed: {}", name, e)),
  }
}

fn to_str<'a>(s: &'a mut State, index: Index, name: &str) -> Result<&'a str> {
  let _stack = s.save_stack();
  match s.to_str(index) {
    Ok(r) => Ok(r),
    Err(e) => Err(anyhow!("get {} failed: {}", name, e)),
  }
}

impl<T: FromLua> FromLua for Reply<T> {
  fn from_lua(s: &mut State, index: Index) -> Result<Self> {
    let _stack = s.save_stack();
    let index = s.absindex(index);
    if !s.is_table(index) {
      return Err(anyhow!("expect reply table, got {}", s.typename_of(s.type_of(index)?)));
    }

    s.raw_geti(index, 1); // status
    s.raw_geti(index, 2); // body
    s.raw_geti(index, 3); // headers

    let status = warp::http::StatusCode::from_u16(to_type::<u16>(s, -3, "Reply.StatusCode")?)?;
    let (body, error) = if s.is_nil(-2) {
      (None, None)
    } else {
      match to_type::<ErrorMessage<T>>(s, -2, "Reply.error") {
        Ok(err) => (None, Some(err)),
        Err(_) => (Some(to_type::<T>(s, -2, "Reply.body")?), None),
      }
    };
    let headers = match s.type_of(-1)? {
      Type::Nil => None,
      Type::Table => {
        let mut headers = HeaderMap::new();
        s.push_nil();
        while s.next(-2) {
          let val = HeaderValue::from_str(to_str(s, -1, "Reply.HeaderValue")?)?;
          let key = HeaderName::from_bytes(to_str(s, -2, "Reply.HeaderName")?.as_bytes())?;
          headers.insert(key, val);
          s.pop(1);
        }
        Some(headers)
      }
      t => return Err(anyhow!("reply expect headers table, got {}", s.typename_of(t))),
    };
    Ok(Reply {
      status: status,
      headers: headers,
      body: body,
      error: error,
    })
  }
}

impl<T: ToString + Serialize> ToString for Reply<T> {
  fn to_string(&self) -> String {
    match self.error {
      Some(ref e) => e.to_string(),
      None => match self.body {
        Some(ref b) => b.to_string(),
        None => String::default(),
      },
    }
  }
}

impl<T> Reply<T> {
  pub fn map<U: From<T>>(self) -> Reply<U> {
    Reply {
      status: self.status,
      headers: self.headers,
      body: self.body.map(|v| From::from(v)),
      error: self.error.map(|v| v.map()),
    }
  }
}

impl<T: ToString + Serialize> IntoResponse for Reply<T> {
  fn response(self) -> Response<String> {
    let mut res = Response::new(self.to_string());
    *res.status_mut() = self.status;
    match self.headers {
      Some(headers) => *res.headers_mut() = headers,
      _ => {}
    }
    res
  }
}

impl<T> Reply<T> {
  pub fn isOk(&self) -> bool {
    self.status == StatusCode::OK
  }

  pub fn addHeader(&mut self, name: HeaderName, val: &str) -> Result<()> {
    let val = HeaderValue::from_bytes(val.as_bytes())?;
    if let Some(ref mut headers) = self.headers {
      let _ = headers.insert(name, val);
    } else {
      let mut headers = HeaderMap::new();
      let _ = headers.insert(name, val);
      self.headers = Some(headers);
    }
    Ok(())
  }

  pub fn header<'a>(&'a self, name: HeaderName) -> Option<&'a HeaderValue> {
    self.headers.as_ref()?.get(name)
  }

  pub fn data(&self) -> Option<&T> {
    if let Some(ref err) = self.error {
      err.data.as_ref()
    } else {
      self.body.as_ref()
    }
  }

  pub fn data_mut(&mut self) -> Option<&mut T> {
    if let Some(ref mut err) = self.error {
      err.data.as_mut()
    } else {
      self.body.as_mut()
    }
  }
}

pub extern "C" fn l_log(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let level = state.check_integer(1)?;
    let msg = state.check_string(2)?;
    let lv = match level {
      t if t == log::Level::Error as isize => log::Level::Error,
      t if t == log::Level::Warn as isize => log::Level::Warn,
      t if t == log::Level::Info as isize => log::Level::Info,
      t if t == log::Level::Debug as isize => log::Level::Debug,
      t if t == log::Level::Trace as isize => log::Level::Trace,
      _ => return Ok(0),
    };
    log!(lv, "{}", msg);
    Ok(0)
  })
}

pub extern "C" fn l_logLevel(L: *mut lua_State) -> c_int {
  let mut state = State::from(L);
  state.push_integer(log::max_level() as Integer);
  1
}

pub extern "C" fn l_base64_encode(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let data = state.to_bytes(1)?;
    let out = general_purpose::STANDARD.encode(data);
    state.push_string(&out)?;
    Ok(1)
  })
}

pub extern "C" fn l_base64_decode(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let data = state.check_string(1)?;
    match general_purpose::STANDARD.decode(data) {
      Ok(out) => {
        state.push_bool(true);
        state.push_bytes(&out)?;
      }
      Err(e) => {
        state.push_bool(false);
        state.push_string(&e.to_string())?;
      }
    }
    Ok(2)
  })
}

type LuaState = Weak<Mutex<State>>;
struct RebootHandle(LuaState, c_int);
type RebootHandler = extern "C" fn(rc: c_int) -> i32;
type RegisterRebootHandler = extern "C" fn(RebootHandler) -> i32;
static DFL_REGISTER_REBOOT_HANDLER: OnceCell<RegisterRebootHandler> = OnceCell::new();
lazy_static! {
  static ref REBOOT_HANDLER: Arc<Mutex<Vec<RebootHandle>>> = Arc::new(Mutex::new(Vec::new()));
}

impl Drop for RebootHandle {
  fn drop(&mut self) {
    let href = self.1;
    if let Some(s) = self.0.upgrade() {
      let _ = lock_state_call(&s, |s| {
        s.unreference(lua::LUA_REGISTRYINDEX, href);
        Ok(())
      });
    }
  }
}

pub(crate) fn init_portal(dflib: &Library) -> Result<()> {
  unsafe {
    let f = dflib.get::<RegisterRebootHandler>(b"static_register_reboot_handler\0")?;
    let _ = DFL_REGISTER_REBOOT_HANDLER.set(*f);
  }
  Ok(())
}

fn run_reboot_handler(s: &LuaState, href: c_int, rc: c_int, dt: Duration) -> Result<Integer> {
  let state = s.upgrade().ok_or_else(|| anyhow!("invalid state"))?;
  lock_state_call(&state, |state| {
    let _stack = state.save_stack();
    state.raw_geti(lua::LUA_REGISTRYINDEX, href as Integer);
    state.push_integer(rc as Integer);
    state.push_integer(dt.as_millis() as Integer);
    state.pcall(2, 1, 0)?;
    state.to_integer(-1)
  })
}

const DFL_OK: Integer = 0;
fn do_reboot_handler(rc: c_int, dt: Duration) -> i32 {
  let handlers = REBOOT_HANDLER.lock().unwrap();
  for h in handlers.iter() {
    match run_reboot_handler(&h.0, h.1, rc, dt) {
      Ok(result) if result != DFL_OK => return result as i32,
      Err(e) => error!("do reboot handler: {}", e),
      _ => {}
    }
  }
  DFL_OK as i32
}

extern "C" fn on_reboot_handler(rc: c_int) -> i32 {
  let now = SystemTime::now();
  loop {
    let ret = do_reboot_handler(rc, now.elapsed().unwrap_or(Duration::from_secs(60)));
    if ret >= 0 {
      return ret;
    }
    std::thread::sleep(Duration::from_millis(100));
  }
}

pub extern "C" fn l_register_reboot_handler(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    state.check_type(1, Type::Function)?;
    let register_func = DFL_REGISTER_REBOOT_HANDLER.get().ok_or_else(|| {
      error!("regist reboot handle failed: invalid static_register_reboot_handler");
      anyhow!("invalid DFL_REGISTER_REBOOT_HANDLER")
    })?;
    register_func(on_reboot_handler);

    let mut reboot_handler = REBOOT_HANDLER.lock().unwrap();

    state.push_value(1);
    let href = state.reference(lua::LUA_REGISTRYINDEX);

    reboot_handler.push(RebootHandle(with_state(|s| Arc::downgrade(s)), href));
    Ok(0)
  })
}

pub(super) fn open_portal(state: &mut State) {
  let _stack = state.save_stack();
  let l = vec![
    luaL_Reg::new(b"log\0", Some(l_log)),
    luaL_Reg::new(b"logLevel\0", Some(l_logLevel)),
    luaL_Reg::new(b"base64_encode\0", Some(l_base64_encode)),
    luaL_Reg::new(b"base64_decode\0", Some(l_base64_decode)),
    luaL_Reg::new(b"register_reboot_handler\0", Some(l_register_reboot_handler)),
    luaL_Reg::null(),
  ];
  state.register(b"portal\0", &l);
}

#[cfg(test)]
mod test {
  use super::*;
  use anyhow::Result;

  #[test]
  fn test_base64() -> Result<()> {
    let mut state = State::new();
    state.open_libs()?;
    open_portal(&mut state);
    assert_eq!(state.get_top(), 0);

    let data = "123456";
    state.load_string(r#"return portal.base64_encode(...)"#)?;
    state.push_string(data)?;
    state.pcall(1, lua::LUA_MULTRET, 0)?;
    assert_eq!(state.get_top(), 1);
    let encoded_data = state.to_str(-1)?.to_owned();
    state.pop(1);

    state.load_string(r#"return portal.base64_decode(...)"#)?;
    state.push_string(&encoded_data)?;
    state.pcall(1, lua::LUA_MULTRET, 0)?;
    assert_eq!(state.get_top(), 2);
    assert_eq!(state.to_bool(-2), true);
    assert_eq!(state.to_str(-1)?, data);
    Ok(())
  }
}

pub fn fmt_json(val: &JsonValue) -> String {
  match val {
    JsonValue::String(s) => s.clone(),
    _ => val.to_string(),
  }
}
