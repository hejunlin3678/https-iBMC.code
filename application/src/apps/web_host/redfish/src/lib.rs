#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

extern crate anyhow;
extern crate base;
extern crate bytes;
#[macro_use]
extern crate core;
extern crate handles;
extern crate lua;
extern crate serde_json;
extern crate warp;
#[macro_use]
extern crate log;
extern crate regex;
#[macro_use]
extern crate lazy_static;

extern crate base as redfish_base;
extern crate handles as redfish_handles;

pub mod proxy;
pub mod sms_proxy;
pub use base::config_init;

fn open_redfish(state: &mut lua::State) -> anyhow::Result<()> {
  base::open_redfish_registry(state)?;
  Ok(())
}

pub fn init(lua_path: &std::path::Path) -> anyhow::Result<()> {
  base::load_registris()?;
  handles::init(lua_path, Some(open_redfish))?;
  Ok(())
}
