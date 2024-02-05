use bytes::Bytes;
use chrono::{Datelike, NaiveDate, NaiveDateTime, NaiveTime};
use core::validator::{
    AllInOrNone, CheckBase64OrNone, CheckDateRangeOrNone, CheckIpv4OrNone, CheckIpv6OrNone, CheckRegexOrEmpty,
    CheckServerAddressOrNone, CheckStringRangeOrNone, DateTimeOrNone, Error as ValidatorErr, In, InOrNone, Length,
    LengthOrNone, OptionValue, Range, RangeOrNone, RangesOrNone, Regex, RegexOrNone, RegexSetOrNone, Result, UniqueOrNone,
    CheckDNSAddressOrNone, CheckIPAddressOrNone
};
use lua::JsonOption;
use serde::Deserialize;
use serde_json::Value as JsonValue;
const ASSET_INFO_REGULAR: &str = r#"^[0-9a-zA-Z_ `~!@#$%^&*()-=+{}|;",:'.\?/<>\\]*$"#;
const RACK_INFO_REGULAR: &str = r#"^(0|[1-9]\d*)$"#;
const ASSER_INFO_ALL_SPACE: &str = r#"[^\s]"#;

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LoginReq {
    #[validate(CheckUserNameLogin())]
    UserName: String,

    #[serde(deserialize_with="core::string_or_bytes")]
    #[serde(serialize_with="core::bytes_to_string")]
    Password: Bytes,

    #[validate(In(&["Local", "Ldap", "Kerberos", "Auto", "KerberosSSO"]))]
    Type: String,

    Domain: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct KVMReq {
    #[validate(CheckUserNameLogin())]
    user_name: String,
    #[validate(Length(1, 255))]
    check_pwd: String,
    #[validate(Range(0, 1))]
    logtype: u8,
    #[validate(InOrNone(&["DirectKVM"]))]
    func: Option<String>,
    #[validate(RangeOrNone(0, 1))]
    KvmMode: Option<u8>,
    #[validate(RangeOrNone(0, 1))]
    IsKvmApp: Option<u8>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct KVMUri {
    authParam: Option<String>,
    #[validate(InOrNone(&["html5", "jre"]))]
    openWay: Option<String>,
    #[validate(InOrNone(&["html5", "jre"]))]
    openway: Option<String>,
    #[validate(RegexOrNone(r"^[a-zA-Z0-9]{1,6}$"))]
    lp: Option<String>,
    #[validate(RegexOrNone(r"^[a-zA-Z0-9]{1,6}$"))]
    lang: Option<String>,
    #[validate(InOrNone(&["jump", "notjump"]))]
    jumpflag: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SSOReq {
    #[validate(CheckUserNameOrNone())]
    username: Option<String>,
    #[validate(LengthOrNone(1, 255))]
    password: Option<String>,
    #[validate(LengthOrNone(1, 255), CheckIPAddressOrNone())]
    ip: Option<String>,
    #[validate(RegexOrNone(r"^[a-zA-Z0-9]{1,128}$"))]
    token: Option<String>,
    #[validate(In(&["addtoken", "destroytoken"]))]
    function: String,
    #[validate(InOrNone(&["ldap", "local"]))]
    logtype: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SSOUri {
    #[validate(LengthOrNone(1, 128), RegexOrNone(r"^[a-zA-Z0-9]+$"))]
    token: Option<String>,
    #[validate(InOrNone(&["index", "console"]))]
    redirect: Option<String>,
    #[validate(InOrNone(&["dedicate", "share"]))]
    kvmmode: Option<String>,
    #[validate(InOrNone(&["html5", "java"]))]
    openway: Option<String>,
    #[validate(RegexOrNone(r"^[a-zA-Z0-9]{1,6}$"))]
    lang: Option<String>,
    #[validate(InOrNone(&["/v1/bmcPage/#/home"]))]
    prefix: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct AliveMode {
    #[validate(In(&["Activate", "Deactivate"]))]
    Mode: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LedState {
    #[validate(InOrNone(&["Lit", "Off", "Blinking"]))]
    IndicatorLEDState: Option<String>,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct ChassisInfo {
      #[validate(LengthOrNone(0,20))]
      ChassisName: Option<String>,
      #[validate(RangeOrNone(0, 999999))]
      ChassisID: Option<u32>,
      #[validate(LengthOrNone(0,20))]
      ChassisLocation: Option<String>,
    }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct DigitalWarranty {
    #[validate(RegexOrNone(r"^[0-9]{4}-[0-9]{2}-[0-9]{2}$"))]
    StartPoint: Option<String>,
    Lifespan: Option<u8>,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct RackInfo {
        #[validate(RangeOrNone(1, 2147483647))]
        LoadCapacityKg: JsonOption<u32>,
        #[validate(RangeOrNone(1, 2147483647))]
        RatedPowerWatts: JsonOption<u32>,
        #[validate(CheckStringRangeOrNone(0, 500), RegexOrNone(RACK_INFO_REGULAR))]
        Rack: Option<String>,
        #[validate(CheckStringRangeOrNone(0, 500), RegexOrNone(RACK_INFO_REGULAR))]
        Row: Option<String>,
        #[validate(LengthOrNone(1, 48), RegexOrNone(ASSER_INFO_ALL_SPACE))]
        Name: Option<String>,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct ProductInfo {
      #[validate(LengthOrNone(0, 48))]
      AssetTag: Option<String>,
      DeviceLocation: Option<String>,
      #[validate]
      DigitalWarranty: Option<DigitalWarranty>,
      #[validate]
      RackInfo: Option<RackInfo>,
    }
}

fn CheckAssetInfoExtendFieldItem(name: &str, k: &String, v: &JsonValue) -> Result {
    let field_name = format!("{}.{}", name, k);
    Length(&field_name, k, 1, 48)?;
    Regex(&field_name, k, "^[[:alnum:]]+$")?;
    if let JsonValue::String(s) = v {
      Length(&field_name, s, 0, 48)?;
      Regex(&field_name, s, ASSET_INFO_REGULAR)?;
      return Ok(());
    }
    Err(ValidatorErr::new_ensure("CheckAssetInfoExtendField", &field_name))
}
  
pub fn CheckAssetInfoExtendField(name: &'static str, val: &Option<JsonValue>) -> Result {
    match val {
      None => Ok(()),
      Some(JsonValue::Object(obj)) => {
        let mut errs = vec![];
        for (k, v) in obj {
          let _ = CheckAssetInfoExtendFieldItem(name, k, v).map_err(|e| errs.push(e));
        }
        if !errs.is_empty() {
          return Err(ValidatorErr::new_errs(name, errs));
        }
        Ok(())
      }
      _ => Err(ValidatorErr::new_ensure("CheckAssetInfoExtendField", name)),
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct AssetInfo {
      #[validate(Length(1, 48), Regex(ASSET_INFO_REGULAR), Regex(ASSER_INFO_ALL_SPACE))]
      Model: String,
      #[validate(Length(1, 48), Regex(ASSET_INFO_REGULAR), Regex(ASSER_INFO_ALL_SPACE))]
      DeviceType: String,
      #[validate(Length(1, 48), Regex(ASSET_INFO_REGULAR), Regex(ASSER_INFO_ALL_SPACE))]
      Manufacturer: String,
      #[validate(Length(1, 48), Regex(ASSET_INFO_REGULAR), Regex(ASSER_INFO_ALL_SPACE))]
      SerialNumber: String,
      #[validate(LengthOrNone(0, 48), RegexOrNone(ASSET_INFO_REGULAR))]
      PartNumber: Option<String>,
      #[validate(Range(1, 2147483647))]
      UHeight: u32,
      #[validate(RangeOrNone(1, 255))]
      LifeCycleYear: Option<u32>,
      #[validate(CheckDateRangeOrNone("%Y/%m/%d", 1970, 2099))]
      CheckInTime: Option<String>,
      #[validate(RangeOrNone(1, 2147483647))]
      WeightKg: Option<u32>,
      #[validate(RangeOrNone(1, 2147483647))]
      RatedPowerWatts: Option<u32>,
      #[validate(LengthOrNone(0, 48), RegexOrNone(ASSET_INFO_REGULAR))]
      AssetOwner: Option<String>,
      #[validate(LengthOrNone(0, 5), CheckAssetInfoExtendField())]
      ExtendField: Option<JsonValue>,
    }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct WriteCachePolicy {
    ConfiguredDrive: Option<String>,
    UnconfiguredDrive: Option<String>,
    HBADrive: Option<String>,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct ConsisCheckInfo {
      Enabled: Option<bool>,
      #[validate(RangeOrNone(1, 1440))]
      Period: Option<u16>,
      #[validate(InOrNone(&["Low", "Medium", "High"]))]
      Rate: Option<String>,
      AutoRepairEnabled: Option<bool>,
      RunningStatus: Option<bool>,
      #[validate(RangeOrNone(0, 65535))]
      TotalVolumeCount: Option<u16>,
      #[validate(RangeOrNone(0, 65535))]
      CompletedVolumeCount: Option<u16>,
      #[validate(RangeOrNone(0, 24))]
      DelayForStart: Option<u32>,
    }
}

define_param! {
  #[portal(ToLua, validate)]
  #[derive(Deserialize)]
  pub struct RaidController {
    #[validate(InOrNone(&["RAID", "HBA", "JBOD", "Mixed"]))]
    Mode: Option<String>,
    CopyBackState: Option<bool>,
    SmarterCopyBackState: Option<bool>,
    JBODState: Option<bool>,
    NoBatteryWriteCacheEnabled: Option<bool>,
    ReadCachePercent: Option<u8>,
    #[validate]
	WriteCachePolicy: Option<WriteCachePolicy>,
    BootDevices: Option<Vec<String>>,
    #[validate]
    ConsisCheckInfo: Option<ConsisCheckInfo>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct RaidVolume {
    #[validate(LengthOrNone(0, 64))]
    Name: Option<String>,
    CreateCacheCadeFlag: Option<bool>,
    OptimumIOSizeBytes: Option<u32>,
    CapacityMBytes: Option<u32>,
    #[validate(InOrNone(&["NoReadAhead", "ReadAhead"]))]
    DefaultReadPolicy: Option<String>,
    #[validate(InOrNone(&["WriteThrough", "WriteBackWithBBU", "WriteBack"]))]
    DefaultWritePolicy: Option<String>,
    #[validate(InOrNone(&["CachedIO", "DirectIO"]))]
    DefaultCachePolicy: Option<String>,
    #[validate(InOrNone(&["ReadWrite", "ReadOnly", "Blocked", "Hidden"]))]
    AccessPolicy: Option<String>,
    #[validate(InOrNone(&["Unchanged", "Enabled", "Disabled"]))]
    DriveCachePolicy: Option<String>,
    BGIEnabled: Option<bool>,
    BootEnabled: Option<bool>,
    BootPriority: Option<String>,
    SSDCachingEnabled: Option<bool>,
    #[validate(InOrNone(&["UnInit", "QuickInit", "FullInit","RPI","OPO","Front","Background"]))]
    InitializationMode: Option<String>,
    #[validate(InOrNone(&["RAID0", "RAID1", "RAID5", "RAID6", "RAID10", "RAID50", "RAID60", "RAID1(ADM)", "RAID10(ADM)", "RAID1Triple", "RAID10Triple"]))]
    VolumeRaidLevel: Option<String>,
    SpanNumber: Option<u8>,
    Drives: Option<Vec<u8>>,
    ArrayId: Option<u16>,
    BlockIndex: Option<u8>,
    #[validate(InOrNone(&["None", "ControllerCache", "IOBypass"]))]
    AccelerationMethod: Option<String>,
    AssociatedVolumes: Option<Vec<String>>,
    #[validate(InOrNone(&["64K", "256K"]))]
    CacheLineSize: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct RaidDrive {
    #[validate(InOrNone(&["Off", "Blinking"]))]
    IndicatorLED: Option<String>,
    #[validate(InOrNone(&["None", "Global", "Dedicated", "Auto Replace"]))]
    HotspareType: Option<String>,
    #[validate(LengthOrNone(0, 128))]
    FirmwareStatus: Option<String>,
    #[validate(LengthOrNone(0, 128))]
    SpareforLogicalDrive: Option<Vec<String>>,
    BootEnabled: Option<bool>,
    BootPriority: Option<String>,
    PatrolState: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PerformanceMonitoringThreshold {
    #[validate(RangeOrNone(0, 100))]
    ThresholdPercent: Option<u32>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct NetworkBandwidthHistory {
    BWUWaveTitle: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SupplyList {
    #[validate(Length(1, 128))]
    Name: String,
    #[validate(In(&["Active", "Standby", "Shared"]))]
    Mode: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchPowerSupply {
    #[validate(InOrNone(&["On", "Off"]))]
    DeepSleep: Option<String>,

    #[validate(InOrNone(&["Active/Standby", "Sharing"]))]
    PowerMode: Option<String>,

    #[validate]
    SupplyList: Option<Vec<SupplyList>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PowerLimit {
    #[validate(InOrNone(&["On", "Off"]))]
    LimitState: Option<String>,
    LimitInWatts: Option<u16>,

    #[validate(InOrNone(&["HardPowerOff", "NoAction", "Reset"]))]
    LimitException: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchPower {
    #[validate]
    PowerLimit: Option<PowerLimit>,

    HighPowerThresholdWatts: Option<u32>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PowerRestoreDelay {
    #[validate(InOrNone(&["DefaultDelay", "HalfDelay", "FixedDelay", "RandomDelay"]))]
    Mode: Option<String>,
    Seconds: Option<JsonValue>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PowerOffAfterTimeout {
    #[validate(InOrNone(&["On", "Off"]))]
    Enabled: Option<String>,
    TimeoutSeconds: Option<u32>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchPowerControl {
    #[validate(InOrNone(&["TurnOn", "RestorePreviousState", "StayOff"]))]
    PowerOnStrategy: Option<String>,

    #[validate(InOrNone(&["On", "Off"]))]
    PanelPowerButtonEnabled: Option<String>,

    #[validate]
    PowerRestoreDelay: Option<PowerRestoreDelay>,

    #[validate]
    PowerOffAfterTimeout: Option<PowerOffAfterTimeout>,

    #[validate(InOrNone(&["ManualPowerOff", "AutoPowerOff"]))]
    LeakStrategy: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PowerOperateType {
    #[validate(In(&["On", "ForceOff", "GracefulShutdown", "ForceRestart", "Nmi", "ForcePowerCycle"]))]
    OperateType: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct Controller {
    #[validate(LengthOrNone(0, 255), CheckServerAddressOrNone())]
    IPAddress: Option<String>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    #[validate(LengthOrNone(0, 255))]
    Domain: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    Folder: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    BindDN: Option<String>,
    #[validate(LengthOrNone(1, 20))]
    BindPwd: Option<String>,
    UserFolder: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct Certificate {
    VerificationEnabled: Option<bool>,
    #[validate(InOrNone(&["Demand", "Allow"]))]
    VerificationLevel: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LdapGroup {
    #[validate(RangeOrNone(0, 5))]
    ID: Option<u32>,
    #[validate(LengthOrNone(0, 255))]
    Name: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    Folder: Option<String>,
    VerificationEnabled: Option<bool>,
    #[validate(InOrNone(&["Administrator", "Operator", "Common User", "Custom Role 1", "Custom Role 2", "Custom Role 3", "Custom Role 4", "No Access"]))]
    RoleID: Option<String>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Rule1", "Rule2", "Rule3"]))]
    LoginRule: Option<Vec<String>>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Web", "SSH", "Redfish"]))]
    LoginInterface: Option<Vec<String>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LDAPConfig {
    LDAPEnabled: Option<bool>,
    #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
    ReauthKey: Option<String>,
    #[validate]
    Controller: Option<Controller>,
    #[validate]
    Certificate: Option<Certificate>,
    #[validate]
    LdapGroup: Option<Vec<LdapGroup>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct KBRGroup {
    #[validate(RangeOrNone(0, 5))]
    ID: Option<u32>,
    #[validate(LengthOrNone(0, 255))]
    Name: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    SID: Option<String>,
    VerificationEnabled: Option<bool>,
    #[validate(InOrNone(&["Administrator", "Operator", "Common User", "Custom Role 1", "Custom Role 2", "Custom Role 3", "Custom Role 4", "No Access"]))]
    RoleID: Option<String>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Rule1", "Rule2", "Rule3"]))]
    LoginRule: Option<Vec<String>>,
    #[validate(LengthOrNone(0, 2), UniqueOrNone(), AllInOrNone(&["Web", "Redfish"]))]
    LoginInterface: Option<Vec<String>>,
    #[validate(LengthOrNone(0, 255))]
    Domain: Option<String>,
    DelGroupFlag: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct KBRConfig {
    #[validate(LengthOrNone(0, 255), CheckServerAddressOrNone())]
    IPAddress: Option<String>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    #[validate(LengthOrNone(0, 255))]
    Domain: Option<String>,
    #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
    ReauthKey: Option<String>,
    #[validate]
    Group: Option<Vec<KBRGroup>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct TwoFactorAuth {
    TwoFactorAuthenticationEnabled: Option<bool>,
    OCSPEnabled: Option<bool>,
    CRLEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct TwoFactorAuthDeleteData {
    #[validate(Range(0, 1))]
    CertificateType: u32,
    #[validate(Range(1, 255))]
    ID: u32,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct Enabled {
    Enabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PortEnabled {
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    Enabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PortSpareEnabled {
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    #[validate(RangeOrNone(1, 65535))]
    SparePort: Option<u32>,
    Enabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PortConfig {
    #[validate]
    SSH: Option<PortEnabled>,
    #[validate]
    VMM: Option<PortEnabled>,
    #[validate]
    VNC: Option<PortEnabled>,
    #[validate]
    KVM: Option<PortEnabled>,
    #[validate]
    Video: Option<PortEnabled>,
    #[validate]
    NAT: Option<PortEnabled>,
    #[validate]
    SNMPAgent: Option<PortEnabled>,
    #[validate]
    HTTP: Option<PortEnabled>,
    #[validate]
    HTTPS: Option<PortEnabled>,
    #[validate]
    IPMILANRMCP: Option<PortSpareEnabled>,
    #[validate]
    IPMILANRMCPPlus: Option<Enabled>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct WebSession {
    #[validate(RangeOrNone(5, 480))]
    TimeoutMinutes: Option<u32>,
    #[validate(InOrNone(&["Shared", "Private"]))]
    Mode: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct WEBService {
    #[validate]
    HTTP: Option<PortEnabled>,
    #[validate]
    HTTPS: Option<PortEnabled>,
    #[validate]
    WebSession: Option<WebSession>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SSLCSR {
    #[validate(Length(2, 2))]
    Country: String,
    #[validate(LengthOrNone(0, 128))]
    State: Option<String>,
    #[validate(LengthOrNone(0, 128))]
    City: Option<String>,
    #[validate(LengthOrNone(0, 64))]
    OrgName: Option<String>,
    #[validate(LengthOrNone(0, 64))]
    OrgUnit: Option<String>,
    #[validate(Length(1, 64))]
    CommonName: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SSLCertificate {
    #[validate(InOrNone(&["Server", "Custom"]))]
    CertificateType: Option<String>,
    #[validate(InOrNone(&["URI"]))]
    Type: Option<String>,
    Content: Option<String>,
    Password: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct KVMConfig {
    Enabled: Option<bool>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    #[validate(RangeOrNone(0, 480))]
    SessionTimeoutMinutes: Option<u32>,
    EncryptionEnabled: Option<bool>,
    LocalKvmEnabled: Option<bool>,
    PersistentUSBConnectionEnabled: Option<bool>,
    AutoOSLockEnabled: Option<bool>,
    #[validate(InOrNone(&["Custom", "Windows"]))]
    AutoOSLockType: Option<String>,
    AutoOSLockKey: Option<Vec<String>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct KVMMode {
    #[validate(InOrNone(&["Shared", "Private"]))]
    Mode: Option<String>,
    #[validate(RegexOrNone(r"^[a-zA-Z0-9]{1,6}$"))]
    Language: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct VMMConfig {
    Enabled: Option<bool>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    EncryptionEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct VMMControlConfig {
    #[validate(InOrNone(&["Disconnect", "Connect"]))]
    VmmControlType: Option<String>,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct VNCPassword {
        #[validate(Length(0, 8))]
        VNCPassword: String,
        #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
        ReauthKey: Option<String>,
    }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct VNCConfig {
    Enabled: Option<bool>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    #[validate(RangeOrNone(0, 480))]
    SessionTimeoutMinutes: Option<u32>,
    #[validate(InOrNone(&["jp", "en", "de"]))]
    KeyboardLayout: Option<String>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Rule1", "Rule2", "Rule3"]))]
    LoginRule: Option<Vec<String>>,
    SSLEncryptionEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SNMPv1v2 {
    SNMPv1Enabled: Option<bool>,
    SNMPv2Enabled: Option<bool>,
    LongPasswordEnabled: Option<bool>,
    #[validate(LengthOrNone(0, 32))]
    ROCommunityName: Option<String>,
    #[validate(LengthOrNone(0, 32))]
    RWCommunityName: Option<String>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Rule1", "Rule2", "Rule3"]))]
    LoginRule: Option<Vec<String>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SNMPv3 {
    Enabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SNMPConfig {
    Enabled: Option<bool>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<u32>,
    #[validate(LengthOrNone(0, 255))]
    SystemContact: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    SystemLocation: Option<String>,
    #[validate]
    SNMPv1v2: Option<SNMPv1v2>,
    #[validate]
    SNMPv3: Option<SNMPv3>,
  }
}

fn CheckFanSpeedPercent(name: &str, val: &Option<Vec<u32>>) -> Result {
    if let Some(vv) = val {
        for v in vv.iter() {
            Range(name, v, 20, 100)?;
        }
    }
    Ok(())
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SmartCoolingCustomConfig {
    #[validate(RangeOrNone(0, 255))]
    CPUTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(0, 255))]
    OutletTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(0, 255))]
    MemoryTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(1, 65535))]
    NPUHbmTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(0, 255))]
    NPUAiCoreTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(0, 255))]
    NPUBoardTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(0, 255))]
    SoCBoardInletTargetTemperatureCelsius: Option<u32>,
    #[validate(RangeOrNone(0, 255))]
    SoCBoardOutletTargetTemperatureCelsius: Option<u32>,
    #[validate(LengthOrNone(0 , 64), CheckFanSpeedPercent())]
    FanSpeedPercents: Option<Vec<u32>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SmartCooling {
    #[validate(InOrNone(&["EnergySaving", "LowNoise", "HighPerformance", "Custom", "LiquidCooling"]))]
    Mode: Option<String>,
    #[validate]
    CustomConfig: Option<SmartCoolingCustomConfig>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchThermal {
    #[validate]
    SmartCooling: SmartCooling,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct ChassisThermal {
      #[validate(In(&["HighPerformance", "LowNoise", "EnergySaving"]))]
      FanSmartCoolingMode: String,
    }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PreferredBootMedium {
    #[validate(LengthOrNone(6, 6), UniqueOrNone(), AllInOrNone(&["None", "Pxe", "Floppy", "Cd", "Hdd", "BiosSetup"]))]
    AllowableValues: Option<Vec<String>>,
    #[validate(InOrNone(&["Tftp", "Https", "NvmeM2", "Emmc", "Sata", "Usb"]))]
    AllowableValuesExt: Option<String>,
    #[validate(InOrNone(&["None", "Pxe", "Floppy", "Cd", "Hdd", "BiosSetup"]))]
    Target: Option<String>,
    #[validate(InOrNone(&["None", "Tftp", "Https", "NvmeM2", "Emmc", "Sata", "Usb"]))]
    TargetExt: Option<String>,
    #[validate(InOrNone(&["Disabled", "Once", "Continuous"]))]
    EffectivePeriod: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct BootOptions {
    #[validate(InOrNone(&["On", "Off"]))]
    BootModeConfigOverIpmiEnabled: Option<String>,

    #[validate(InOrNone(&["Legacy", "UEFI"]))]
    BootSourceOverrideMode: Option<String>,

    #[validate]
    PreferredBootMedium: Option<PreferredBootMedium>,

    #[validate(LengthOrNone(4, 4), UniqueOrNone(), AllInOrNone(&["HardDiskDrive", "DVDROMDrive", "PXE", "Others"]))]
    BootOrder: Option<Vec<String>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct CPUAdjust {
    Pstate: Option<u8>,
    Tstate: Option<u8>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct FusionPatition {
    #[validate(InOrNone(&["SingleSystem", "DualSystem"]))]
    FusionPartition: Option<String>,
    #[validate(CheckUserNameOrNone())]
    RemoteNodeUserName: Option<String>,
    #[validate(LengthOrNone(8, 20))]
    RemoteNodePassword: Option<String>,
    VGAUSBDVDEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct GetEventParam {
    Skip: i32,

    #[validate(Range(0, 1000))]
    Top: i32,

    #[validate(LengthOrNone(1, 4), UniqueOrNone(), AllInOrNone(&["Informational", "Minor", "Major", "Critical"]))]
    Level: Option<Vec<String>>,
    #[validate(LengthOrNone(1, 128), UniqueOrNone())]
    Subject: Option<Vec<String>>,

    #[validate(DateTimeOrNone("%Y-%m-%d %H:%M:%S"))]
    BeginTime: Option<String>,

    #[validate(DateTimeOrNone("%Y-%m-%d %H:%M:%S"))]
    EndTime: Option<String>,

    #[validate(LengthOrNone(1, 21))]
    SearchString: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SyslogServer {
    #[validate(Range(0, 3))]
    ID: u32,
    #[validate(LengthOrNone(0, 255), CheckServerAddressOrNone())]
    IP: Option<String>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<i32>,

    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["OperationLog", "SecurityLog", "EventLog"]))]
    LogType: Option<Vec<String>>,

    Enabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SyslogNotification {
    EnableState: Option<bool>,

    #[validate(InOrNone(&["BoardSN", "ProductAssetTag", "HostName"]))]
    ServerIdentity: Option<String>,

    #[validate(InOrNone(&["Custom", "RFC3164"]))]
    MessageFormat: Option<String>,

    #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical", "None"]))]
    AlarmSeverity: Option<String>,

    #[validate(InOrNone(&["TCP", "UDP", "TLS"]))]
    TransmissionProtocol: Option<String>,

    #[validate(InOrNone(&["OneWay", "TwoWay"]))]
    AuthenticateMode: Option<String>,

    #[validate]
    SyslogServerList: Option<Vec<SyslogServer>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct EmailInfo {
    AnonymousLoginEnabled: Option<bool>,   
    #[validate(LengthOrNone(1, 64), RegexOrNone(ASSER_INFO_ALL_SPACE))]
    SenderUserName: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    SenderAddress: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    EmailSubject: Option<String>,
    #[validate(LengthOrNone(1, 50))]
    SenderPassword: Option<String>,

    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["HostName", "BoardSN", "ProductAssetTag"]))]
    EmailSubjectContains: Option<Vec<String>>,

    #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical", "None"]))]
    AlarmSeverity: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct RecipientList {
    ID: i32,
    Enabled: Option<bool>,
    #[validate(LengthOrNone(0, 255))]
    EmailAddress: Option<String>,
    #[validate(LengthOrNone(0, 255))]
    Description: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct EmailNotification {
    Enabled: Option<bool>,
    #[validate(LengthOrNone(0, 255), CheckServerAddressOrNone())]
    ServerAddress: Option<String>,
    #[validate(RangeOrNone(1, 65535))]
    ServerPort: Option<i32>,
    TLSEnabled: Option<bool>,
    CertVerificationEnabled: Option<bool>,

    #[validate]
    EmailInfo: Option<EmailInfo>,

    #[validate]
    RecipientList: Option<Vec<RecipientList>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct TrapServerList {
    ID: i32,
    Enabled: Option<bool>,
    #[validate(LengthOrNone(0, 255), CheckServerAddressOrNone())]
    IPAddress: Option<String>,
    #[validate(RangeOrNone(1, 65535))]
    Port: Option<i32>,
    BobEnabled: Option<bool>,
    #[validate(InOrNone(&[",", ";", "/", "-"]))]
    MessageDelimiter: Option<String>,
    #[validate(LengthOrNone(0, 5), UniqueOrNone(),
      AllInOrNone(&["Time", "Severity", "SensorName", "EventCode", "EventDescription"]))]
    MessageContent: Option<Vec<String>>,
    DisplayKeywords: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct TrapNotification {
    Enabled: Option<bool>,
    #[validate(InOrNone(&["SNMPv1", "SNMPv2c", "SNMPv3"]))]
    Version: Option<String>,
    #[validate(InOrNone(&["EventCode", "OID", "PreciseAlarm"]))]
    Mode: Option<String>,
    #[validate(InOrNone(&["BoardSN", "ProductAssetTag", "HostName"]))]
    ServerIdentity: Option<String>,
    #[validate(LengthOrNone(1, 16))]
    SNMPv3User: Option<String>,
    #[validate(LengthOrNone(1, 32))]
    CommunityName: Option<String>,
    #[validate(InOrNone(&["None", "Normal", "Minor", "Major", "Critical"]))]
    AlarmSeverity: Option<String>,

    #[validate]
    TrapServerList: Option<Vec<TrapServerList>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LogServerID {
    ID: i32,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct ManagementNetworkPort {
    #[validate(InOrNone(&["LOM", "Dedicated", "ExternalPCIe", "Aggregation", "OCP", "OCP2", "FlexIO"]))]
    Type: Option<String>,
    #[validate(RangeOrNone(0, 255))]
    PortNumber: Option<u32>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct BasePort {
    #[validate(InOrNone(&["Connected", "Disconnected"]))]
    LinkStatus: Option<String>,
    #[validate(RangeOrNone(0, 255))]
    PortNumber: Option<u32>,
    AdaptiveFlag: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct Members {
    #[validate]
    LOM: Option<Vec<BasePort>>,
    #[validate]
    Dedicated: Option<Vec<BasePort>>,
    #[validate]
    Aggregation: Option<Vec<BasePort>>,
    #[validate]
    ExternalPCIe: Option<Vec<BasePort>>,
    #[validate]
    OCP: Option<Vec<BasePort>>,
    #[validate]
    OCP2: Option<Vec<BasePort>>,
    #[validate]
    FlexIO: Option<Vec<BasePort>>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct NetworkPort {
    #[validate(InOrNone(&["Fixed", "Automatic"]))]
    Mode: Option<String>,
    #[validate(InOrNone(&["Manual Switch Mode", "Auto Failover Mode"]))]
    NcsiMode: Option<String>,
    #[validate]
    ManagementNetworkPort: Option<ManagementNetworkPort>,
    #[validate]
    Members: Option<Members>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct IPv4Config {
    #[validate(InOrNone(&["DHCP", "Static"]))]
    IPAddressMode: Option<String>,
    #[validate(CheckIpv4OrNone())]
    IPAddress: Option<String>,
    #[validate(LengthOrNone(1, 15))]
    SubnetMask: Option<String>,
    #[validate(CheckIpv4OrNone())]
    Gateway: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct IPv6Config {
    #[validate(InOrNone(&["DHCPv6", "Static"]))]
    IPAddressMode: Option<String>,
    #[validate(CheckIpv6OrNone())]
    IPAddress: Option<String>,
    #[validate(RangeOrNone(0, 128))]
    PrefixLength: Option<u32>,
    #[validate(CheckIpv6OrNone())]
    Gateway: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct NetworkProtocols {
    #[validate(InOrNone(&["IPv4", "IPv6", "IPv4AndIPv6"]))]
    ProtocolMode: Option<String>,
    #[validate]
    IPv6Config: Option<IPv6Config>,
    #[validate]
    IPv4Config: Option<IPv4Config>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct DNS {
    #[validate(InOrNone(&["IPv4", "IPv6", "Static"]))]
    AddressMode: Option<String>,
    #[validate(LengthOrNone(0, 67), CheckDNSAddressOrNone())]
    Domain: Option<String>,
    #[validate(LengthOrNone(0, 65), CheckServerAddressOrNone())]
    PrimaryServer: Option<String>,
    #[validate(LengthOrNone(0, 65), CheckServerAddressOrNone())]
    BackupServer: Option<String>,
    #[validate(LengthOrNone(0, 65), CheckServerAddressOrNone())]
    TertiaryServer: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct VLAN {
    Enabled: Option<bool>,
    #[validate(RangeOrNone(1, 4094))]
    ID: Option<u16>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LLDP {
    Enabled: Option<bool>,
    #[validate(InOrNone(&["Tx", "Rx", "TxRx", "Disabled"]))]
    WorkMode: Option<String>,
    #[validate(RangeOrNone(1, 8192))]
    TxDelaySeconds: Option<u32>,
    #[validate(RangeOrNone(5, 32768))]
    TxIntervalSeconds: Option<u32>,
    #[validate(RangeOrNone(2, 10))]
    TxHold: Option<u32>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct NetworkSettings {
    #[validate(LengthOrNone(1, 64))]
    HostName: Option<String>,
    #[validate]
    NetworkPort: Option<NetworkPort>,
    #[validate]
    NetworkProtocols: Option<NetworkProtocols>,
    #[validate]
    DNS: Option<DNS>,
    #[validate]
    VLAN: Option<VLAN>,
    #[validate]
    LLDP: Option<LLDP>,
  }
}

pub fn CheckNtpServerAddressOrNone<T>(name: &'static str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    None => Ok(()),
    Some(v) if v.len() == 0 => Ok(()),
    _ => CheckServerAddressOrNone(name, val),
  }
}

pub fn CheckNtpServerAddressListOrNone<T>(name: &'static str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    None => Ok(()),
    Some(v) => {
      for vv in v.split(",") {
        CheckNtpServerAddressOrNone(name, &Some(vv.to_owned()))?;
      }
      Ok(())
    }
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct NTP {
    Enabled: Option<bool>,
    #[validate(LengthOrNone(1, 255))]
    DateTimeLocalOffset: Option<String>,
    #[validate(LengthOrNone(1, 255))]
    DateTime: Option<String>,
    #[validate(InOrNone(&["IPv4", "IPv6", "Static"]))]
    AddressMode: Option<String>,
    #[validate(LengthOrNone(0, 67), CheckNtpServerAddressOrNone())]
    PreferredServer: Option<String>,
    #[validate(LengthOrNone(0, 67), CheckNtpServerAddressOrNone())]
    AlternateServer: Option<String>,
    #[validate(LengthOrNone(0, 271), CheckNtpServerAddressListOrNone())]
    ExtraServer: Option<String>,
    #[validate(RangeOrNone(3, 17))]
    MinPollingInterval: Option<u32>,
    #[validate(RangeOrNone(3, 17))]
    MaxPollingInterval: Option<u32>,
    AuthEnabled: Option<bool>,
    DstEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct USBStickControlAction {
    #[validate(InOrNone(&["Connect", "Disconnect"]))]
    Type: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct AdvancedSecurity {
    SystemLockDownEnabled: Option<bool>,
    OSUserManagementEnabled: Option<bool>,
    PasswordComplexityCheckEnabled: Option<bool>,
    AntiDNSRebindEnabled: Option<bool>,

    #[validate(InOrNone(&["Password", "PublicKey"]))]
    SSHAuthenticationMode: Option<String>,

    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["TLS1.0", "TLS1.1", "TLS1.2", "TLS1.3"]))]
    TLSVersion: Option<Vec<String>>,

    #[validate(RangeOrNone(0, 365))]
    PasswordValidityDays: Option<u32>,

    #[validate(RangeOrNone(8, 20))]
    MinimumPasswordLength: Option<u32>,

    #[validate(RangeOrNone(0, 365))]
    MinimumPasswordAgeDays: Option<u32>,

    #[validate(RangesOrNone(&[(0, 0), (30, 365)]))]
    AccountInactiveTimelimitDays: Option<u32>,

    #[validate(LengthOrNone(0, 16))]
    EmergencyLoginUser: Option<String>,

    #[validate(RangeOrNone(0, 5))]
    PreviousPasswordsDisallowedCount: Option<u32>,

    #[validate(RangeOrNone(0, 6))]
    AccountLockoutThreshold: Option<u32>,

    #[validate(RangeOrNone(1, 30))]
    AccountLockoutDuration: Option<u32>,

    #[validate(RangeOrNone(7, 180))]
    CertificateOverdueWarningTime: Option<u32>,
  }
}

// 时间段： 支持三种格式，YYYY-MM-DD HH:MM ， YYYY-MM-DD 和 HH:MM；
// 起始时间和结束时间的格式必须保持一致。起始年份和结束年份只能在1970到2050之间。
fn CheckLoginRuleTime(name: &'static str, val: &Option<String>, minDay: i32, maxDay: i32) -> Result {
  match val {
    Some(ref v) => {
      let r = match v.len() {
        16 => NaiveDateTime::parse_from_str(v, "%Y-%m-%d %H:%M").map_or(false, |dt| {
          let d = dt.date();
          d.year() >= minDay && d.year() <= maxDay
        }),
        10 => NaiveDate::parse_from_str(v, "%Y-%m-%d").map_or(false, |d| d.year() >= minDay && d.year() <= maxDay),
        5 => NaiveTime::parse_from_str(v, "%H:%M").map_or(false, |_| true),
        0 => true,
        _ => false,
      };
      if r {
        Ok(())
      } else {
        Err(ValidatorErr::new_ensure("CheckLoginRuleTime", name))
      }
    }
    None => Ok(()),
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LoginRule {
    #[validate(Range(0, 2))]
    ID: u8,

    #[validate(CheckLoginRuleTime(1970, 2050))]
    StartTime: Option<String>,

    #[validate(CheckLoginRuleTime(1970, 2050))]
    EndTime: Option<String>,

    // IP段： 支持两种格式，xxx.xxx.xxx.xxx 和 xxx.xxx.xxx.xxx/mask；
    // xxx.xxx.xxx.xxx是指单个完整的IP地址，而xxx.xxx.xxx.xxx/mask是指一个IP段，mask取值范围为1~32。
    #[validate(CheckRegexOrEmpty(&[
      r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$",
      r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/\d{1,2}$"]
    ))]
    IP: Option<String>,

    // MAC段： 支持两种格式，xx:xx:xx 和 xx:xx:xx:xx:xx:xx；
    // xx:xx:xx只能是mac地址的前三段，而xx:xx:xx:xx:xx:xx是指单个完整的MAC地址。
    #[validate(CheckRegexOrEmpty(&[
      r"^[\dA-Fa-f]{2}:[\dA-Fa-f]{2}:[\dA-Fa-f]{2}$",
      r"^[\dA-Fa-f]{2}:[\dA-Fa-f]{2}:[\dA-Fa-f]{2}:[\dA-Fa-f]{2}:[\dA-Fa-f]{2}:[\dA-Fa-f]{2}$"
    ]))]
    Mac: Option<String>,

    Status: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct TsbInfo {
    ReauthKey: Option<String>,
    MeasureEnable: Option<bool>,
    ControlAction: Option<bool>,
    TrustedCenterServerAddr: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct DigestInfo {
    ReauthKey: Option<String>,
    #[validate(RegexSetOrNone(&[r"^[0-9a-fA-F]{64}$"]))]
    BiosStandardDigest: Option<String>,
    #[validate(RegexSetOrNone(&[r"^[0-9a-fA-F]{64}$"]))]
    ShimStandardDigest: Option<String>,
    #[validate(RegexSetOrNone(&[r"^[0-9a-fA-F]{64}$"]))]
    GrubStandardDigest: Option<String>,
    #[validate(RegexSetOrNone(&[r"^[0-9a-fA-F]{64}$"]))]
    GrubCfgStandardDigest: Option<String>,
    #[validate(RegexSetOrNone(&[r"^[0-9a-fA-F]{64}$"]))]
    KernelStandardDigest: Option<String>,
    #[validate(RegexSetOrNone(&[r"^[0-9a-fA-F]{64}$"]))]
    InitrdStandardDigest: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct RightManagement {
    ID: u8,
    UserConfigEnabled: bool,
    BasicConfigEnabled: bool,
    RemoteControlEnabled: Option<bool>,
    VMMEnabled: Option<bool>,
    SecurityConfigEnabled: bool,
    PowerControlEnabled: bool,
    DiagnosisEnabled: Option<bool>,
    ConfigureSelfEnabled: bool,
    #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
    ReauthKey: Option<String>,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct SubjectInfo {
        #[validate(Length(2, 2), Regex(r"^[A-Za-z]+$"))]
        Country: String,
        #[validate(RegexSetOrNone(&[r"^$", r"^[0-9a-zA-Z_ \-.]{0,128}$"]))]
        State: Option<String>,
        #[validate(RegexSetOrNone(&[r"^$", r"^[0-9a-zA-Z_ \-.]{0,128}$"]))]
        Location: Option<String>,
        #[validate(RegexSetOrNone(&[r"^$", r"^[0-9a-zA-Z_ \-.]{0,64}$"]))]
        OrgName: Option<String>,
        #[validate(RegexSetOrNone(&[r"^$", r"^[0-9a-zA-Z_ \-.]{0,64}$"]))]
        OrgUnit: Option<String>,
        #[validate(Length(1, 64), Regex(r"^[0-9a-zA-Z_ \-.]+$"))]
        CommonName: String,
        #[validate(RegexSetOrNone(&[r"^$", r"^[0-9a-zA-Z_ \-.]{0,64}$"]))]
        InternalName: Option<String>,
        #[validate(LengthOrNone(0, 255), RegexSetOrNone(&[r"^$", r"^[a-zA-Z0-9][a-zA-Z0-9_\-.]{0,63}@[a-zA-Z0-9_\-.]{1,254}\.[a-zA-Z0-9]{2,64}$"]))]
        Email: Option<String>,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct CMPConfig {
        #[validate(LengthOrNone(1, 67), CheckServerAddressOrNone())]
        CAServerAddr: Option<String>,
        #[validate(RangeOrNone(1, 65535))]
        CAServerPort: Option<u32>,
        #[validate(RegexOrNone(r"^/[A-Za-z0-9_/]{0,127}$"))]
        CAServerCMPPath: Option<String>,
        #[validate(InOrNone(&["two-way"]))]
        TLSAuthType: Option<String>,
        AutoUpdateEnabled: Option<bool>,
        #[validate]
        SubjectInfo: Option<SubjectInfo>,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct CMPConfigPram {
        #[validate]
        CMPConfig: CMPConfig,
    }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SecurityBanner {
    Enabled: Option<bool>,

    #[validate(LengthOrNone(0, 1024))]
    SecurityBanner: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchSystemDiagnostic {
    PCIeInterfaceEnabled: Option<bool>,
    BlackBoxEnabled: Option<bool>,
    SerialPortDataEnabled: Option<bool>,
    VideoRecordingEnabled: Option<bool>,
    ScreenshotEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct ExportVideoStartupFileConfig {
    #[validate(In(&["Java", "HTML5"]))]
    PlayType: String,
    #[validate(In(&["CaterrorVideo", "PoweroffVideo", "OsresetVideo", "LocalVideo"]))]
    VideoType: String,
    #[validate(Length(1, 128), Regex(r"^[\[\]a-zA-Z0-9:._-]+$"))]
    HostName: String,
    #[validate(Length(1, 64))]
    ServerName: String,
    #[validate(Regex(r"^[a-zA-Z0-9]{1,6}$"))]
    Language: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LogCount {
    #[validate(Range(0, 65535))]
    Skip: i32,
    #[validate(Range(0, 65535))]
    Top: i32,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct WorkRecordConfig {
    #[validate(Length(1, 255))]
    Content: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchWorkRecord {
    #[validate(Length(1, 255))]
    ID: String,
    #[validate(Length(1, 255))]
    Content: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct DownloadVideoType {
    #[validate(In(&["CaterrorVideo", "PoweroffVideo", "OsresetVideo"]))]
    VideoType: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct ReauthKey {
    #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
    ReauthKey: Option<String>,
  }
}

pub fn CheckUserName(name: &'static str, val: &String) -> Result {
  Regex(name, val, r"^[^:<>&,'/\\% #+\-][^:<>&,'/\\% ]{0,254}$")
}

/*
 * Description : 登录对用户名的宽松校验，去除所有ascii码中的控制字符，长度至少为1
 */
pub fn CheckUserNameLogin(name: &'static str, val: &String) -> Result {
  Regex(name, val, r#"^[^\x00-\x1F\x7F]+$"#)
}

pub fn CheckUserNameOrNone(name: &'static str, val: &Option<String>) -> Result {
  match val {
    None => Ok(()),
    Some(v) => CheckUserName(name, v),
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct AccountConfig {
    #[validate(RangeOrNone(2,17))]
    ID: Option<u32>,
    #[validate(CheckUserName())]
    UserName: String,
    #[validate(Length(1, 255))]
    Password: String,
    #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
    ReauthKey: Option<String>,
    #[validate(In(&["Administrator","Operator","Commonuser","CustomRole1","CustomRole2","CustomRole3","CustomRole4","Noaccess"]))]
    RoleID: String,
    #[validate(LengthOrNone(0, 7), UniqueOrNone(), AllInOrNone(&["Redfish","Web","SFTP","Local","IPMI","SSH","SNMP"]))]
    LoginInterface: Option<Vec<String>>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Rule1","Rule2","Rule3"]))]
    LoginRule: Option<Vec<String>>,
    #[validate(RangeOrNone(1, 2))]
    FirstLoginPolicy: Option<u8>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchAccount {
    Enabled: Option<bool>,
    #[validate(LengthOrNone(1, 16), CheckUserNameOrNone())]
    UserName: Option<String>,
    #[validate(LengthOrNone(1, 255))]
    Password: Option<String>,
    #[validate(LengthOrNone(1, 128), CheckBase64OrNone())]
    ReauthKey: Option<String>,
    #[validate(InOrNone(&["Administrator","Operator","Commonuser","CustomRole1","CustomRole2","CustomRole3","CustomRole4","Noaccess"]))]
    RoleID: Option<String>,
    InsecurePromptEnabled: Option<bool>,
    #[validate(LengthOrNone(0, 7), UniqueOrNone(), AllInOrNone(&["Redfish","Web","SFTP","Local","IPMI","SSH","SNMP"]))]
    LoginInterface: Option<Vec<String>>,
    #[validate(LengthOrNone(0, 3), UniqueOrNone(), AllInOrNone(&["Rule1","Rule2","Rule3"]))]
    LoginRule: Option<Vec<String>>,
    #[validate(LengthOrNone(1, 20))]
    SNMPV3PrivPasswd: Option<String>,
    #[validate(InOrNone(&["MD5", "SHA", "SHA1", "SHA256", "SHA384", "SHA512"]))]
    SnmpV3AuthProtocol: Option<String>,
    #[validate(InOrNone(&["DES", "AES", "AES256"]))]
    SnmpV3PrivProtocol: Option<String>,
    #[validate(RangeOrNone(1, 2))]
    FirstLoginPolicy: Option<u8>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct SPConfig {
    SPDeviceInfoCollectEnabled: Option<bool>,
    SPStartEnabled: Option<bool>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct USBMgmtService {
    ServiceEnabled: bool,
    #[validate(LengthOrNone(1, 32))]
    USBUncompressPassword: Option<String>,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct PatchNetworkAdapter {
    HotPlugAttention: bool,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct LanguageSetStr {
    #[validate(Length(1, 256), Regex(r"^[[:alnum:],]+$"))]
    LanguageSetStr: String,
  }
}

define_param! {
  #[portal(ToLua, validate)]
  #[serde(deny_unknown_fields)]
  #[derive(Deserialize)]
  pub struct NpuLogPram {
    #[validate(Range(1, 8))]
    NpuId: i32,
  }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct PowerMode {
        SleepModeEnabled: Option<bool>,
        #[validate(InOrNone(&["N+1", "N+N"]))]
        SleepMode: Option<String>,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct BladePower {
        #[validate(Length(5, 6))]
        Id: String,
        LimitInWatts: Option<u16>,
        #[validate(InOrNone(&["HardPowerOff", "NoAction"]))]
        LimitException: Option<String>,
    }
}


define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct ChassisPowerLimit {
        PowerLimitEnabled: Option<bool>,
        LimitInWatts: Option<u16>,
        #[validate(RangeOrNone(0, 70))]
        PowerLimitThreshold: Option<u8>,
        #[validate(InOrNone(&["Equal", "Proportion", "Manual"]))]
        PowerLimitMode: Option<String>,
        #[validate]
        PowerLimitBladeInfo: Option<Vec<BladePower>>,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct EventSeverity {
        #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical"]))]
        PSU: Option<String>,
        #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical"]))]
        Fan: Option<String>,
        #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical"]))]
        Blade: Option<String>,
        #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical"]))]
        Swi: Option<String>,
        #[validate(InOrNone(&["Normal", "Minor", "Major", "Critical"]))]
        SMM: Option<String>,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct SwitchProfile {
        #[validate(Range(1, 4))]
        SlotId: u8,
        #[validate(Range(1, 2))]
        FruId: u8,
        #[validate(Length(7, 10))]
        BackupTime: String,
        #[validate(Length(5, 6))]
        SwitchModel: String,
    }
}

define_param! {
    #[portal(ToLua, validate)]
    #[serde(deny_unknown_fields)]
    #[derive(Deserialize)]
    pub struct SlotIdInfo {
      #[validate(Range(1, 8))]
      SlotId: u8,
    }
}