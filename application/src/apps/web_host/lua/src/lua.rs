use libc::{c_char, c_double, c_int, c_void, intptr_t, ptrdiff_t, size_t};
use std::ptr;

pub type lua_State = c_void;
pub type lua_Number = c_double;
pub type lua_Integer = ptrdiff_t;

pub type lua_CFunction = unsafe extern "C" fn(_: *mut lua_State) -> c_int;

pub type lua_KContext = intptr_t;
pub type lua_KFunction = unsafe extern "C" fn(_: *mut lua_State, status: c_int, ctx: lua_KContext) -> c_int;

pub type lua_Alloc =
  unsafe extern "C" fn(ud: *mut c_void, ptr: *const c_void, osize: size_t, nsize: size_t) -> *mut c_void;

#[repr(C)]
pub struct luaL_Reg {
  pub name: *const c_char,
  pub func: Option<lua_CFunction>,
}

impl luaL_Reg {
  pub fn new(name: &[u8], func: Option<lua_CFunction>) -> luaL_Reg {
    luaL_Reg {
      name: name.as_ptr() as *const c_char,
      func: func,
    }
  }

  pub fn null() -> luaL_Reg {
    luaL_Reg {
      name: std::ptr::null(),
      func: None,
    }
  }
}

// basic types
pub const LUA_TNONE: c_int = -1;

pub const LUA_TNIL: c_int = 0;
pub const LUA_TBOOLEAN: c_int = 1;
pub const LUA_TLIGHTUSERDATA: c_int = 2;
pub const LUA_TNUMBER: c_int = 3;
pub const LUA_TSTRING: c_int = 4;
pub const LUA_TTABLE: c_int = 5;
pub const LUA_TFUNCTION: c_int = 6;
pub const LUA_TUSERDATA: c_int = 7;
pub const LUA_TTHREAD: c_int = 8;

// thread status
pub const LUA_OK: c_int = 0;
pub const LUA_YIELD: c_int = 1;
pub const LUA_ERRRUN: c_int = 2;
pub const LUA_ERRSYNTAX: c_int = 3;
pub const LUA_ERRMEM: c_int = 4;
pub const LUA_ERRGCMM: c_int = 5;
pub const LUA_ERRERR: c_int = 6;
pub const LUA_ERRFILE: c_int = LUA_ERRERR + 1;

pub const LUA_MULTRET: c_int = -1;

pub const LUA_REGISTRYINDEX: c_int = -10000;
pub const LUA_ENVIRONINDEX: c_int = -10001;
pub const LUA_GLOBALSINDEX: c_int = -10002;

#[cfg(feature = "LUA53")]
pub const LUA_RIDX_MAINTHREAD: c_int = 1;

#[cfg(feature = "LUA53")]
pub const LUA_RIDX_GLOBALS: c_int = 2;

#[inline(always)]
pub fn lua_upvalueindex(i: c_int) -> c_int {
  LUA_GLOBALSINDEX - i
}

extern "C" {
  pub fn luaL_newstate() -> *mut lua_State;
  pub fn lua_close(L: *mut lua_State);
  pub fn luaL_openlibs(L: *mut lua_State);
  pub fn luaopen_base(L: *mut lua_State) -> c_int;
  pub fn luaopen_package(L: *mut lua_State) -> c_int;
  pub fn luaopen_table(L: *mut lua_State) -> c_int;
  pub fn luaopen_io(L: *mut lua_State) -> c_int;
  pub fn luaopen_os(L: *mut lua_State) -> c_int;
  pub fn luaopen_string(L: *mut lua_State) -> c_int;
  pub fn luaopen_math(L: *mut lua_State) -> c_int;
  #[cfg(feature = "LUA53")]
  pub fn luaL_requiref(state: *mut lua_State, modname: *const c_char, openf: lua_CFunction, glb: c_int);
  #[cfg(feature = "LUA51")]
  pub fn luaopen_bit(L: *mut lua_State) -> c_int;
  #[cfg(feature = "LUA51")]
  pub fn luaopen_jit(L: *mut lua_State) -> c_int;

  pub fn luaL_loadfilex(L: *mut lua_State, filename: *const c_char, mode: *const c_char) -> c_int;
  pub fn luaL_loadstring(L: *mut lua_State, s: *const c_char) -> c_int;
  #[cfg(feature = "LUA51")]
  pub fn luaL_register(L: *mut lua_State, libname: *const c_char, l: *const luaL_Reg);
  #[cfg(feature = "LUA53")]
  pub fn luaL_setfuncs(L: *mut lua_State, l: *const luaL_Reg, nup: c_int);
  pub fn luaL_newmetatable(L: *mut lua_State, tname: *const c_char);
  pub fn luaL_ref(L: *mut lua_State, t: c_int) -> c_int;
  pub fn luaL_unref(L: *mut lua_State, t: c_int, r: c_int);
  pub fn luaL_traceback(L: *mut lua_State, L1: *mut lua_State, msg: *const c_char, level: c_int);

  pub fn luaL_checkinteger(L: *mut lua_State, numArg: c_int) -> lua_Integer;
  pub fn luaL_checklstring(L: *mut lua_State, numArg: c_int, l: *mut size_t) -> *const c_char;
  pub fn luaL_checkstring(L: *mut lua_State, numArg: c_int) -> *const c_char;
  pub fn luaL_checktype(L: *mut lua_State, numArg: c_int, t: c_int);
  pub fn luaL_checkudata(L: *mut lua_State, ud: c_int, tname: *const c_char) -> *mut c_void;
}

