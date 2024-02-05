#[macro_use]
extern crate core;

use core::{body_json, multipart::body_multipart, multipart::body_multipart_save_file_sp};
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::{http::Response, Filter};
use webrest_handles as h;
use webrest_handles::{context, empty_body};

const SIZE_1K: u64 = 1024;
const SIZE_1M: u64 = 1024 * SIZE_1K;
const SIZE_2M: u64 = 2 * SIZE_1M;
const SIZE_90M: u64 = 90 * SIZE_1M;
const SIZE_500M: u64 = 500 * SIZE_1M;
const RO: Privilege = Privilege::READONLY;
const BS: Privilege = Privilege::BASICSETTING;
const UM: Privilege = Privilege::USERMGNT;
const VM: Privilege = Privilege::VMMMGNT;

pub fn routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    GET => {
      ["UpdateService" / "SPUpdateFileList"] => (context(BS)).and_then(h::GetSPUpdateFileList);
      ["NetworkSettings"] => (context(RO)).and_then(h::GetNetworkSettings);
      ["UpdateService"] => (context(BS)).and_then(h::GetUpdateService);
      ["NTP"] => (context(RO)).and_then(h::GetNTP);
      ["UpdateService" / "UpdateProgress"] => (context(BS)).and_then(h::GetUpdateProgress);
      ["Configuration" / "ImportExportProgress"] => (context(UM)).and_then(h::GetImportExportConfigProgress);
      ["Language"] => (context(BS)).and_then(h::GetLanguage);
      ["LicenseService"] => (context(RO)).and_then(h::GetLicenseService);
      ["iBMA"] => (context(RO)).and_then(h::GetiBMA);
      ["SP"] => (context(RO)).and_then(h::GetSP);
      ["USBMgmtService"] => (context(RO)).and_then(h::GetUSBMgmtService);
      ["Failover"] => (context(RO)).and_then(h::GetFailover)
    };
    PATCH => {
      ["NetworkSettings"] => (body_json(SIZE_1K), context(BS)).and_then(h::SetNetworkSettings);
      ["NTP"] => (body_json(SIZE_1K), context(BS)).and_then(h::SetNTP);
      ["SP"] => (context(BS), body_json(SIZE_1K)).and_then(h::SetSP);
      ["USBMgmtService"] => (context(BS), body_json(SIZE_1K)).and_then(h::SetUSBMgmtService)
    };
    POST => {
      ["NTP" / "ImportKey"] => (body_multipart(context(BS), SIZE_2M, &["keys"], h::raw::UploadFile)).and_then(h::ImportKey);
      ["UpdateService" / "FirmwareUpdate"] => (body_multipart(context(BS), SIZE_90M, &["hpm"], h::raw::UploadHpmFile)).and_then(h::FirmwareUpdate);
      ["UpdateService" / "Reset"] => (context(BS), empty_body()).and_then(h::Reset);
      ["UpdateService" / "SwitchImage"] => (context(BS), empty_body()).and_then(h::SwitchImage);
      ["Configuration" / "ImportConfig"] => (body_multipart(context(UM), SIZE_1M, &["xml"], h::raw::UploadFile)).and_then(h::ImportConfig);
      ["Configuration" / "ExportConfig"] => (context(UM), empty_body()).and_then(h::ExportConfig);
      ["Language" / "OpenLanguage"] => (body_json(SIZE_1K), context(BS)).and_then(h::OpenLanguage);
      ["Language" / "CloseLanguage"] => (body_json(SIZE_1K), context(BS)).and_then(h::CloseLanguage);
      ["LicenseService" / "DeleteLicense"] => (context(UM), empty_body()).and_then(h::DeleteLicense);
      ["LicenseService" / "DisableLicense"] => (context(UM), empty_body()).and_then(h::DisableLicense);
      ["LicenseService" / "ExportLicense"] => (context(UM), empty_body()).and_then(h::ExportLicense);
      ["LicenseService" / "InstallLicense"] => (body_multipart(context(UM), SIZE_1M, &["xml"], h::raw::UploadFile)).and_then(h::InstallLicense);
      ["iBMA" / "USBStickControl"] => (context(VM), body_json(SIZE_1K)).and_then(h::USBStickControl);
      ["UpdateService" / "SPFWUpdate"] => (body_multipart_save_file_sp(context(BS), SIZE_500M, &["zip", "asc", "p7s"], h::raw::UploadFile)).and_then(h::SPFWUpdate);
      ["UpdateService" / "DeleteSPUpdateFile"] => (context(BS), empty_body()).and_then(h::DeleteSPUpdateFile);
      ["Failover" / "ForceFailover"] => (context(BS)).and_then(h::SetFailover)
    }
  )
  .boxed()
}
