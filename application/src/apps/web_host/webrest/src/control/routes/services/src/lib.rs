#[macro_use]
extern crate core;

use core::{body_json, header_host, multipart::body_multipart};
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::{http::Response, Filter};
use webrest_handles as h;
use webrest_handles::context;

const SIZE_1K: u64 = 1024;
const SIZE_1M: u64 = 1024 * SIZE_1K;
const RO: Privilege = Privilege::READONLY;
const SM: Privilege = Privilege::SECURITYMGNT;
const UM: Privilege = Privilege::USERMGNT;
const VM: Privilege = Privilege::VMMMGNT;
const KM: Privilege = Privilege::KVMMGNT;

pub fn routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    GET => {
      ["PortConfig"] => (context(RO)).and_then(h::GetPortConfig);
      ["WEBService"] => (context(RO)).and_then(h::GetWEBService);
      ["KVM"] => (context(RO)).and_then(h::GetKVM);
      ["VMM"] => (context(RO)).and_then(h::GetVMM);
      ["VNC"] => (context(RO)).and_then(h::GetVNC);
      ["SNMP"] => (context(RO)).and_then(h::GetSNMP)
    };
    PATCH => {
      ["PortConfig"] => (body_json(SIZE_1K), context(RO)).and_then(h::SetPortConfig);
      ["WEBService"] => (body_json(SIZE_1K), context(RO)).and_then(h::SetWEBService);
      ["KVM"] => (context(RO), body_json(SIZE_1K)).and_then(h::SetKVM);
      ["VMM"] => (context(RO), body_json(SIZE_1K)).and_then(h::SetVMM);
      ["VNC"] => (context(RO), body_json(SIZE_1K)).and_then(h::SetVNC);
      ["VNC" / "Password"] => (context(KM), body_json(SIZE_1K)).and_then(h::SetVNCPassword);
      ["SNMP"] => (body_json(SIZE_1K), context(UM)).and_then(h::SetSNMP)
    };
    POST => {
      ["KVM" / "GenerateStartupFile"] => (context(KM), header_host(), body_json(SIZE_1K)).and_then(h::GenerateStartupFile);
      ["WEBService" / "ExportCSR"] => (body_json(SIZE_1K), context(SM)).and_then(h::ExportCSR);
      ["WEBService" / "ImportCertificate"] => (body_multipart(context(SM), SIZE_1M, &["p12","crt","cer","pem","pfx"], h::raw::UploadFile)).and_then(h::ImportCertificate);
      ["VMM" / "Control"] => (context(VM), body_json(SIZE_1K)).and_then(h::VMMControl)
    }
  )
  .boxed()
}
