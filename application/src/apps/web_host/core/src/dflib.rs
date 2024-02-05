use super::handle::open_handle;
use super::handle::HandleFuture;
use super::main_debug_log::init_main_debug;
use super::tasks;
use super::utils::{init_portal, lock_state_call, open_portal, Params};
use anyhow::{anyhow, Result};
use libc::c_int;
use libloading::os::unix::Library;
use lua::{FromLua, State, Type, LUA_REGISTRYINDEX};
use std::collections::HashMap;
use std::path::Path;
use std::sync::{Arc, Mutex};
use std::time::Duration;

const DFL_OK: i32 = 0;
const RTLD_LAZY: i32 = 0x00001;
const RTLD_GLOBAL: i32 = 0x00100;
pub fn dfl_init(module_name: &[u8]) -> Result<(Library, Library)> {
  let hdflib = unsafe { Library::open(Some("libdflib.so"), RTLD_LAZY | RTLD_GLOBAL)? };
  let ret = unsafe {
    let f = hdflib.get::<extern "C" fn(slot_string: *const u8) -> c_int>(b"static_register_module\0")?;
    f(module_name.as_ptr()) as i32
  };
  if ret != DFL_OK {
    return Err(anyhow!(
      "register module {} failed.errcode:{}",
      std::str::from_utf8(&module_name[..module_name.len() - 1])?,
      ret
    ));
  }

  let h_main_dbg = init_main_debug()?;

  let ret = unsafe {
    let f = hdflib.get::<extern "C" fn() -> c_int>(b"static_complete_module_register\0")?;
    f() as i32
  };
  if ret != DFL_OK {
    return Err(anyhow!("get object/class/route info failed.errcode:{}", ret));
  }

  init_portal(&hdflib)?;

  let static_start_rpc_service_fn = unsafe { hdflib.get::<extern "C" fn()>(b"static_start_rpc_service\0")? };
  std::thread::spawn(move || {
    static_start_rpc_service_fn();
  });

  Ok((hdflib, h_main_dbg))
}

pub fn dfl_setprocess_uiggid(gid: u32, uid :u32) -> Result<()> {
  let hdflib = unsafe { Library::open(Some("libdflib.so"), RTLD_LAZY | RTLD_GLOBAL)? };
  let dfl_rpc_selftest = unsafe { hdflib.get::<extern "C" fn() -> c_int>(b"dfl_rpc_selftest\0")? };
  let dfl_update_proxy_flag_fn = unsafe { hdflib.get::<extern "C" fn()>(b"dfl_update_proxy_flag\0")? };
  std::thread::spawn(move || {
    std::thread::sleep(Duration::from_secs(10));
    while dfl_rpc_selftest() != DFL_OK {
      std::thread::sleep(Duration::from_millis(100));
    }
    match unsafe { libc::setgid(gid) } {
      0 => {}
      ret => error!("setgid failed: {}", ret),
    }
    match unsafe { libc::setuid(uid) } {
      0 => {}
      ret => error!("setuid failed: {}", ret),
    }
    dfl_update_proxy_flag_fn();
  });
  Ok(())
}

pub struct Dflib {
  pub state: Arc<Mutex<State>>,
  maps: HashMap<&'static str, i32>,
  module: &'static str,
}

impl Dflib {
  pub fn new(module: &'static str) -> Dflib {
    Dflib {
      state: Arc::new(Mutex::new(State::new())),
      maps: HashMap::new(),
      module: module,
    }
  }
}

impl Dflib {
  pub fn init(&mut self, path: &Path, mode_init: &Option<fn(state: &mut State) -> Result<()>>) -> Result<()> {
    lock_state_call(&self.state, |state| {
      state.open_libs()?;
      tasks::open_tasks(state);
      open_portal(state);
      open_handle(state)?;
      match mode_init {
        Some(ref f) => {
          f(state)?;
        }
        _ => {}
      };

      state.push_string(path.to_str().unwrap_or("./"))?;
      state.set_global("ROOT_PATH")?;

      let mut loader_file = std::path::PathBuf::from(path);
      loader_file.push(self.module);
      loader_file.push(format!("{}.lua", self.module));
      state.do_file(
        loader_file
          .to_str()
          .unwrap_or(&format!("./{}/{}.lua", self.module, self.module)),
      )?;
      Ok(())
    })?;
    Ok(())
  }

  pub fn load_handles(&mut self, handles: &'static [&str]) -> Result<()> {
    let mut state = self.state.lock().unwrap();

    match state.get_global(self.module)? {
      Type::Table => {}
      t => return Err(anyhow!("invalid module {}: {}", self.module, t)),
    }

    for name in handles {
      state.push_string(name)?;
      state.get_table(-2);
      let ty = state.type_of(-1)?;
      if ty != Type::Function {
        return Err(anyhow!(
          "invalid lua handle {}.{}, function expected, got {}",
          self.module,
          name,
          state.typename_of(ty)
        ));
      }
      self.maps.insert(name, state.reference(LUA_REGISTRYINDEX));
    }

    Ok(())
  }

  pub fn call<A: Params + Sized, T: FromLua>(&mut self, h: &'static str, args: A) -> HandleFuture<A, T> {
    HandleFuture::new(self.state.clone(), h, *self.maps.get(h).unwrap(), args)
  }
}
