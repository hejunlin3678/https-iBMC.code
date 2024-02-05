#[macro_use]
extern crate core;

use core::{body_json, header_host, multipart::body_multipart, query};
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::{http::Response, Filter};
use webrest_handles as h;
use webrest_handles::{context, empty_body};

const SIZE_1K: u64 = 1024;
const SIZE_1M: u64 = 1024 * SIZE_1K;
const RO: Privilege = Privilege::READONLY;
const DM: Privilege = Privilege::DIAGNOSEMGNT;
const SM: Privilege = Privilege::SECURITYMGNT;
const BS: Privilege = Privilege::BASICSETTING;

pub fn routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    GET => {
      ["Alarm"] => (context(RO)).and_then(h::GetAlarm);
      ["EventObjectType"] => (context(RO)).and_then(h::GetEventObjectType);
      ["SyslogNotification"] => (context(RO)).and_then(h::GetSyslogNotification);
      ["EmailNotification"] => (context(RO)).and_then(h::GetEmailNotification);
      ["TrapNotification"] => (context(RO)).and_then(h::GetTrapNotification);
      ["FDM"] => (context(RO)).and_then(h::GetFDM);
      ["FDM" / u32 / "EventReports"] => (context(RO)).and_then(h::GetFDMEventReports);
      ["FDM" / "Device" / String] => (context(RO)).and_then(h::GetFDMDeviceInfo);
      ["SystemDiagnostic"] => (context(DM)).and_then(h::GetSystemDiagnostic);
      ["OperationLog"] => (query(), context(SM)).and_then(h::GetOperationLog);
      ["RunLog"] => (query(), context(RO)).and_then(h::GetRunLog);
      ["SecurityLog"] => (query(), context(SM)).and_then(h::GetSecurityLog);
      ["WorkRecord"] => (context(RO)).and_then(h::GetWorkRecord);
      ["Enclosure" / "RemovedEventSeverity"] => (context(RO)).and_then(h::GetRemovedEventSeverity)
    };
    PATCH => {
      ["SyslogNotification"] => (body_json(SIZE_1K), context(SM)).and_then(h::UpdateSyslogNotification);
      ["EmailNotification"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateEmailNotification);
      ["TrapNotification"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateTrapNotification);
      ["SystemDiagnostic"] => (context(DM), body_json(SIZE_1K)).and_then(h::UpdateSystemDiagnostic);
      ["Enclosure" / "RemovedEventSeverity"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateRemovedEventSeverity)
    };
    POST => {
      ["QueryEvent"] => (body_json(SIZE_1K), context(RO)).and_then(h::GetEvent);
      ["DownloadEvent"] => (context(RO), empty_body()).and_then(h::DownloadEvent);
      ["ClearEvent"] => (context(BS)).and_then(h::ClearEvent);
      ["TestSyslogNotification"] => (body_json(20), context(SM)).and_then(h::TestSyslogNotification);
      ["EmailNotification" / "ImportRootCertificate"] =>
          (body_multipart(context(SM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile))
          .and_then(h::ImportEmailRootCertificate);
      ["SyslogNotification" / "ImportRootCertificate"] =>
          (body_multipart(context(SM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile))
          .and_then(h::ImportSyslogRootCertificate);
      ["SyslogNotification" / "ImportClientCertificate"] =>
          (body_multipart(context(SM), SIZE_1M, &["pfx","p12"], h::raw::UploadFile))
          .and_then(h::ImportSyslogClientCertificate);
      ["SyslogNotification" / "ImportCrl"] =>
          (body_multipart(context(SM), SIZE_1M, &["crl"], h::raw::UploadFile))
          .and_then(h::ImportSyslogCrl);
      ["SyslogNotification" / "DeleteCrl"] => (context(SM)).and_then(h::DeleteSyslogCrl);
      ["TestEmailNotification"] => (body_json(20), context(BS)).and_then(h::TestEmailNotification);
      ["TestTrapNotification"] => (body_json(20), context(BS)).and_then(h::TestTrapNotification);
      ["SystemDiagnostic" / "DownloadBlackBox"] => (context(DM), empty_body()).and_then(h::DownloadBlackBox);
      ["SystemDiagnostic" / "DownloadSerialPortData"] => (context(DM), empty_body()).and_then(h::DownloadSerialPort);
      ["SystemDiagnostic" / "DownloadNPULog"] => (context(DM), body_json(SIZE_1K)).and_then(h::DownloadNPULog);
      ["SystemDiagnostic" / "DownloadVideo"] => (context(DM), body_json(SIZE_1K)).and_then(h::DownloadVideo);
      ["SystemDiagnostic" / "ExportVideoStartupFile"] => (context(DM), header_host(), body_json(SIZE_1K)).and_then(h::ExportVideoStartupFile);
      ["SystemDiagnostic" / "StopVideo"] => (context(DM)).and_then(h::StopVideo);
      ["SystemDiagnostic" / "CaptureScreenshot"] => (context(DM)).and_then(h::CaptureScreenshot);
      ["DownloadOperationLog"] => (context(SM), empty_body()).and_then(h::DownloadOperationLog);
      ["DownloadRunLog"] => (context(RO), empty_body()).and_then(h::DownloadRunLog);
      ["DownloadSecurityLog"] => (context(SM), empty_body()).and_then(h::DownloadSecurityLog);
      ["DownloadBlackBoxOfSdi"] => (context(SM), empty_body()).and_then(h::DownloadBlackBoxOfSdi);
      ["WorkRecord"] => (context(RO), body_json(SIZE_1K)).and_then(h::AddWorkRecord)
    };
    PUT => {
      ["WorkRecord"] => (context(RO), body_json(SIZE_1K)).and_then(h::UpdateWorkRecord)
    };
    DELETE => {
      ["SystemDiagnostic" / "DeleteScreenshot"] => (context(DM)).and_then(h::DeleteScreenshot);
      ["WorkRecord" / u32] => (context(RO)).and_then(h::DeleteWorkRecord)
    }
  )
  .boxed()
}
