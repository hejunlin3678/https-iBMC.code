use super::lua;
use super::{lua_State, State};
use anyhow::{anyhow, Result};
use libc::c_int;

struct CallParam<F, U> {
  func: F,
  result: Option<U>,
  nresults: c_int,
}

unsafe extern "C" fn l_call<F, U>(L: *mut lua_State) -> c_int
where
  F: Fn(&mut State) -> Result<U>,
{
  let mut state = State::from(L);
  #[cfg(all(target_arch = "aarch64", feature = "LUA51"))]
  let param = &mut *std::mem::transmute::<_, *mut CallParam<F, Result<U>>>(state.to_integer(-1).unwrap());
  #[cfg(not(all(target_arch = "aarch64", feature = "LUA51")))]
  let param = &mut *state.to_userdata::<CallParam<F, Result<U>>>(-1);

  state.pop(1);
  param.result = Some((param.func)(&mut state));
  if param.nresults == lua::LUA_MULTRET {
    state.get_top()
  } else {
    param.nresults
  }
}

pub fn safe_call_closure<F, U>(state: &mut State, nargs: c_int, nresults: c_int, func: F) -> Result<U>
where
  F: Fn(&mut State) -> Result<U>,
{
  let mut param = CallParam {
    func: func,
    result: None::<Result<U>>,
    nresults,
  };

  let first_param = state.get_top() - nargs + 1;
  state.push_cfunction(l_call::<F, U>);
  if nargs > 0 {
    unsafe {
      #[cfg(feature = "LUA53")]
      lua::lua_rotate(state.L, first_param, 1);
      #[cfg(feature = "LUA51")]
      lua::lua_insert(state.L, first_param);
    }
  }
  #[cfg(all(target_arch = "aarch64", feature = "LUA51"))]
  state.push_integer(unsafe { std::mem::transmute(&mut param) });
  #[cfg(not(all(target_arch = "aarch64", feature = "LUA51")))]
  state.push_lightuserdata(&mut param);
  match state.pcall(nargs + 1, nresults, 0) {
    Ok(_) => param.result.unwrap(),
    Err(e) => Err(anyhow!("call cclosure failed: {}", e)),
  }
}

pub fn wrap_lua_func<F>(state: &mut State, func: F) -> c_int
where
  F: Fn(&mut State) -> Result<c_int>,
{
  match func(state) {
    Ok(v) => v,
    Err(e) => {
      error!("wrap_lua_func: {}", e);
      state.set_global_error(e.to_string());
      drop(e);
      drop(func);
      state.raise_global_error();
      0
    }
  }
}

#[inline(always)]
#[cfg(all(target_arch = "aarch64", feature = "LUA51"))]
pub fn json_lightudata_mask<T>(ludata: *const T) -> *const T {
  let u: super::Integer = ludata as _;
  (u & ((1 << 47) - 1)) as *const T
}

#[inline(always)]
#[cfg(not(all(target_arch = "aarch64", feature = "LUA51")))]
pub fn json_lightudata_mask<T>(ludata: *const T) -> *const T {
  ludata
}
