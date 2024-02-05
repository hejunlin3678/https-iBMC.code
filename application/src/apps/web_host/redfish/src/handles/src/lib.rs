#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate core;
extern crate anyhow;
extern crate base as redfish_base;
extern crate lua;
extern crate serde;
extern crate sha2;

pub mod datas;
mod redfish_handles;

pub use redfish_handles::*;
