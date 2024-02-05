#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(dead_code)]
#![allow(non_upper_case_globals)]
#![type_length_limit = "4000000"]

extern crate anyhow;
#[macro_use]
extern crate log;
#[macro_use]
extern crate core;
extern crate num_cpus;
extern crate once_cell;
#[cfg(feature = "REDFISH")]
extern crate redfish;
#[cfg(feature = "WEBREST")]
extern crate webrest;

use anyhow::Result;

mod route;
use core::{dbg_log, dfl_init, dfl_setprocess_uiggid};
use log::{Metadata, Record};
use route::routes;
use tokio::runtime;

struct SimpleLogger;

impl log::Log for SimpleLogger {
  fn enabled(&self, metadata: &Metadata) -> bool {
    metadata.level() <= log::max_level()
  }

  fn log(&self, record: &Record) {
    if self.enabled(record.metadata()) {
      dbg_log(record.level(), record.args().to_string());
    }
  }

  fn flush(&self) {}
}

static LOGGER: SimpleLogger = SimpleLogger;

fn check_lua_handlds() -> Result<()> {
  let exe_path = std::env::current_exe()?;
  let cur_path = std::env::current_dir()?;
  let path = exe_path.parent().unwrap_or(cur_path.as_path());

  #[cfg(feature = "REDFISH")]
  redfish::init(path)?;

  #[cfg(feature = "WEBREST")]
  webrest::init(path)?;

  // 防止所有模块特性都未指定时的 unused 告警
  drop(path);

  Ok(())
}

#[cfg(feature = "REDFISH")]
fn init_redfish() -> Result<()> {
  info!("configuration property \"redfish.conf_path\" not found, use default path!");
  redfish::config_init("/data/opt/pme/conf".into())?;
  Ok(())
}

fn main() -> Result<()> {
  // 这里必须主动加载 libdflib.so 并维持引用。
  // 为了确定注册的 lua handle 是否全部实现，我们会先启用一个临时 lua 引擎来做检查,
  // 临时引擎销毁时会卸载 libdflib.so 库，这偶尔会使程序崩溃。
  // 让宿主程序主动加载 libdflib.so 库，保证虚拟机退出时不会卸载它。
  let _hdflib = dfl_init(b"portal\0")?;

  log::set_logger(&LOGGER)?;
  let mut port = 30080;
  match unsafe { libc::fork() } {
    -1 => error!("fork fail"),
    0 => {
      let ppid = unsafe {libc::getppid()};
      unsafe {
        libc::prctl(15 as libc::c_int, "sp_upload".to_string().as_ptr() as libc::c_ulong, 0, 0, 0)
      };

      dfl_setprocess_uiggid(103, 97)?; // 103为apps属组, 97为kvm_user
      port = 30081;

      std::thread::spawn(move || {
        loop {
          // 父进程退出后，子进程的父亲会变更为init进程，这里通过父进程pid的变更检测父进程是否已退出
          if ppid != unsafe {libc::getppid()} {
            error!("Parent process has exit");
            std::process::exit(0);
          }
          std::thread::sleep(std::time::Duration::from_millis(1000));
        }
      });
    },
    child_pid => {
      dfl_setprocess_uiggid(103, 102)?; // 103为apps属组, 102为redfish_user
      port = 30080;

      // 监控子进程是否退出,
      std::thread::spawn(move || {
        let mut status:i32 = 0;
        match unsafe {libc::waitpid(child_pid, &mut status as *mut libc::c_int, 0)} {
          pid if pid == child_pid => {
            error!("child process has exit {}", status);
            std::process::exit(0);
          },
          -1 => { // -1 代表ECHILD，即当前进程无子进程
            error!("child process has no child");
            std::process::exit(0);
          },
          res => {
            error!("child waitpid error {} {}", res, status);
          }
        }
      });
    }
  };

  let mut buider = runtime::Builder::new_current_thread();

  #[cfg(feature = "REDFISH")]
  init_redfish()?;

  let rt = buider.enable_io().enable_time().build()?;
  rt.block_on(async move {
    core::auto_update_log_level();
    check_lua_handlds()?;
    let routes = routes();
    warp::serve(routes).run(([127, 0, 0, 1], port)).await;
    Ok(())
  })
}
