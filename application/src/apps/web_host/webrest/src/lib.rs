#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate core;
extern crate anyhow;
extern crate lua;
#[macro_use]
extern crate log;
extern crate webrest_handles;

mod control;
pub use control::route::routes;

fn open_webrest(state: &mut lua::State) -> anyhow::Result<()> {
  control::webtask::open_webtask(state)
}

pub fn init(path: &std::path::Path) -> anyhow::Result<()> {
  webrest_handles::init(path, Some(open_webrest))?;
  Ok(())
}
