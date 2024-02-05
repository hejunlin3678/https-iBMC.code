use core::{response_error, ResponseError, User};
use warp::http::StatusCode;
use warp::{reject, Rejection};

bitflags! {
  // User role privilage.
  //
  // # Example
  //
  // ```
  // use crate::privilege::Privilege;
  //
  // const ro = Privilege::READONLY;
  // const robs = Privilege::READONLY | Privilege::BASICSETTING;
  // ```
  pub struct Privilege: u8 {
    const READONLY     = 0b00000001;
    const DIAGNOSEMGNT = 0b00000010;
    const SECURITYMGNT = 0b00000100;
    const BASICSETTING = 0b00001000;
    const USERMGNT     = 0b00010000;
    const POWERMGNT    = 0b00100000;
    const VMMMGNT      = 0b01000000;
    const KVMMGNT      = 0b10000000;
  }
}

pub fn check(user: &User, v: Privilege) -> std::result::Result<(), Rejection> {
  if v.is_empty() {
    return Ok(());
  }
  match Privilege::from_bits(user.Privilege) {
    Some(p) if p.contains(v) => Ok(()),
    _ => Err(reject::custom(ResponseError(response_error(
      StatusCode::FORBIDDEN,
      "InsufficientPrivilege",
      String::default(),
    )))),
  }
}
