export enum SysId {
    Info = 'leftSysInfo',
    Monitor = 'leftSysMonitor',
    Storage = 'leftSysStorage',
    Power = 'leftSysPower',
    Fans = 'leftSysFans',
    Bios = 'leftSysBios',
    Partity = 'leftSysPartity',
}

export enum MainId {
    Event = 'leftMainEvent',
    Report = 'leftMainReport',
    Fdm = 'leftMainFdm',
    Video = 'leftMainVideo',
    SysLog = 'leftMainSysLog',
    IbmcLog = 'leftMainIbmcLog',
    Note = 'leftMainNote',
}

export enum UserId {
    LocalUsers = 'leftUserLocalUsers',
    Ldap = 'leftUserLdap',
    Kerberos = 'leftUserKerberos',
    TwoFactor = 'leftUserTwoFactor',
    Online = 'leftUserOnline',
    Security = 'leftUserSecurity',
}

export enum ServiceId {
    Port = 'leftPortService',
    Web = 'leftWebService',
    Kvm = 'leftKvmService',
    Vmm = 'leftVmmService',
    Vnc = 'leftVncService',
    Snmp = 'leftSnmpService',
}

export enum ManageId {
    Network = 'leftNetworkConfig',
    Ntp = 'leftNtpConfig',
    Upgrade = 'leftUpgradeConfig',
    Update = 'leftUpdateConfig',
    Language = 'leftLanguageConfig',
    License = 'leftLicenseConfig',
    Ibma = 'leftIbmaConfig',
    Sp = 'leftSpConfig',
    USB = 'leftUSBConfig',
}

export enum OceanType {
    // 大西洋控制器
    Atlantic = 'PangeaV6_Atlantic',
    // 北冰洋
    Arctic = 'PangeaV6_Arctic',
    // 太平洋
    Pacific = 'PangeaV6_Pacific',
    // 大西洋直通板
    AtlanticSMM = 'PangeaV6_Atlantic_Smm',
}
