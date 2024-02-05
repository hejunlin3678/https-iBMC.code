#![allow(unused_must_use)]

use super::utils::{lock_state_call, Params};
use anyhow::{anyhow, Result};
use libc::c_int;
use lua::{lua_State, FromLua, Integer, State, LUA_REGISTRYINDEX};
use pin_project::pin_project;
use std::future::Future;
use std::marker::PhantomData;
use std::pin::Pin;
use std::sync::{Arc, Mutex};
use std::task::{Context, Poll, Waker};

const MT_HANDLE: &[u8] = b"mt_handle\0";

pub trait Decorator<T> {
  fn render(&self, reply: &mut T);
}

#[pin_project]
pub struct HandleFuture<A: Params + Sized, T: FromLua> {
  state: Arc<Mutex<State>>,
  handleName: &'static str,
  phantom: PhantomData<T>,
  status: HandleStatus,
  args: A,
}

enum HandleStatus {
  First(i32),
  Second(i32),
  Done,
}

impl<A: Params + Sized, T: FromLua> HandleFuture<A, T> {
  pub fn new(state: Arc<Mutex<State>>, handleName: &'static str, href: i32, args: A) -> Self {
    HandleFuture {
      state,
      handleName,
      phantom: PhantomData,
      status: HandleStatus::First(href),
      args,
    }
  }
}

struct HandleUnRef {
  state: State,
  href: c_int,
}

impl HandleUnRef {
  fn new(s: &State, href: c_int) -> Self {
    HandleUnRef {
      state: State::from(s.L),
      href,
    }
  }

  fn release(&mut self) {
    self.href = 0;
  }
}

impl Drop for HandleUnRef {
  fn drop(&mut self) {
    if self.href != 0 {
      self.state.unreference(LUA_REGISTRYINDEX, self.href);
    }
  }
}

fn poll_first(state: &Arc<Mutex<State>>, waker: &Waker, href: c_int, args: &dyn Params) -> Result<(Option<i32>, i32)> {
  lock_state_call(state, move |state| {
    state.raw_geti(LUA_REGISTRYINDEX, href as Integer);
    push_handle(state, waker.clone())?;
    let nparam = args.push(state)?;
    state.pcall(nparam + 1, 2, 0)?;
    if state.to_bool(-2) {
      return Ok((Some(state.reference(LUA_REGISTRYINDEX)), 0));
    }

    state.check_type(-1, lua::Type::Function)?;
    Ok((None, state.reference(LUA_REGISTRYINDEX)))
  })
}

fn poll_second(state: &Arc<Mutex<State>>, href: c_int) -> Result<Option<i32>> {
  lock_state_call(state, |state| {
    let mut unref = HandleUnRef::new(state, href);
    state.raw_geti(LUA_REGISTRYINDEX, href as Integer);
    state.pcall(0, 1, 0)?;
    if !state.is_nil(-1) {
      return Ok(Some(state.reference(LUA_REGISTRYINDEX)));
    }

    unref.release();
    Ok(None)
  })
}

fn get_result<T: FromLua>(state: &Arc<Mutex<State>>, result_ref: c_int) -> Result<T> {
  lock_state_call(&state, move |state| {
    let _unref = HandleUnRef::new(state, result_ref);
    state.raw_geti(LUA_REGISTRYINDEX, result_ref as Integer);
    Ok(state.to_type::<T>(-1)?)
  })
}

fn try_poll(
  status: &mut HandleStatus,
  state: &Arc<Mutex<State>>,
  name: &'static str,
  args: &dyn Params,
  cx: &mut Context,
) -> Poll<Result<i32>> {
  match status {
    HandleStatus::First(href) => match poll_first(state, cx.waker(), *href, args) {
      Ok((Some(result_ref), _)) => {
        *status = HandleStatus::Done;
        Poll::Ready(Ok(result_ref))
      }
      Ok((None, href)) => {
        *status = HandleStatus::Second(href);
        Poll::Pending
      }
      Err(e) => Poll::Ready(Err(anyhow!("{}: {}", name, e))),
    },
    HandleStatus::Second(href) => match poll_second(state, *href) {
      Ok(Some(result_ref)) => {
        *status = HandleStatus::Done;
        Poll::Ready(Ok(result_ref))
      }
      Ok(None) => Poll::Pending,
      Err(e) => Poll::Ready(Err(anyhow!("{}: {}", name, e))),
    },
    HandleStatus::Done => panic!("polled after complete"),
  }
}

impl<A: Params, T: FromLua> Future for HandleFuture<A, T> {
  type Output = Result<T>;

  fn poll(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Self::Output> {
    let pin = self.as_mut().project();
    match try_poll(pin.status, pin.state, pin.handleName, pin.args, cx) {
      Poll::Pending => Poll::Pending,
      Poll::Ready(Ok(result_ref)) => Poll::Ready(get_result(&pin.state, result_ref)),
      Poll::Ready(Err(e)) => Poll::Ready(Err(e)),
    }
  }
}

extern "C" fn l_gc_handle(L: *mut lua_State) -> c_int {
  let state = State::from(L);
  let waker = state.to_userdata::<Waker>(1);
  unsafe { std::ptr::drop_in_place(waker) };
  0
}

extern "C" fn l_wake_handle(L: *mut lua_State) -> c_int {
  let state = State::from(L);
  let ud = state.to_userdata::<Waker>(1);
  unsafe { (*ud).wake_by_ref() };
  0
}

fn push_handle(state: &mut State, waker: Waker) -> Result<()> {
  let ud = state.new_userdata::<Waker>()?;
  unsafe { std::ptr::write(ud, waker) };
  state.getmetatable(MT_HANDLE);
  state.setmetatable(-2);
  Ok(())
}

pub(super) fn open_handle(state: &mut State) -> Result<()> {
  let _stack = state.save_stack();

  state.newmetatable(MT_HANDLE);
  state.push_string("__gc")?;
  state.push_cfunction(l_gc_handle);
  state.set_table(-3)?;

  state.push_string("__index")?;
  state.push_value(-2);
  state.set_table(-3)?;

  state.push_string("wake")?;
  state.push_cfunction(l_wake_handle);
  state.set_table(-3)?;

  Ok(())
}
