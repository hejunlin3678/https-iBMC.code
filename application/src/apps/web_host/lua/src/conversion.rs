use super::{Index, Integer, Number, State, Type};
use anyhow::{anyhow, Result};
use bytes::Bytes;
use libc::c_int;
use serde::{Deserialize, Deserializer};
use serde_json::{map::Map as JsonMap, Number as JsonNumber, Value as JsonValue};

pub trait ToLua {
  fn to_lua(&self, L: &mut State) -> Result<()>;
}

impl ToLua for String {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.push_string(&self)?;
    Ok(())
  }
}

impl<T: ToLua> ToLua for Vec<T> {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.create_table(self.len() as libc::c_int, 0)?;
    for (i, v) in self.into_iter().enumerate() {
      v.to_lua(L)?;
      L.raw_seti(-2, (i + 1) as Integer)?;
    }
    Ok(())
  }
}

impl ToLua for &str {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.push_string(self)?;
    Ok(())
  }
}

impl ToLua for bool {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.push_bool(*self);
    Ok(())
  }
}

impl ToLua for Bytes {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.push_bytes(&self)?;
    Ok(())
  }
}

impl<T: ToLua> ToLua for Option<T> {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    match self {
      Some(v) => v.to_lua(L),
      None => {
        L.push_nil();
        Ok(())
      }
    }
  }
}

impl ToLua for std::net::SocketAddr {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.push_string(&self.ip().to_string())?;
    Ok(())
  }
}

impl ToLua for JsonValue {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    match self {
      JsonValue::Null => L.push_nil(),
      JsonValue::Number(v) => {
        if let Some(d) = v.as_i64() {
          L.push_integer(d as Integer);
        } else if let Some(d) = v.as_u64() {
          L.push_integer(d as Integer);
        } else if let Some(d) = v.as_f64() {
          L.push_number(d as Number);
        } else {
          L.push_nil();
        }
      }
      JsonValue::String(v) => L.push_string(&v)?,
      JsonValue::Bool(v) => L.push_bool(*v),
      JsonValue::Array(v) => {
        L.create_table(v.len() as c_int, 0)?;
        for (i, d) in v.iter().enumerate() {
          d.to_lua(L)?;
          L.raw_seti(-2, (i + 1) as Integer)?;
        }
      }
      JsonValue::Object(v) => {
        L.create_table(0, 0)?;
        for (k, d) in v.iter() {
          L.push_string(&k)?;
          d.to_lua(L)?;
          L.set_table(-3)?;
        }
      }
    }
    Ok(())
  }
}

macro_rules! to_lua_int {
  ($x:ty) => {
    impl ToLua for $x {
      fn to_lua(&self, L: &mut State) -> Result<()> {
        if let Some(i) = num_traits::cast(*self) {
          L.push_integer(i);
          Ok(())
        } else {
          Err(anyhow!("from {} to number out of range", stringify!($x)))
        }
      }
    }
  };
}

to_lua_int!(i8);
to_lua_int!(u8);
to_lua_int!(i16);
to_lua_int!(u16);
to_lua_int!(i32);
to_lua_int!(u32);
to_lua_int!(i64);
to_lua_int!(u64);

pub trait FromLua: Sized {
  const N_INDEX: i32 = 1;
  fn from_lua(state: &mut State, index: Index) -> Result<Self>;
}

impl FromLua for String {
  fn from_lua(state: &mut State, index: Index) -> Result<String> {
    state.to_str(index).map(ToOwned::to_owned)
  }
}

impl<T: FromLua> FromLua for Vec<T> {
  fn from_lua(state: &mut State, index: Index) -> Result<Self> {
    match state.type_of(index)? {
      Type::Nil => Ok(vec![]),
      Type::Table => {
        let mut vals = vec![];
        let len = state.raw_len(index);
        for i in 1..=len {
          state.raw_geti(index, i as Integer);
          vals.push(state.to_type(-1)?);
          state.pop(1);
        }
        Ok(vals)
      }
      t => Err(anyhow!("expect table, got {}", state.typename_of(t))),
    }
  }
}

impl FromLua for Integer {
  fn from_lua(state: &mut State, index: Index) -> Result<Integer> {
    state.to_integer(index)
  }
}

impl FromLua for Number {
  fn from_lua(state: &mut State, index: Index) -> Result<Number> {
    state.to_number(index)
  }
}

impl FromLua for bool {
  fn from_lua(state: &mut State, index: Index) -> Result<bool> {
    Ok(state.to_bool(index))
  }
}

impl FromLua for Bytes {
  fn from_lua(state: &mut State, index: Index) -> Result<Bytes> {
    state.to_bytes(index).map(|v| v.to_owned().into())
  }
}