// basic stack manipulation
extern "C" {
  #[cfg(feature = "LUA53")]
  pub fn lua_absindex(L: *const lua_State, i: c_int) -> c_int;
  pub fn lua_gettop(L: *const lua_State) -> c_int;
  pub fn lua_settop(L: *mut lua_State, idx: c_int);
  pub fn lua_pushvalue(L: *mut lua_State, idx: c_int);
  #[cfg(feature = "LUA51")]
  pub fn lua_remove(L: *mut lua_State, idx: c_int);
  #[cfg(feature = "LUA51")]
  pub fn lua_insert(L: *mut lua_State, idx: c_int);
  #[cfg(feature = "LUA51")]
  pub fn lua_replace(L: *mut lua_State, idx: c_int);
  #[cfg(feature = "LUA53")]
  pub fn lua_rotate(L: *mut lua_State, idx: c_int, n: c_int);
  #[cfg(feature = "LUA53")]
  pub fn lua_copy(L: *mut lua_State, fromidx: c_int, toidx: c_int);
  pub fn lua_checkstack(L: *mut lua_State, sz: c_int) -> c_int;
  pub fn lua_xmove(from: *mut lua_State, to: *mut lua_State, n: c_int);
}

// access functions (stack -> C)
extern "C" {
  pub fn lua_isnumber(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_isstring(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_iscfunction(L: *mut lua_State, idx: c_int) -> c_int;
  #[cfg(feature = "LUA53")]
  pub fn lua_isinteger(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_isuserdata(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_type(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_typename(L: *mut lua_State, tp: c_int) -> *const c_char;

  #[cfg(feature = "LUA53")]
  pub fn lua_tonumberx(L: *mut lua_State, idx: c_int, isnum: *mut c_int) -> lua_Number;
  #[cfg(feature = "LUA53")]
  pub fn lua_tointegerx(L: *mut lua_State, idx: c_int, isnum: *mut c_int) -> lua_Integer;
  #[cfg(feature = "LUA51")]
  pub fn lua_tonumber(L: *mut lua_State, idx: c_int) -> lua_Number;
  #[cfg(feature = "LUA51")]
  pub fn lua_tointeger(L: *mut lua_State, idx: c_int) -> lua_Integer;
  pub fn lua_toboolean(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_tolstring(L: *mut lua_State, idx: c_int, len: *mut size_t) -> *const c_char;
  #[cfg(feature = "LUA53")]
  pub fn lua_rawlen(L: *mut lua_State, idx: c_int) -> size_t;
  #[cfg(feature = "LUA51")]
  pub fn lua_objlen(L: *mut lua_State, idx: c_int) -> size_t;
  pub fn lua_tocfunction(L: *mut lua_State, idx: c_int) -> lua_CFunction;
  pub fn lua_touserdata(L: *mut lua_State, idx: c_int) -> *mut c_void;
  pub fn lua_tothread(L: *mut lua_State, idx: c_int) -> *mut lua_State;
  pub fn lua_topointer(L: *mut lua_State, idx: c_int) -> *const c_void;
}

// push functions (C -> stack)
extern "C" {
  pub fn lua_pushnil(L: *mut lua_State);
  pub fn lua_pushnumber(L: *mut lua_State, n: lua_Number);
  pub fn lua_pushinteger(L: *mut lua_State, n: lua_Integer);
  pub fn lua_pushlstring(L: *mut lua_State, s: *const c_char, l: size_t) -> *const c_char;
  pub fn lua_pushstring(L: *mut lua_State, s: *const c_char) -> *const c_char;
  pub fn lua_pushcclosure(L: *mut lua_State, f: lua_CFunction, n: c_int);
  pub fn lua_pushboolean(L: *mut lua_State, b: c_int);
  pub fn lua_pushlightuserdata(L: *mut lua_State, p: *const c_void);
  pub fn lua_pushthread(L: *mut lua_State) -> c_int;
}

// get functions (Lua -> stack)
extern "C" {
  #[cfg(feature = "LUA53")]
  pub fn lua_getglobal(L: *mut lua_State, name: *const c_char) -> c_int;
  pub fn lua_gettable(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_getfield(L: *mut lua_State, idx: c_int, k: *const c_char) -> c_int;
  #[cfg(feature = "LUA53")]
  pub fn lua_geti(L: *mut lua_State, idx: c_int, n: lua_Integer) -> c_int;
  pub fn lua_rawget(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_rawgeti(L: *mut lua_State, idx: c_int, n: lua_Integer) -> c_int;
  #[cfg(feature = "LUA53")]
  pub fn lua_rawgetp(L: *mut lua_State, idx: c_int, p: *const c_void) -> c_int;

  pub fn lua_createtable(L: *mut lua_State, narr: c_int, nrec: c_int);
  pub fn lua_newuserdata(L: *mut lua_State, sz: size_t) -> *mut c_void;
  pub fn lua_getmetatable(L: *mut lua_State, idx: c_int) -> c_int;
  #[cfg(feature = "LUA53")]
  pub fn lua_getuservalue(L: *mut lua_State, idx: c_int) -> c_int;
  #[cfg(feature = "LUA51")]
  pub fn lua_getfenv(L: *mut lua_State, idx: c_int);
}

// set functions (stack -> Lua)
extern "C" {
  #[cfg(feature = "LUA53")]
  pub fn lua_setglobal(L: *mut lua_State, var: *const c_char);
  pub fn lua_settable(L: *mut lua_State, idx: c_int);
  pub fn lua_setfield(L: *mut lua_State, idx: c_int, k: *const c_char);
  #[cfg(feature = "LUA53")]
  pub fn lua_seti(L: *mut lua_State, idx: c_int, n: lua_Integer);
  pub fn lua_rawset(L: *mut lua_State, idx: c_int);
  pub fn lua_rawseti(L: *mut lua_State, idx: c_int, n: lua_Integer);
  #[cfg(feature = "LUA53")]
  pub fn lua_rawsetp(L: *mut lua_State, idx: c_int, p: *const c_void);
  pub fn lua_setmetatable(L: *mut lua_State, objindex: c_int) -> c_int;
  #[cfg(feature = "LUA53")]
  pub fn lua_setuservalue(L: *mut lua_State, idx: c_int);
  #[cfg(feature = "LUA51")]
  pub fn lua_setfenv(L: *mut lua_State, idx: c_int) -> c_int;
}

// `load' and `call' functions (load and run Lua code)
extern "C" {
  #[cfg(feature = "LUA53")]
  pub fn lua_callk(L: *mut lua_State, nargs: c_int, nresults: c_int, ctx: lua_KContext, k: Option<lua_KFunction>);
  #[cfg(feature = "LUA53")]
  pub fn lua_pcallk(
    L: *mut lua_State,
    nargs: c_int,
    nresults: c_int,
    errfunc: c_int,
    ctx: lua_KContext,
    k: Option<lua_KFunction>,
  ) -> c_int;
  #[cfg(feature = "LUA51")]
  pub fn lua_call(L: *mut lua_State, nargs: c_int, nresults: c_int);
  #[cfg(feature = "LUA51")]
  pub fn lua_pcall(L: *mut lua_State, nargs: c_int, nresults: c_int, errfunc: c_int) -> c_int;
}

// miscellaneous functions
extern "C" {
  pub fn lua_error(L: *mut lua_State) -> c_int;
  pub fn lua_next(L: *mut lua_State, idx: c_int) -> c_int;
  pub fn lua_concat(L: *mut lua_State, n: c_int);
  pub fn lua_getallocf(L: *mut lua_State, ud: *mut *const c_void) -> lua_Alloc;
  pub fn lua_setallocf(L: *mut lua_State, f: lua_Alloc, ud: *const c_void);
}

#[inline(always)]
pub unsafe fn lua_pop(L: *mut lua_State, n: c_int) {
  lua_settop(L, -n - 1)
}

#[inline(always)]
pub unsafe fn lua_newtable(L: *mut lua_State) {
  lua_createtable(L, 0, 0)
}

#[inline(always)]
pub unsafe fn lua_isfunction(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TFUNCTION) as c_int
}

#[inline(always)]
pub unsafe fn lua_istable(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TTABLE) as c_int
}

#[inline(always)]
pub unsafe fn lua_islightuserdata(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TLIGHTUSERDATA) as c_int
}

#[inline(always)]
pub unsafe fn lua_isnil(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TNIL) as c_int
}

#[inline(always)]
pub unsafe fn lua_isboolean(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TBOOLEAN) as c_int
}

#[inline(always)]
pub unsafe fn lua_isthread(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TTHREAD) as c_int
}

#[inline(always)]
pub unsafe fn lua_isnone(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) == LUA_TNONE) as c_int
}

