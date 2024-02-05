use super::datas::{
  AccountConfig, AdvancedSecurity, AliveMode, BootOptions, CPUAdjust, CMPConfigPram, DownloadVideoType, EmailNotification, Enabled,
  ExportVideoStartupFileConfig, FusionPatition, GetEventParam, KBRConfig, KVMConfig, KVMMode, KVMReq, KVMUri,
  LDAPConfig, LedState, LogCount, LogServerID, LoginReq, LoginRule, NetworkBandwidthHistory,
  NetworkSettings, PatchAccount, PatchPower, PatchPowerControl, PatchPowerSupply, PatchSystemDiagnostic, PatchThermal,
  PatchWorkRecord, PerformanceMonitoringThreshold, PortConfig, PowerOperateType, ProductInfo, RaidController,
  RaidDrive, RaidVolume, ReauthKey, RightManagement, SNMPConfig, SPConfig, SSOReq, SSOUri, SecurityBanner, SyslogNotification,
  TrapNotification, TwoFactorAuth, TwoFactorAuthDeleteData, USBMgmtService, USBStickControlAction, VMMConfig,
  VMMControlConfig, VNCConfig, VNCPassword, WEBService, WorkRecordConfig, NTP, SSLCSR, PatchNetworkAdapter, LanguageSetStr,
  NpuLogPram, AssetInfo, ChassisInfo, ChassisThermal, PowerMode, ChassisPowerLimit, EventSeverity, SwitchProfile, TsbInfo, DigestInfo,
  SlotIdInfo
};
use super::WebReply;
use core::multipart::FormData;
use core::{Dflib, Reply, User};
use serde_json::Value as JsonValue;

