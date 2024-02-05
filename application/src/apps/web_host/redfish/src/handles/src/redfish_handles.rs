/*
 * @Author: your name
 * @Date: 2021-07-09 16:32:32
 * @LastEditTime: 2021-07-26 19:15:59
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \V2R2_trunk\application\src\apps\web_host\redfish\src\handles\src\redfish_handles.rs
 */
use crate::datas::{RedfishReq, SmsReq};
use core::{multipart::FormData, Dflib, User as RawUser};
use redfish_base::{AuthInfo, Context, RawString, RedfishReply};
use serde_json::Value as JsonValue;

defhandle!(redfish, {
  ProxyRedfish(req: RedfishReq) -> core::Reply<RawString>;
  ProxySms(req: SmsReq) -> core::Reply<String>;
  UploadFile(form_data: FormData) -> RedfishReply;
  UploadHpmFile(form_data: FormData) -> RedfishReply;
  UploadFirmwareInventory(form_data: FormData) -> RedfishReply;
  UploadFirmwareInventoryMemory(form_data: FormData) -> RedfishReply;
  UploadFileCheck(data: JsonValue, ctx: Context) -> RedfishReply;
  DownloadStaticFile(ctx: Context) -> RedfishReply;

  // SessionService
  AuthToken(data: AuthInfo) -> core::Reply<RawUser>;

  // UpdateService
  post_update_firmware_inventory(formdata: JsonValue) -> RedfishReply;
});