#[inline(always)]
pub unsafe fn lua_isnoneornil(L: *mut lua_State, n: c_int) -> c_int {
  (lua_type(L, n) <= 0) as c_int
}

#[inline(always)]
pub unsafe fn luaL_loadfile(L: *mut lua_State, f: *const c_char) -> c_int {
  luaL_loadfilex(L, f, ptr::null())
}

#[inline(always)]
#[cfg(feature = "LUA51")]
pub unsafe fn lua_setglobal(L: *mut lua_State, var: *const c_char) {
  lua_setfield(L, LUA_GLOBALSINDEX, var)
}

#[inline(always)]
#[cfg(feature = "LUA51")]
pub unsafe fn lua_getglobal(L: *mut lua_State, name: *const c_char) -> c_int {
  lua_getfield(L, LUA_GLOBALSINDEX, name)
}

#[inline(always)]
#[cfg(feature = "LUA51")]
pub unsafe fn lua_absindex(L: *const lua_State, i: c_int) -> c_int {
  if i < 0 && i > LUA_REGISTRYINDEX {
    i + lua_gettop(L) + 1
  } else {
    i
  }
}

#[inline(always)]
#[cfg(feature = "LUA51")]
pub unsafe fn lua_rawlen(L: *mut lua_State, idx: c_int) -> size_t {
  lua_objlen(L, idx)
}

