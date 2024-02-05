#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(dead_code)]
use libc::c_int;
use std::fmt;

#[cfg(feature = "LUA53")]
#[macro_use]
extern crate scoped_tls;
#[macro_use]
extern crate log;

mod conversion;
mod lua;
mod state;
mod utils;

mod conversion_test;

pub use self::lua::{luaL_Reg, lua_State, LUA_ENVIRONINDEX, LUA_GLOBALSINDEX, LUA_MULTRET, LUA_REGISTRYINDEX};
pub use state::State;

pub type Index = libc::c_int;
pub use self::lua::lua_Integer as Integer;
pub use self::lua::lua_Number as Number;
pub use conversion::{FromLua, JsonOption, ToLua};
pub use utils::{safe_call_closure, wrap_lua_func};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Type {
  None = lua::LUA_TNONE as isize,
  Nil = lua::LUA_TNIL as isize,
  Boolean = lua::LUA_TBOOLEAN as isize,
  LightUserdata = lua::LUA_TLIGHTUSERDATA as isize,
  Number = lua::LUA_TNUMBER as isize,
  String = lua::LUA_TSTRING as isize,
  Table = lua::LUA_TTABLE as isize,
  Function = lua::LUA_TFUNCTION as isize,
  Userdata = lua::LUA_TUSERDATA as isize,
  Thread = lua::LUA_TTHREAD as isize,
}

impl Type {
  fn from_c_int(i: c_int) -> Option<Type> {
    match i {
      lua::LUA_TNIL => Some(Type::Nil),
      lua::LUA_TBOOLEAN => Some(Type::Boolean),
      lua::LUA_TLIGHTUSERDATA => Some(Type::LightUserdata),
      lua::LUA_TNUMBER => Some(Type::Number),
      lua::LUA_TSTRING => Some(Type::String),
      lua::LUA_TTABLE => Some(Type::Table),
      lua::LUA_TFUNCTION => Some(Type::Function),
      lua::LUA_TUSERDATA => Some(Type::Userdata),
      lua::LUA_TTHREAD => Some(Type::Thread),
      _ => None,
    }
  }
}

impl fmt::Display for Type {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    let m = match self {
      Type::Nil => "nil",
      Type::Boolean => "boolean",
      Type::LightUserdata => "lightuserdata",
      Type::Number => "number",
      Type::String => "string",
      Type::Table => "table",
      Type::Function => "function",
      Type::Userdata => "userdata",
      Type::Thread => "thread",
      _ => "none",
    };
    write!(f, "{}", m)
  }
}
