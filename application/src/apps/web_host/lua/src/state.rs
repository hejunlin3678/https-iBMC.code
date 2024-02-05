use super::lua;
use super::safe_call_closure;
use super::utils::json_lightudata_mask;
use super::Type;
use super::{FromLua, Index, Integer, Number};
use anyhow::{anyhow, Result};
use libc::{c_char, c_int, c_void, size_t};
use std::ffi::{CStr, CString};
use std::slice;
use std::str;

static EMPTY_OBJECT: i32 = 0;

#[cfg(feature = "LUA53")]
scoped_thread_local!(static G_L: *const lua::luaL_Reg);

#[cfg(feature = "LUA53")]
extern "C" fn l_openlib(L: *mut lua::lua_State) -> c_int {
  G_L.with(|l| unsafe {
    lua::lua_createtable(L, 0, 0);
    lua::luaL_setfuncs(L, *l, 0);
  });
  1
}

#[macro_export]
macro_rules! cstr {
  ($e: expr) => {
    concat!($e, "\0").as_ptr() as *const libc::c_char
  };
}
pub struct StackSave {
  L: *mut lua::lua_State,
  idx: c_int,
}

impl Drop for StackSave {
  fn drop(&mut self) {
    unsafe { lua::lua_settop(self.L, self.idx) }
  }
}

pub struct State {
  pub L: *mut lua::lua_State,
  owned: bool,
}

unsafe impl Send for State {}

unsafe extern "C" fn l_traceback(L: *mut lua::lua_State) -> c_int {
  let mut len = 0;
  let mut state = State::from(L);
  let ptr = lua::lua_tolstring(L, -1, &mut len);
  if ptr.is_null() {
    state.push_string("(traceback no error)").unwrap_or(());
  } else {
    let slice = slice::from_raw_parts(ptr as *const u8, len as usize);
    match str::from_utf8(slice) {
      Ok(msg) => state.traceback(L, msg, 1),
      Err(e) => state.traceback(L, &e.to_string(), 1),
    }
  }
  1
}

impl State {
  pub fn new() -> State {
    let mut state = State {
      L: unsafe { lua::luaL_newstate() },
      owned: true,
    };
    state.regist_global_error_str();

    state.push_lightuserdata(json_lightudata_mask(&EMPTY_OBJECT));
    if let Err(e) = state.set_global("__empty_object") {
      error!("set global __empty_object failed: {}", e);
    }

    state
  }

  pub fn from(L: *mut lua::lua_State) -> State {
    State { L: L, owned: false }
  }

  pub fn open_libs(&mut self) -> Result<()> {
    safe_call_closure(self, 0, 0, |state: &mut State| {
      unsafe { lua::luaL_openlibs(state.L) };
      Ok(())
    })
  }

