#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

extern crate anyhow;
extern crate core;
extern crate serde;
extern crate sha2;
#[macro_use]
extern crate bitflags;
#[macro_use]
extern crate log;
#[macro_use]
extern crate lua;

mod config;
mod registries;
mod reply;
mod utils;

pub const SIZE_256: u64 = 256;
pub const SIZE_1K: u64 = 1024;
pub const SIZE_2K: u64 = 2048;
pub const SIZE_4K: u64 = 4096;
pub const SIZE_120K: u64 = 120 * 1024;
pub const SIZE_1200K: u64 = 1200 * 1024;

pub use config::config_init;
pub use registries::{create_message, load_registris, open_redfish_registry};
pub use reply::{reject_error_info, response_error, RedfishReply};
pub use utils::{auth, AuthInfo, getBasicAuth, Context, DownloadReply, RawString};
