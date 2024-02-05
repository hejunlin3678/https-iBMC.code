#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(dead_code)]

#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate scoped_tls;
#[macro_use]
extern crate log;
extern crate anyhow;
#[macro_use]
extern crate lua;
extern crate chrono;
extern crate futures;
extern crate pin_project;

#[macro_use]
pub mod handles;
pub mod handle;
pub mod macros;
pub mod multipart;
pub mod tasks;
pub mod validator;

mod anit_dos;
mod dflib;
mod formdata;
mod main_debug_log;
mod utils;
mod wrap_utils;
pub use anit_dos::{is_locked as anit_dos_locked, warp as anit_dos};
pub use dflib::*;
pub use handle::{Decorator, HandleFuture};
pub use main_debug_log::{auto_update_log_level, dbg_log, dbg_log_level};
pub use utils::*;
pub use validator::Validator;
pub use wrap_utils::*;
