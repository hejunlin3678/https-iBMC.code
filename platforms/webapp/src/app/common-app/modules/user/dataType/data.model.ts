// 二次密码认证的错误ID
export enum SecondAuthorityFail {
    EMPTY = '',
    AUTHORIZATIONFAILED = 'ReauthFailed'
}

// 本地用户密码和snmpv3密码匹配字符。
export enum PasswordFailedProp {
    PASSWORD = 'Password',
    SNMPV3PASSWORD = 'SnmpV3PrivPasswd',
    PASSWORDPROP = 'password',
    SNMPV3PASSWORDPROP = 'snmp privacy password'
}

// KRB特定错误ID列表
export enum KrbErrorIds {
    KRBKeytabUploadFail = 'KRBKeytabUploadFail'
}

// Kerberos组的特定角色
export enum UserRoleList {
    NOACCESS = 'No Access',
    COMMONUSER = 'Common User',
}

// 按钮禁用和启用的类别
export enum Operators {
    DISABLE = 'disable',
    ENABLE = 'enable',
    ADD = 'add',
    EDIT = 'edit',
    DELETE = 'delete',
    DELETESSHKEY = 'deleteSSHKey'
}

// 不能被删除的用户
export enum UnDeleteUsers {
    SSHLOGIN = 'SSHLogin',
    TRAPV3USER = 'TrapV3User',
    EMERGENCYUSER = 'EmergencyUser',
    LASTADMIN = 'USER_UNDELETED_LAST'
}

// 用户的角色列表
export enum USER_ROLES {
    ADMINISTRATOR = 'Administrator',
    OPERATOR = 'Operator',
    COMMONUSER = 'Commonuser',
    NOACCESS = 'Noaccess',
    CUSTOMROLE1 = 'CustomRole1',
    CUSTOMROLE2 = 'CustomRole2',
    CUSTOMROLE3 = 'CustomRole3',
    CUSTOMROLE4 = 'CustomRole4',
}

// 半屏弹窗的标题
export enum HalfComponentTitle {
    EDIT = 'USER_TEMPLATE_TITLE_EDIT',
    ADD = 'USER_TEMPLATE_TITLE_ADD'
}

// 用户的接口操作
export enum SendMethodName {
    DELETE = 'deleteUser',
    MODIFY = 'modifyUserState',
    SSHDELETE = 'deleteSSHKey',
    NORMAL = ''
}

// 表单的禁用和启用操作
export enum FormControlOperators {
    ENABLE = 'enable',
    DISABLE = 'disable'
}

// SNMPV3密码是否是独立的密码提示信息
export enum SnmpV3Tips {
    SAFE = 'SNMP_PWD_INIT_TIP_2',
    NOTSAFE = 'SNMP_PWD_INIT_TIP_1'
}

// 登录接口
export enum LoginInterfaces {
    SSH = 'SSH',
    SNMP = 'SNMP',
    IPMI = 'IPMI',
    Web = 'Web',
    SFTP = 'SFTP',
    Local = 'Local',
    Redfish = 'Redfish'
}

// 密码校验规则操作类型
export enum PwdValidatorOperators {
    ADD = 'add',
    DELETE = 'delete'
}

// 涉及保存时需要下发多个接口的操作结果，成功，失败，部分失败
export enum OperatorResult {
    SUCCESS = 'success',
    ERROR = 'error',
    SOMEFAILED = 'someFailed'
}

export enum FileAddFaild {
    MAXSIZE = 'maxSize',
    TYPE = 'type'
}

