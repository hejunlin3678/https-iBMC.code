use anyhow::{anyhow, Result};
use libc::{c_char, c_int};
use libloading::os::unix::{Library, Symbol};
use once_cell::sync::OnceCell;
static DBG_LOG: OnceCell<Symbol<extern "C" fn(level: c_int, fmt: *const c_char, ...)>> = OnceCell::new();
static DBG_LOG_LEVEL: OnceCell<Symbol<extern "C" fn() -> c_int>> = OnceCell::new();

const DFL_OK: i32 = 0;
const DLOG_ERROR: i32 = 0;
const DLOG_INFO: i32 = 1;
const DLOG_DEBUG: i32 = 2;
const DLOG_MASS: i32 = 3;
const DLOG_BUTT: i32 = 4;

fn get_dbg_log_outlvl() -> i32 {
  if let Some(f) = DBG_LOG_LEVEL.get() {
    f() as i32
  } else {
    1
  }
}

pub(crate) fn init_main_debug() -> Result<Library> {
  let h_main_dbg = unsafe { Library::new("libmaint_debug.so")? };
  let ret = unsafe {
    let f = h_main_dbg.get::<extern "C" fn() -> i32>(b"init_md_so\0")?;
    f()
  };
  if ret != DFL_OK {
    return Err(anyhow!("init_md_so failed.errcode: {}", ret));
  }
  let _ = DBG_LOG.set(unsafe { h_main_dbg.get::<extern "C" fn(level: c_int, fmt: *const c_char, ...)>(b"dbg_log\0")? });
  let _ = DBG_LOG_LEVEL.set(unsafe { h_main_dbg.get::<extern "C" fn() -> c_int>(b"get_dbg_log_outlvl\0")? });

  update_dbg_log_level();
  Ok(h_main_dbg)
}

fn update_dbg_log_level() -> log::Level {
  let level = dbg_log_level();
  log::set_max_level(level.to_level_filter());
  level
}

pub fn dbg_log_level() -> log::Level {
  match get_dbg_log_outlvl() {
    DLOG_ERROR => log::Level::Warn,
    DLOG_INFO => log::Level::Info,
    _ => log::Level::Debug,
  }
}

pub fn dbg_log(level: log::Level, msg: String) {
  if let Some(f) = DBG_LOG.get() {
    let lv = match level {
      log::Level::Error | log::Level::Warn => DLOG_ERROR,
      log::Level::Info => DLOG_INFO,
      _ => DLOG_DEBUG,
    };
    let c_msg = unsafe { std::ffi::CString::from_vec_unchecked(msg.into()) };
    f(lv as c_int, c_msg.as_bytes_with_nul().as_ptr() as *const c_char);
  }
}

// pme 框架改变日志级别后没有办法知道,只能定时刷新
pub fn auto_update_log_level() {
  tokio::spawn(async move {
    loop {
      let level = dbg_log_level().to_level_filter();
      if log::max_level() != level {
        log::set_max_level(level);
      }
      tokio::time::sleep(std::time::Duration::from_millis(300)).await;
    }
  });
}
