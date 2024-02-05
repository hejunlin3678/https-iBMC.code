#[macro_use]
extern crate core;

use core::body_json;
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::{http::Response, Filter};
use webrest_handles as h;
use webrest_handles::context;

const SIZE_1K: u64 = 1024;
const RO: Privilege = Privilege::READONLY;
const BS: Privilege = Privilege::BASICSETTING;
const PM: Privilege = Privilege::POWERMGNT;

pub fn routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    GET => {
        ["Enclosure" / "Thermal"] => (context(RO)).and_then(h::GetChassisThermal);
        ["Enclosure" / "PowerSupply"] => (context(RO)).and_then(h::GetChassisPowerSupply);
        ["Enclosure" / "Power"] => (context(RO)).and_then(h::GetChassisPower);
        ["Enclosure" / "SwitchProfile"] => (context(RO)).and_then(h::GetSwitchFileList)
    };
    PATCH => {
        ["Enclosure" / "Thermal"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateChassisThermal);
        ["Enclosure" / "PowerSupply"] => (body_json(SIZE_1K), context(PM)).and_then(h::UpdatePowerSleepMode);
        ["Enclosure" / "Power"] => (body_json(SIZE_1K), context(PM)).and_then(h::UpdateChassisPower)
    };
    POST => {
        ["Enclosure" / "ParseSwitchProfile"] => (body_json(SIZE_1K), context(BS)).and_then(h::GetSwitchFile);
        ["Enclosure" / "RestoreSwitchProfile"] => (body_json(SIZE_1K), context(BS)).and_then(h::RestoreSwiProfile)
    }
  )
  .boxed()
}