defhandle!(webrest, {
  AuthUser(method: String, url: String, sessionId: String, scrf_token: Option<String>) -> Reply<User>;
  UploadFile(form_data: FormData) -> Reply<JsonValue>;
  UploadHpmFile(form_data: FormData) -> Reply<JsonValue>;

  // 登录页、首页
  GetLogin() -> WebReply;
  MutualLogin(clientIp: Option<String>, serial: Option<String>, issuer: Option<String>, subject: Option<String>, logout: Option<String>, SessionId: Option<String>) -> WebReply;
  KBRSSOLogin(authorization: Option<String>) -> WebReply;
  GetSessions(user: User) -> WebReply;
  GetAccounts(user: User) -> WebReply;
  Login(body: LoginReq, clientIp: Option<String>, SessionId: Option<String>) -> WebReply;
  SSOLogin(body: SSOReq, clientIp: Option<String>) -> WebReply;
  KeepAlive(body: AliveMode, user: User) -> WebReply;
  SSOAccess(body: SSOUri, clientIp: Option<String>, hostname: Option<String>, SessionId: Option<String>, token: Option<String>) -> WebReply;
  DeleteSession(sessionId: String, user: User) -> WebReply;
  GetGenericInfo(user: User) -> WebReply;
  UpdateGenericInfo(body: LedState, user: User) -> WebReply;
  GetOverview(user: User) -> WebReply;
  GetChassisOverview(user: User) -> WebReply;
  UpdateChassisOverview(body: ChassisInfo,user: User) -> WebReply;
  RestoreFactory(body: ReauthKey, user: User) -> WebReply;
  Dump(user: User) -> WebReply;
  AccessKVM(body: KVMUri, user_agent: Option<String>, sessionid: Option<String>, authParam: Option<String>, hostname: Option<String>) -> WebReply;
  AuthKVM(clientIp: Option<String>, body: KVMReq) -> WebReply;
  AuthJar(uri: Option<String>) -> WebReply;

  // 系统信息
  GetProductInfo(user: User) -> WebReply;
  UpdateProductInfo(body: ProductInfo, user: User) -> WebReply;
  GetProcessor(user: User) -> WebReply;
  GetMemory(user: User) -> WebReply;
  GetThresholdSensor(user: User) -> WebReply;
  GetNetworkAdapter(user: User) -> WebReply;
  SetNetworkAdapter(adapterId: String, user: User, data: PatchNetworkAdapter) -> WebReply;
  GetBoards(user: User) -> WebReply;
  GetExpandBoard(user: User) -> WebReply;
  GetDiskBackplane(user: User) -> WebReply;
  GetLeakageDetectionCard(user: User) -> WebReply;
  GetPCIeRiserCard(user: User) -> WebReply;
  GetRAIDCard(user: User) -> WebReply;
  GetM2TransformCard(user: User) -> WebReply;
  GetPassThroughCard(user: User) -> WebReply;
  GetMEZZCard(user: User) -> WebReply;
  GetPCIeTransformCard(user: User) -> WebReply;
  GetGPUBoard(user: User) -> WebReply;
  GetFanBackplane(user: User) -> WebReply;
  GetHorizontalConnectionBoard(user: User) -> WebReply;
  GetCPUBoard(user: User) -> WebReply;
  GetMemoryBoard(user: User) -> WebReply;
  GetIOBoard(user: User) -> WebReply;
  GetEcuBoard(user: User) -> WebReply;
  GetChassisBackplane(user: User) -> WebReply;
  GetBBUModule(user: User) -> WebReply;
  GetSecurityModule(user: User) -> WebReply;
  GetLCD(user: User) -> WebReply;
  GetPeripheralFirmware(user: User) -> WebReply;
  GetSDCardController(user: User) -> WebReply;
  GetOCPCard(user: User) -> WebReply;
  GetPCIeCard(user: User) -> WebReply;
  GetCICCard(user: User) -> WebReply;
  GetARCard(user: User) -> WebReply;
  GetConvergeBoard(user: User) -> WebReply;
  
  // 组件信息
  GetUnits(user: User) -> WebReply;
  GetBasicComputingUnit(user: User) -> WebReply;
  GetExtensionUnit(user: User) -> WebReply;
  GetStorageExtensionUnit(user: User) -> WebReply;
  GetIOExtensionUnit(user: User) -> WebReply;
  GetCoolingUnit(user: User) -> WebReply;

  // 性能监控
  GetProcesserUsage(user: User) -> WebReply;
  UpdateProcesserUsage(user: User, body: PerformanceMonitoringThreshold) -> WebReply;
  GetMemoryUsage(user: User) -> WebReply;
  UpdateMemoryUsage(user: User, body: PerformanceMonitoringThreshold) -> WebReply;
  GetDiskUsage(user: User) -> WebReply;
  UpdateDiskUsage(user: User, body: PerformanceMonitoringThreshold) -> WebReply;
  GetNetworkBandwidthUsage(user: User) -> WebReply;
  UpdateNetworkBandwidthUsage(user: User, body: PerformanceMonitoringThreshold) -> WebReply;
  ClearNetworkBandwidthHistory(user: User, body: NetworkBandwidthHistory) -> WebReply;

  // 存储管理
  GetViewsSummary(user:User) -> WebReply;
  GetRaidController(idStr:String, user:User) -> WebReply;
  GetLogicalDrive(controllerId:String, volumeId:String, user:User) -> WebReply;
  GetPhysicalDrives(idStr:String, user:User) -> WebReply;
  SetRaidController(controllerId:String , user: User, body:RaidController) -> WebReply;
  SetLogicalDrive(controllerId:String, volumeId:String, user: User, body:RaidVolume) -> WebReply;
  SetPhysicalDrives(driveId:String, user: User, body:RaidDrive) -> WebReply;
  AddLogicalDrive(controllerId:String, user: User, body:RaidVolume) -> WebReply;
  DeleteLogicalDrive(controllerId:String, volumeId:String, user: User) -> WebReply;
  RestoreDefaultSettings(controllerId:String, user: User) -> WebReply;
  ImportForeignConfig(controllerId:String, user: User) -> WebReply;
  ClearForeignConfig(controllerId:String, user: User) -> WebReply;
  PhysicalDrivesCryptoErase(driveId:String, user: User) -> WebReply;
  DumpControllerLog(controllerId:String, user: User) -> WebReply;

  // 电源 & 功率
  GetPowerSupply(user: User) -> WebReply;
  UpdatePowerSupply(body: PatchPowerSupply, user: User) -> WebReply;
  GetPower(user: User) -> WebReply;
  UpdatePower(user: User, body: PatchPower) -> WebReply;
  ResetPowerStatistics(user: User) -> WebReply;
  DeletePowerHistory(user: User) -> WebReply;
  GetPowerHistory(user: User) -> WebReply;
  GetPowerControl(user: User) -> WebReply;
  UpdatePowerControl(user: User, body: PatchPowerControl) -> WebReply;
  SetPowerOperateType(user: User, body: PowerOperateType) -> WebReply;
  GetChassisPowerSupply(user: User) -> WebReply;
  GetChassisPower(user: User) -> WebReply;
  UpdatePowerSleepMode(body: PowerMode, user: User) -> WebReply;
  UpdateChassisPower(body: ChassisPowerLimit, user: User) -> WebReply;

  // 风扇 & 散热
  GetThermal(user: User) -> WebReply;
  UpdateThermal(data: PatchThermal, user: User) -> WebReply;
  ClearInletHistory(user: User) -> WebReply;
  GetChassisThermal(user: User) -> WebReply;
  UpdateChassisThermal(body: ChassisThermal, user: User) -> WebReply;

  // BIOS 配置
  GetBootOptions(user: User) -> WebReply;
  UpdateBootOptions(user: User, body: BootOptions) -> WebReply;
  GetCPUAdjust(user: User) -> WebReply;
  UpdateCPUAdjust(user: User, body: CPUAdjust) -> WebReply;

  // 电池管理
  GetBatteryInfo(user: User) -> WebReply;

  // 资产管理
  UpdateAssetInfo(UNum:String, user: User, body:AssetInfo) -> WebReply;

  // SDI卡管理
  SetSdiCardNMIMethod(user: User, body: SlotIdInfo) -> WebReply;

  //硬分区设置
  GetFusionPartition(user: User) -> WebReply;
  SetFusionPartition(user: User, data: FusionPatition) -> WebReply;

  // 告警 & 事件
  GetAlarm(user: User) -> WebReply;
  GetEvent(data: GetEventParam, user: User) -> WebReply;
  GetEventObjectType(user: User) -> WebReply;
  DownloadEvent(user: User) -> WebReply;
  ClearEvent(user: User) -> WebReply;

  // 告警上报
  GetSyslogNotification(user: User) -> WebReply;
  UpdateSyslogNotification(data: SyslogNotification, user: User) -> WebReply;
  TestSyslogNotification(ID: LogServerID, user: User) -> WebReply;
  ImportEmailRootCertificate(data: JsonValue) -> WebReply;
  ImportSyslogRootCertificate(data: JsonValue) -> WebReply;
  ImportSyslogClientCertificate(data: JsonValue) -> WebReply;
  ImportSyslogCrl(data: JsonValue) -> WebReply;
  DeleteSyslogCrl(user: User) -> WebReply;
  GetEmailNotification(user: User) -> WebReply;
  UpdateEmailNotification(data: EmailNotification, user: User) -> WebReply;
  TestEmailNotification(ID: LogServerID, user: User) -> WebReply;
  GetTrapNotification(user: User) -> WebReply;
  UpdateTrapNotification(data: TrapNotification, user: User) -> WebReply;
  TestTrapNotification(ID: LogServerID, user: User) -> WebReply;

  // FDM
  GetFDM(user: User) -> WebReply;
  GetFDMEventReports(alias: u32, user: User) -> WebReply;
  GetFDMDeviceInfo(refObjName: String, user: User) -> WebReply;

  // 录像截屏、系统日志
  GetSystemDiagnostic(user: User) -> WebReply;
  UpdateSystemDiagnostic(user: User, body: PatchSystemDiagnostic) -> WebReply;
  DownloadBlackBox(user: User) -> WebReply;
  DownloadSerialPort(user: User) -> WebReply;
  DownloadNPULog(user: User, body: NpuLogPram) -> WebReply;
  DownloadVideo(user: User, body: DownloadVideoType) -> WebReply;
  ExportVideoStartupFile(user: User, hostname: Option<String>, body: ExportVideoStartupFileConfig) -> WebReply;
  StopVideo(user: User) -> WebReply;
  CaptureScreenshot(user: User) -> WebReply;
  DeleteScreenshot(user: User) -> WebReply;

  // iBMC日志
  GetOperationLog(body: LogCount, user: User) -> WebReply;
  DownloadOperationLog(user: User) -> WebReply;
  GetRunLog(body: LogCount, user: User) -> WebReply;
  DownloadRunLog(user: User) -> WebReply;
  GetSecurityLog(body: LogCount, user: User) -> WebReply;
  DownloadSecurityLog(user: User) -> WebReply;
  DownloadBlackBoxOfSdi(user: User) -> WebReply;

  // 工作记录
  GetWorkRecord(user: User) -> WebReply;
  AddWorkRecord(user: User, body: WorkRecordConfig) -> WebReply;
  UpdateWorkRecord(user: User, body: PatchWorkRecord) -> WebReply;
  DeleteWorkRecord(id: u32, user: User) -> WebReply;

  // 本地用户
  AddAccount(body: AccountConfig, user: User) -> WebReply;
  UpdateAccount(id: u32, body: PatchAccount, user: User) -> WebReply;
  DeleteAccount(id: u32, body: ReauthKey, user: User) -> WebReply;
  DeleteSSHPublicKey(id: u32, body: ReauthKey, user: User) -> WebReply;
  ImportSSHPublicKey(id: u32 ,data: JsonValue) -> WebReply;

  //LDAP
  GetLDAP(ContorllerId:u32 , user: User) -> WebReply;
  SetLDAP(ContorllerId:u32 , body:LDAPConfig, user: User) -> WebReply;
  GetLDAPState(user: User) -> WebReply;
  SetLDAPState(body:Enabled, user: User) -> WebReply;
  ImportLDAPCertificate(ContorllerId:u32, data: JsonValue) -> WebReply;
  ImportCrlVerification(ContorllerId:u32, data: JsonValue) -> WebReply;
  DeleteCrlVerification(ContorllerId:u32, body: ReauthKey, user: User) -> WebReply;

  //kerbers
  GetKerberos(user: User) -> WebReply;
  SetKerberos(body:KBRConfig, user: User) -> WebReply;
  SetKerberosState(body:Enabled, user: User) -> WebReply;
  ImportKeyTab(data: JsonValue) -> WebReply;

  //双因素认证
  GetTwoFactorAuthentication(user: User) -> WebReply;
  SetTwoFactorAuthentication(user: User, body:TwoFactorAuth) -> WebReply;
  ImportClentCertificate(data: JsonValue) -> WebReply;
  ImportRootCertificate(data: JsonValue) -> WebReply;
  DeleteCertificate(user: User, body: TwoFactorAuthDeleteData) -> WebReply;
  ImportCrlCertificate(data: JsonValue) -> WebReply;
  DeleteCrlCertificate(id: u32, user: User) -> WebReply;

  //TPCM
  GetTPCMBaseInfo(user: User) -> WebReply;
  GetTPCMDetailedInfo(user: User) -> WebReply;
  GetTPCMTsbInfo(user: User) -> WebReply;
  SetTPCMTsbInfo(user: User, data: TsbInfo) -> WebReply;
  SetStandardDigest(user: User, data: DigestInfo) -> WebReply;

  //BIOS证书
  GetBootCertificates(user: User) -> WebReply;
  ImportBootCertActionInfo(data: JsonValue) -> WebReply;
  ImportBootCrlActionInfo(data: JsonValue) -> WebReply;
  ImportSecureBootCertActionInfo(data: JsonValue) -> WebReply;
 
  //BIOS SecureBoot 开关
  GetBiosSecureBoot(user: User) -> WebReply;
  SetBiosSecureBoot(body:JsonValue, user: User) -> WebReply;

  // 端口服务
  GetPortConfig(user: User) -> WebReply;
  SetPortConfig(body:PortConfig, user: User) -> WebReply;

  //web服务
  GetWEBService(user: User) -> WebReply;
  SetWEBService(body:WEBService, user: User) -> WebReply;
  ExportCSR(body: SSLCSR, user: User) -> WebReply;
  ImportCertificate(data: JsonValue) -> WebReply;

  //虚拟控制台
  GetKVM(user: User) -> WebReply;
  SetKVM(user: User, body:KVMConfig) -> WebReply;
  GenerateStartupFile(user: User, hostname: Option<String>, data:KVMMode) -> WebReply;
  //虚拟多媒体
  GetVMM(user: User) -> WebReply;
  SetVMM(user: User, body: VMMConfig) -> WebReply;
  VMMControl(user: User, body: VMMControlConfig) -> WebReply;

  //VNC
  GetVNC(user: User) -> WebReply;
  SetVNC(user: User, body: VNCConfig) -> WebReply;
  SetVNCPassword(user: User, body: VNCPassword) -> WebReply;
  //SNMP
  GetSNMP(user: User) -> WebReply;
  SetSNMP(body: SNMPConfig, user: User) -> WebReply;

  //网络配置
  GetNetworkSettings(user: User) -> WebReply;
  SetNetworkSettings(body: NetworkSettings, user: User) -> WebReply;

  //NTP和时区
  GetNTP(user: User) -> WebReply;
  SetNTP(body: NTP, user: User) -> WebReply;
  ImportKey(data: JsonValue) -> WebReply;

  //主备板倒换
  GetFailover(user: User) -> WebReply;
  SetFailover(user: User) -> WebReply;

  //固件升级
  GetUpdateService(user: User) -> WebReply;
  GetUpdateProgress(user: User) -> WebReply;
  FirmwareUpdate(data: JsonValue) -> WebReply;
  Reset(user: User) -> WebReply;
  SwitchImage(user: User) -> WebReply;

  //配置更新
  ImportConfig(data: JsonValue) -> WebReply;
  ExportConfig(user: User) -> WebReply;
  GetImportExportConfigProgress(user: User) -> WebReply;

  //语言管理
  GetLanguage(user: User) -> WebReply;
  OpenLanguage(body: LanguageSetStr, user: User) -> WebReply;
  CloseLanguage(body: LanguageSetStr, user: User) -> WebReply;

  //许可证管理
  GetLicenseService(user: User) -> WebReply;
  DeleteLicense(user: User) -> WebReply;
  DisableLicense(user: User) -> WebReply;
  InstallLicense(data: JsonValue) -> WebReply;
  ExportLicense(user: User) -> WebReply;

  //iBMA管理
  GetiBMA(user: User) -> WebReply;
  USBStickControl(user: User, body: USBStickControlAction) -> WebReply;

  //SP管理
  GetSP(user: User) -> WebReply;
  SetSP(user: User, body: SPConfig) -> WebReply;
  SPFWUpdate(data: JsonValue) -> WebReply;
  DeleteSPUpdateFile(user: User) -> WebReply;
  GetSPUpdateFileList(user: User) -> WebReply;

  //USB管理
  GetUSBMgmtService(user: User) -> Reply<String>;
  SetUSBMgmtService(user: User, body: USBMgmtService) -> Reply<String>;

  // 用户 & 安全: 安全设置
  GetAdvancedSecurity(user: User) -> WebReply;
  UpdateAdvancedSecurity(data: AdvancedSecurity, user: User) -> WebReply;
  GetLoginRule(user: User) -> WebReply;
  UpdateLoginRule(data: LoginRule, user: User) -> WebReply;
  GetRightManagement(user: User) -> WebReply;
  UpdateRightManagement(data: RightManagement, user: User) -> WebReply;
  GetSecurityBanner(user: User) -> WebReply;
  UpdateSecurityBanner(data: SecurityBanner, user: User) -> WebReply;

  // 用户 & 安全: 证书更新
  GetCMPConfig(user: User) -> WebReply;
  SetCMPConfig(data: CMPConfigPram, user: User) -> WebReply;
  ImportCACert(data: JsonValue) -> WebReply;
  ImportCRL(data: JsonValue) -> WebReply;
  DeleteCRL(user: User) -> WebReply;
  ImportClientCert(data: JsonValue) -> WebReply;
  UpdateCertFromCA(user: User) -> WebReply;

  GetRemovedEventSeverity(user: User) -> WebReply;
  UpdateRemovedEventSeverity(data: EventSeverity, user: User) -> WebReply;
  GetSwitchFileList(user: User) -> WebReply;
  GetSwitchFile(body: SwitchProfile, user: User) -> WebReply;
  RestoreSwiProfile(body: SwitchProfile, user: User) -> WebReply;
});