impl FromLua for JsonValue {
  fn from_lua(state: &mut State, index: Index) -> Result<JsonValue> {
    match state.type_of(index)? {
      Type::Nil => Ok(JsonValue::Null),
      Type::Boolean => Ok(JsonValue::Bool(state.to_bool(index))),
      Type::Number => {
        let val = state.to_number(index)?;
        if val.trunc() == val {
          if let Some(v) = num_traits::cast::<_, u64>(val) {
            return Ok(JsonValue::Number(JsonNumber::from(v)));
          } else if let Some(v) = num_traits::cast::<_, i64>(val) {
            return Ok(JsonValue::Number(JsonNumber::from(v)));
          }
        }
        match JsonNumber::from_f64(val) {
          Some(n) => Ok(JsonValue::Number(n)),
          None => Ok(JsonValue::Null),
        }
      }
      Type::String => Ok(JsonValue::String(state.to_str(index)?.into())),
      Type::Table => {
        state.push_value(index);
        let len = state.raw_len(-1);
        let value = if len == 0 {
          let mut obj = JsonMap::new();
          state.push_nil();
          while state.next(-2) {
            let value = state.to_type::<JsonValue>(-1)?;
            let key = state.to_str(-2)?;
            obj.insert(key.into(), value);
            state.pop(1);
          }
          if obj.len() == 0 {
            JsonValue::Array(vec![])
          } else {
            JsonValue::Object(obj)
          }
        } else {
          let mut arr = vec![];
          arr.reserve(len);
          for i in 1..=len {
            state.raw_geti(-1, i as Integer);
            arr.push(state.to_type::<JsonValue>(-1)?);
            state.pop(1);
          }
          JsonValue::Array(arr)
        };
        state.pop(1);
        Ok(value)
      }
      Type::LightUserdata if state.to_userdata::<u8>(index).is_null() => Ok(JsonValue::Null),
      Type::LightUserdata if state.is_empty_object(index) => Ok(JsonValue::Object(JsonMap::new())),
      t => Err(anyhow!("invalid json value: {}", state.typename_of(t))),
    }
  }
}

impl<T: FromLua> FromLua for Option<T> {
  fn from_lua(state: &mut State, index: Index) -> Result<Option<T>> {
    if state.is_nil(index) {
      Ok(None)
    } else {
      Ok(Some(state.to_type(index)?))
    }
  }
}

macro_rules! from_lua_int {
  ($x:ty) => {
    impl FromLua for $x {
      fn from_lua(state: &mut State, index: Index) -> Result<$x> {
        let val: Integer = FromLua::from_lua(state, index)?;
        if let Some(i) = num_traits::cast(val) {
          Ok(i)
        } else {
          Err(anyhow!("from Integer to {} out of range", stringify!($x)))
        }
      }
    }
  };
}

from_lua_int!(i8);
from_lua_int!(u8);
from_lua_int!(i16);
from_lua_int!(u16);
from_lua_int!(i32);
from_lua_int!(u32);
from_lua_int!(i64);
from_lua_int!(u64);

#[macro_export]
macro_rules! to_lua {
  ($name:ident, $($field:ident),+) => {
    impl lua::ToLua for $name {
      fn to_lua(&self, L: &mut crate::lua::State) -> crate::anyhow::Result<()> {
        L.new_table()?;
        $(
          L.push_string(&stringify!($field))?;
          self.$field.to_lua(L)?;
          L.set_table(-3)?;
        )+
        Ok(())
      }
    }
  }
}

#[macro_export]
macro_rules! from_lua {
  ($name:ident, $($field:ident),+) => {
    impl lua::FromLua for $name {
      fn from_lua(L: &mut crate::lua::State, index: crate::lua::Index) -> crate::anyhow::Result<$name> {
        if !L.is_table(index) {
          return Err(crate::anyhow::anyhow!("expect {}, got {}", stringify!($name), L.typename_of(L.type_of(index)?)));
        }
        let index = L.absindex(index);
        $(
          L.push_string(&stringify!($field))?;
          L.get_table(index);
          let $field = match L.to_type(-1) {
            Ok(v) => v,
            Err(e) => return Err(crate::anyhow::anyhow!("get field {}.{} failed: {}", &stringify!($name), &stringify!($field), e))
          };
          L.pop(1);
        )+
        Ok($name {
          $($field),+
        })
      }
    }
  }
}

#[macro_export]
macro_rules! from_to_lua {
  ($name:ident, $($field:ident),+) => {
    to_lua!($name, $($field),+);
    from_lua!($name, $($field),+);
  }
}

/// JsonOption
/// Option that can represent json::null
///
#[derive(Debug, Clone)]
pub enum JsonOption<T> {
  None,
  Null,
  Value(T),
}

impl<T> Default for JsonOption<T> {
  fn default() -> Self {
    JsonOption::None
  }
}

impl<T> From<Option<T>> for JsonOption<T> {
  fn from(opt: Option<T>) -> Self {
    match opt {
      Some(v) => JsonOption::Value(v),
      None => JsonOption::Null,
    }
  }
}

impl<'de, T> Deserialize<'de> for JsonOption<T>
where
  T: serde::de::DeserializeOwned,
{
  fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
  where
    D: Deserializer<'de>,
  {
    match JsonValue::deserialize(deserializer) {
      Ok(JsonValue::Null) => Ok(JsonOption::Null),
      Ok(v) => Ok(JsonOption::Value(T::deserialize(v).map_err(serde::de::Error::custom)?)),
      Err(e) if e.to_string().starts_with("missing field") => return Ok(JsonOption::None),
      Err(e) => return Err(e),
    }
  }
} 

impl<T: ToLua> ToLua for JsonOption<T> {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    match self {
      JsonOption::Value(v) => v.to_lua(L),
      JsonOption::Null => {
        L.push_null();
        Ok(())
      }
      JsonOption::None => {
        L.push_nil();
        Ok(())
      }
    }
  }
}