#[inline(always)]
#[cfg(feature = "LUA53")]
pub unsafe fn lua_tonumber(L: *mut lua_State, idx: c_int) -> lua_Number {
  lua_tonumberx(L, idx, std::ptr::null_mut())
}

#[inline(always)]
#[cfg(feature = "LUA53")]
pub unsafe fn lua_tointeger(L: *mut lua_State, idx: c_int) -> lua_Integer {
  lua_tointegerx(L, idx, std::ptr::null_mut())
}

#[inline(always)]
#[cfg(feature = "LUA51")]
pub unsafe fn lua_isinteger(L: *mut lua_State, idx: c_int) -> c_int {
  lua_isnumber(L, idx)
}

#[inline(always)]
#[cfg(feature = "LUA53")]
pub unsafe fn lua_call(L: *mut lua_State, nargs: c_int, nresults: c_int) {
  lua_callk(L, nargs, nresults, 0, None)
}

#[inline(always)]
#[cfg(feature = "LUA53")]
pub unsafe fn lua_pcall(L: *mut lua_State, nargs: c_int, nresults: c_int, errfunc: c_int) -> c_int {
  lua_pcallk(L, nargs, nresults, errfunc, 0, None)
}

#[inline(always)]
#[cfg(feature = "LUA53")]
pub unsafe fn lua_insert(L: *mut lua_State, idx: c_int) {
  lua_rotate(L, idx, 1)
}

#[inline(always)]
pub unsafe fn luaL_getmetatable(L: *mut lua_State, tname: *const c_char) {
  lua_getfield(L, LUA_REGISTRYINDEX, tname);
}