  pub fn open_base(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_base)
  }

  pub fn open_package(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_package)
  }

  pub fn open_table(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_table)
  }

  pub fn open_io(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_io)
  }

  pub fn open_os(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_os)
  }

  pub fn open_string(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_string)
  }

  pub fn open_math(&mut self) -> Result<()> {
    self.open_lib(lua::luaopen_math)
  }

  pub fn open_bit(&mut self) -> Result<()> {
    #[cfg(feature = "LUA51")]
    self.open_lib(lua::luaopen_bit)?;
    Ok(())
  }

  pub fn open_jit(&mut self) -> Result<()> {
    #[cfg(feature = "LUA51")]
    self.open_lib(lua::luaopen_jit)?;
    Ok(())
  }

  fn open_lib(&mut self, f: lua::lua_CFunction) -> Result<()> {
    safe_call_closure(self, 0, 0, |state: &mut State| {
      unsafe {
        lua::lua_pushcclosure(state.L, f, 0);
        lua::lua_call(state.L, 0, 0);
      }
      Ok(())
    })
  }

  #[inline(always)]
  pub fn save_stack(&mut self) -> StackSave {
    StackSave {
      L: self.L,
      idx: self.get_top(),
    }
  }

  pub fn load_file(&mut self, filename: &str) -> Result<()> {
    let c_str = CString::new(filename)?;
    match safe_call_closure(self, 0, 1, |state| unsafe {
      Ok(lua::luaL_loadfile(state.L, c_str.as_ptr()))
    })? {
      lua::LUA_OK => Ok(()),
      _ => Err(anyhow!("{}", self.to_str(-1).unwrap_or("lua error"))),
    }
  }

  pub fn do_file(&mut self, filename: &str) -> Result<()> {
    self.load_file(filename)?;
    self.pcall(0, lua::LUA_MULTRET, 0)
  }

  pub fn load_string(&mut self, s: &str) -> Result<()> {
    let c_str = CString::new(s)?;
    match safe_call_closure(self, 0, 1, |state| unsafe {
      Ok(lua::luaL_loadstring(state.L, c_str.as_ptr()))
    })? {
      lua::LUA_OK => Ok(()),
      _ => Err(anyhow!("{}", self.to_str(-1).unwrap_or("lua error"))),
    }
  }

  pub fn do_string(&mut self, s: &str) -> Result<()> {
    self.load_string(s)?;
    self.pcall(0, lua::LUA_MULTRET, 0)
  }

  pub fn to_integer(&mut self, index: Index) -> Result<Integer> {
    if self.is_integer(index) || self.is_number(index) {
      Ok(unsafe { lua::lua_tointeger(self.L, index) })
    } else {
      Err(anyhow!(
        "expect Integer, got {}",
        self.typename_of(self.type_of(index)?)
      ))
    }
  }

  pub fn to_number(&mut self, index: Index) -> Result<Number> {
    if self.is_integer(index) || self.is_number(index) {
      Ok(unsafe { lua::lua_tonumber(self.L, index) })
    } else {
      Err(anyhow!("expect Number, got {}", self.typename_of(self.type_of(index)?)))
    }
  }

  pub fn to_str(&self, index: Index) -> Result<&str> {
    let mut len = 0;
    let ptr = unsafe { lua::lua_tolstring(self.L, index, &mut len) };
    if ptr.is_null() {
      Err(anyhow!("expect string, got {}", self.typename_of(self.type_of(index)?)))
    } else {
      let slice = unsafe { slice::from_raw_parts(ptr as *const u8, len as usize) };
      Ok(str::from_utf8(slice)?)
    }
  }

  pub fn to_bytes(&self, index: Index) -> Result<&[u8]> {
    let mut len = 0;
    let ptr = unsafe { lua::lua_tolstring(self.L, index, &mut len) };
    if ptr.is_null() {
      Err(anyhow!("expect bytes, got {}", self.typename_of(self.type_of(index)?)))
    } else {
      let slice = unsafe { slice::from_raw_parts(ptr as *const u8, len as usize) };
      Ok(slice)
    }
  }

  pub fn to_bool(&self, index: Index) -> bool {
    let result = unsafe { lua::lua_toboolean(self.L, index) };
    result != 0
  }

  pub fn to_userdata<T>(&self, index: Index) -> *mut T {
    let result = unsafe { lua::lua_touserdata(self.L, index) };
    result as *mut T
  }

  pub fn is_number(&self, index: Index) -> bool {
    unsafe { lua::lua_isnumber(self.L, index) == 1 }
  }

  pub fn is_string(&self, index: Index) -> bool {
    unsafe { lua::lua_isstring(self.L, index) == 1 }
  }

  pub fn is_integer(&self, index: Index) -> bool {
    unsafe { lua::lua_isinteger(self.L, index) == 1 }
  }

  pub fn is_userdata(&self, index: Index) -> bool {
    unsafe { lua::lua_isuserdata(self.L, index) == 1 }
  }

  pub fn is_table(&self, index: Index) -> bool {
    unsafe { lua::lua_istable(self.L, index) == 1 }
  }

  pub fn is_nil(&self, index: Index) -> bool {
    unsafe { lua::lua_isnil(self.L, index) == 1 }
  }

  pub fn is_empty_object(&self, index: Index) -> bool {
    unsafe {
      if self.is_userdata(index) {
        let p = lua::lua_touserdata(self.L, index) as *const c_void;
        let pEmptyObj = json_lightudata_mask(&EMPTY_OBJECT) as *const c_void;
        p == pEmptyObj
      } else {
        false
      }
    }
  }

  pub fn is_function(&self, index: Index) -> bool {
    unsafe { lua::lua_isfunction(self.L, index) == 1 }
  }

  pub fn check_integer(&mut self, index: Index) -> Result<Integer> {
    self.push_value(index);
    safe_call_closure(self, 1, 0, |state| Ok(unsafe { lua::luaL_checkinteger(state.L, -1) }))
  }

  pub fn check_string(&mut self, index: Index) -> Result<&str> {
    self.push_value(index);
    safe_call_closure(self, 1, 0, |state| {
      let mut len = 0;
      let slice = unsafe {
        let ptr = lua::luaL_checklstring(state.L, -1, &mut len);
        slice::from_raw_parts(ptr as *const u8, len as usize)
      };
      Ok(str::from_utf8(slice)?)
    })
  }

  pub fn check_type(&mut self, index: Index, t: Type) -> Result<()> {
    self.push_value(index);
    safe_call_closure(self, 1, 0, |state| {
      unsafe { lua::luaL_checktype(state.L, -1, t as c_int) };
      Ok(())
    })
  }

  pub fn pop(&mut self, n: c_int) {
    unsafe { lua::lua_pop(self.L, n) }
  }

  pub fn new_table(&mut self) -> Result<()> {
    safe_call_closure(self, 0, 1, |state| {
      unsafe { lua::lua_newtable(state.L) };
      Ok(())
    })
  }

  pub fn create_table(&mut self, narr: c_int, nrec: c_int) -> Result<()> {
    safe_call_closure(self, 0, 1, |state| {
      unsafe { lua::lua_createtable(state.L, narr, nrec) };
      Ok(())
    })
  }

  pub fn set_top(&mut self, index: Index) {
    unsafe { lua::lua_settop(self.L, index) }
  }

  #[inline(always)]
  pub fn get_top(&mut self) -> Index {
    unsafe { lua::lua_gettop(self.L) }
  }

  pub fn pcall(&mut self, nargs: c_int, nresults: c_int, msgh: c_int) -> Result<()> {
    let result = unsafe { lua::lua_pcall(self.L, nargs, nresults, msgh) };
    if result != lua::LUA_OK {
      Err(anyhow!("{}", self.to_str(-1).unwrap_or("lua error")))
    } else {
      Ok(())
    }
  }

  pub fn set_global(&mut self, var: &str) -> Result<()> {
    let c_str = CString::new(var)?;
    safe_call_closure(self, 1, 0, |state| {
      unsafe { lua::lua_setglobal(state.L, c_str.as_ptr()) };
      Ok(())
    })
  }

  pub fn push_string(&mut self, s: &str) -> Result<()> {
    safe_call_closure(self, 0, 1, |state| {
      unsafe { lua::lua_pushlstring(state.L, s.as_ptr() as *const _, s.len() as size_t) };
      Ok(())
    })
  }

  pub fn push_bytes(&mut self, s: &[u8]) -> Result<()> {
    safe_call_closure(self, 0, 1, |state| {
      unsafe { lua::lua_pushlstring(state.L, s.as_ptr() as *const _, s.len() as size_t) };
      Ok(())
    })
  }

  pub fn push_nil(&mut self) {
    unsafe { lua::lua_pushnil(self.L) }
  }

  pub fn push_null(&mut self) {
    unsafe { lua::lua_pushlightuserdata(self.L, std::ptr::null()) }
  }

  pub fn push_number(&mut self, n: Number) {
    unsafe { lua::lua_pushnumber(self.L, n) }
  }

  pub fn push_integer(&mut self, i: Integer) {
    unsafe { lua::lua_pushinteger(self.L, i) }
  }

  pub fn push_bool(&mut self, b: bool) {
    unsafe { lua::lua_pushboolean(self.L, b as c_int) }
  }

  pub fn push_value(&mut self, idx: Index) {
    unsafe { lua::lua_pushvalue(self.L, idx as c_int) }
  }

  pub fn push_lightuserdata<T>(&mut self, p: *const T) {
    unsafe { lua::lua_pushlightuserdata(self.L, p as *mut c_void) }
  }

  pub fn push_cclosure(&mut self, f: lua::lua_CFunction, n: c_int) -> Result<()> {
    safe_call_closure(self, 0, 1, |state| {
      unsafe { lua::lua_pushcclosure(state.L, f, n) };
      Ok(())
    })
  }

  pub fn push_cfunction(&mut self, f: lua::lua_CFunction) {
    unsafe { lua::lua_pushcclosure(self.L, f, 0) };
  }

  pub fn get_global(&mut self, name: &str) -> Result<Type> {
    let c_str = CString::new(name)?;
    unsafe { lua::lua_getglobal(self.L, c_str.as_ptr()) };
    self.type_of(-1)
  }

  pub fn next(&self, idx: Index) -> bool {
    let result = unsafe { lua::lua_next(self.L, idx) };
    result != 0
  }

  pub fn type_of(&self, index: Index) -> Result<Type> {
    let result = unsafe { lua::lua_type(self.L, index) };
    match Type::from_c_int(result) {
      Some(t) => Ok(t),
      None => Err(anyhow!("unknown lua type: {}", result)),
    }
  }

  pub fn typename_of(&self, tp: Type) -> &'static str {
    unsafe {
      let ptr = lua::lua_typename(self.L, tp as c_int);
      let slice = CStr::from_ptr(ptr).to_bytes();
      str::from_utf8_unchecked(slice)
    }
  }

  pub fn to_type<T: FromLua>(&mut self, index: Index) -> Result<T> {
    FromLua::from_lua(self, index)
  }

  pub fn get_table(&mut self, index: Index) -> c_int {
    unsafe { lua::lua_gettable(self.L, index) }
  }

  pub fn set_table(&mut self, index: Index) -> Result<()> {
    self.push_value(index);
    self.insert(-3);
    safe_call_closure(self, 3, 0, |state| {
      unsafe { lua::lua_settable(state.L, -3) };
      Ok(())
    })
  }

  pub fn raw_geti(&mut self, index: Index, n: Integer) {
    unsafe { lua::lua_rawgeti(self.L, index, n) };
  }

  pub fn raw_seti(&mut self, index: Index, n: Integer) -> Result<()> {
    self.push_value(index);
    self.insert(-2);
    safe_call_closure(self, 2, 0, |state| {
      unsafe { lua::lua_rawseti(state.L, -2, n) };
      Ok(())
    })
  }

  pub fn raw_len(&mut self, index: Index) -> usize {
    unsafe { lua::lua_rawlen(self.L, index) as usize }
  }

  pub fn reference(&mut self, t: Index) -> c_int {
    unsafe { lua::luaL_ref(self.L, t) }
  }

  pub fn unreference(&mut self, t: Index, reference: c_int) {
    unsafe { lua::luaL_unref(self.L, t, reference) }
  }

  pub fn traceback(&mut self, L1: *mut lua::lua_State, msg: &str, level: c_int) {
    unsafe { lua::luaL_traceback(self.L, L1, msg.as_ptr() as *const c_char, level) }
  }

  pub fn push_traceback_func(&mut self) {
    self.push_cfunction(l_traceback);
  }

  #[cfg(feature = "LUA51")]
  pub fn register(&mut self, libname: &[u8], l: &[lua::luaL_Reg]) {
    unsafe {
      lua::luaL_register(self.L, libname.as_ptr() as *const c_char, l.as_ptr());
      lua::lua_pop(self.L, 1);
    }
  }

  #[cfg(feature = "LUA53")]
  pub fn register(&mut self, libname: &[u8], l: &[lua::luaL_Reg]) {
    G_L.set(&l.as_ptr(), || unsafe {
      lua::luaL_requiref(self.L, libname.as_ptr() as *const c_char, l_openlib, 1);
      lua::lua_pop(self.L, 1);
    });
  }

  pub fn newmetatable(&mut self, name: &[u8]) {
    unsafe { lua::luaL_newmetatable(self.L, name.as_ptr() as *const c_char) }
  }

  pub fn getmetatable(&mut self, name: &[u8]) {
    unsafe { lua::luaL_getmetatable(self.L, name.as_ptr() as *const c_char) }
  }

  pub fn setmetatable(&mut self, idx: c_int) {
    unsafe { lua::lua_setmetatable(self.L, idx) };
  }

  pub fn raise_error(&mut self) {
    unsafe { lua::lua_error(self.L) };
  }

  pub fn absindex(&self, index: Index) -> Index {
    unsafe { lua::lua_absindex(self.L, index) }
  }

  pub fn insert(&self, index: Index) {
    unsafe { lua::lua_insert(self.L, index) }
  }

  pub fn checkstack(&self, sz: c_int) -> Result<()> {
    if unsafe { lua::lua_checkstack(self.L, sz) == 0 } {
      Err(anyhow!("out of Lua stack"))
    } else {
      Ok(())
    }
  }

  pub fn new_userdata<T>(&mut self) -> Result<*mut T> {
    safe_call_closure(self, 0, 1, |state| {
      Ok(unsafe { lua::lua_newuserdata(state.L, std::mem::size_of::<T>()) as *mut T })
    })
  }

  pub fn regist_global_error_str(&mut self) {
    unsafe {
      let L = self.L;

      let s = lua::lua_newuserdata(L, std::mem::size_of::<String>());
      std::ptr::write(s as *mut String, String::default());

      lua::luaL_newmetatable(L, cstr!("mt_rstring"));
      lua::lua_pushstring(L, cstr!("__gc"));
      lua::lua_pushcclosure(L, drop_string, 0);
      lua::lua_settable(L, -3);

      lua::lua_setmetatable(L, -2);

      lua::lua_setglobal(L, cstr!("REGISTER_ERROR"));
    }
  }

  pub fn set_global_error(&mut self, err: String) {
    unsafe {
      lua::lua_getglobal(self.L, cstr!("REGISTER_ERROR"));
      let s = lua::luaL_checkudata(self.L, -1, cstr!("mt_rstring")) as *mut String;
      *s = err;
      lua::lua_pop(self.L, 1);
    }
  }

  pub fn raise_global_error(&mut self) {
    unsafe {
      lua::lua_getglobal(self.L, cstr!("REGISTER_ERROR"));
      let s = lua::luaL_checkudata(self.L, -1, cstr!("mt_rstring")) as *mut String;
      lua::lua_pushlstring(self.L, (*s).as_ptr() as *const c_char, (*s).len());
      lua::lua_replace(self.L, -2);
      lua::lua_error(self.L);
    }
  }
}

extern "C" fn drop_string(L: *mut lua::lua_State) -> c_int {
  let state = State::from(L);
  let s = state.to_userdata::<String>(1);
  unsafe { std::ptr::drop_in_place(s) };
  debug!("drop global error string");
  0
}

impl Drop for State {
  fn drop(&mut self) {
    if self.owned {
      unsafe {
        lua::lua_close(self.L);
      }
    }
  }
}
