#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

extern crate anyhow;
extern crate core;
#[macro_use]
extern crate bitflags;

mod privilege;

pub const SIZE_256: u64 = 256;
pub const SIZE_1K: u64 = 1024;
pub const SIZE_2K: u64 = 2048;
pub const SIZE_4K: u64 = 4096;
pub const SIZE_120K: u64 = 120 * 1024;
pub const SIZE_1200K: u64 = 1200 * 1024;

pub use privilege::{check as privileg_check, Privilege};
