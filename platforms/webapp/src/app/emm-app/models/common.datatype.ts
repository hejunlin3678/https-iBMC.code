
export enum SysId {
    PowerInfo = 'leftSysPower',
    Fans = 'leftSysFans',
    BmcUser = 'leftSysBmcUser',
    BmcNet = 'leftSysBmcNet',
    PowerOnOff = 'leftSysPowerOnOff',
    RestSwi = 'leftSysRestSwi'
}

export enum MainId {
    Event = 'leftMainEvent',
    Report = 'leftMainReport',
    Config = 'leftMainConfig',
    IbmcLog = 'leftMainIbmcLog',
    Note = 'leftMainNote',
}

export enum UserId {
    LocalUsers = 'leftUserLocalUsers',
    Ldap = 'leftUserLdap',
    TwoFactor = 'leftUserTwoFactor',
    Online = 'leftUserOnline',
    Security = 'leftUserSecurity',
}

export enum ServiceId {
    Port = 'leftPortService',
    Web = 'leftWebService',
    Snmp = 'leftSnmpService',
}

export enum ManageId {
    Network = 'leftNetworkConfig',
    Ntp = 'leftNtpConfig',
    Upgrade = 'leftUpgradeConfig',
    Switch = 'switch',
    Language = 'language'
}

export enum ProductName {
    Tce4U = 'TCE8040',
    Tce8U = 'TCE8080',
    Emm12U = 'E9000',
}

export enum PowerState {
    On = 'On',
    Off = 'Off',
    NotConnected = 'notConnected',
    VersionLow = 'versionLow',
}

export enum Health {
    Ok = 'OK',
    Warning = 'Warning',
    Critical = 'Critical',
}

export enum StateImgs {
    // 健康 图标
    okImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/u642.png',
    // 警告 图标
    warnImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/emergency.png',
    // 爆红 图标
    noImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/u638.png',

    // 问号 图标 powerState==='notConnected'
    unKnownImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/icon_19.png',
    // 有个帽子的图标 powerState==='Off'
    offImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/off.png',
    // 电源 图标 powerState==='On'
    onImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/on.png',
    // 感叹号 图标 powerState==='versionLow'
    verlowImgPath = 'assets/emm-assets/image/home/homeFrontAndRearViewImgs/verlow.png',

}
