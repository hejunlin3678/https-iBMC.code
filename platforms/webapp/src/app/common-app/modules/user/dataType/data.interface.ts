// 登录规则数据结构
export interface ILoginRule {
    id: number;
    memberId?: string;
    active: boolean;
    checked: boolean;
    ruleEnabled: boolean;
    startTime: string;
    endTime: string;
    ip: string;
    mac: string;
}

// Kerberos数据结构
export interface IKerberos {
    KerberosEnabled: boolean;
}

// 单个controller的结构
export interface IGroup {
    id: string;
    kerberosPort: string;
    kerberosServerAddress: string;
    name: string;
    // 领域
    realm: string;
    uploadUrl: string;
    kerberosGroups: IGroupItem[];
}

// 单个用户组结构
export interface IGroupItem {
    groupName: string;
    groupSID: string;
    groupRole: string;
    groupLoginInterface: string[];
    groupLoginRule: string[];
    groupDomain: string;
    memberId: number;
}

// Kerberos保存数据的结构类型
export interface IKrbSave {
    Domain?: string;
    IPAddress?: string;
    Port?: number;
    ReauthKey?: string;
}

// kerberos保存时的下发参数结构
export interface IKerberosSaveData {
    LoginRule?: string[];
    LoginInterface?: string[];
    Name?: string;
    Domain?: string;
    SID?: string;
    RoleID?: string;
}

// 本地用户信息数据结构
export interface IUsersInfo {
    id: number;
    userName: string;
    roleId: string;
    loginInterface: string[];
    loginRole: string[];
    validate: number;
    canDeleted: boolean;
    canDisabled: boolean;
    canDisbledTipSHow: boolean;
    unDeletedReason: string;
    unDisabledReason: string;
    state: boolean;
    sshPublicKeyHash: string;
    deleteableTip?: string;
}


// SSH证书信息
export interface IUerCert {
    FingerPrint: string;
    IssueBy: string;
    IssueTo: string;
    PublicKeyLengthBits: number;
    RevokedDate: string | null;
    RevokedState: boolean;
    RootCertUploadedState: boolean;
    SerialNumber: string;
    ValidFrom: string;
    ValidTo: string;
}

// 后端返回的用户列表的数据结构
export interface IResponseUserData {
    ID: number;
    UserName: string;
    DelDisableReason: string;
    Deleteable: boolean;
    Enabled: boolean;
    InsecurePromptEnabled: number;
    LoginInterface: string[];
    LoginRule: string[];
    MutualAuthClientCert: IUerCert;
    PasswordValidityDays: number;
    RoleID: string;
    SNMPEncryptPwdInit: boolean;
    SSHPublicKeyHash: string | null;
    SnmpV3AuthProtocol: string;
    SnmpV3PrivProtocol: string;
    FirstLoginPolicy?: number;
}

// 剩余可新增用户数据结构
export interface IUserId {
    label: string;
    id: number;
    roleId?: string;
}

// 角色项数据结构
export interface IRole {
    id: number;
    label: string;
}

// 鉴权算法和加密算法结构
export interface IAlgorithm {
    id: string;
    label: string;
    disabled?: boolean;
}

// 特定用户信息数据结构
export interface IUserInfo {
    RoleId: string;
    LoginInterface: string[];
    LoginRule: string[];
    UserName: string;
    AuthProtocol: string;
    EncryProtocol: string;
    SNMPEncryptPwdInit: boolean;
    FirstLoginPolicy: number;
}

// 后端错误对象类型
export interface IBackError {
    code: string;
    message?: string;
    relatedProperties?: string[];
}

// 后端错误对象类型（redfish）
export interface IBackRedfishError {
    MessageId: string;
    Message?: string;
    RelatedProperties?: string[];
}

// 自定义错误ID对象
export interface IError {
    errorId: string;
    relatedProp: string;
}
// 自定义错误封装对象
export interface ICustError {
    type: string;
    errors: IError[];
}

// 添加用户时，下发的数据结构
export interface IAddUser {
    Id: number;
    UserName: string;
    Password: string;
    RoleId: string;
    FirstLoginPolicy?: number;
    LoginInterface?: string[];
    LoginRule?: string;
}

// 保存用户时数据结构
export interface ISaveUser {
    Id: number;
    Password?: string;
    RoleId?: string;
    UserName?: string;
    OldUserName: string;
    LoginInterface?: string;
    LoginRule?: string;
    SNMPV3Password?: string;
    SnmpV3AuthProtocol?: string;
    SnmpV3PrivProtocol?: string;
    FirstLoginPolicy?: number;
}

export interface ILdapControllerTab {
    title?: string;
    id?: number;
    active?: boolean;
    onActiveChange: any;
}

export interface ILdapGroup {
    memberId: number;
    groupName: string;
    groupFolder: string;
    groupRole: string;
    groupLoginRule: ILoginRule[];
    groupLoginInterface: string[];
}

export interface ILdapControllerItem {
    ldapServerAddress: string;
    ldapPort: string;
    bindDN: string;
    bindPassword: string;
    domain: string;
    userFolder: string;
    certificateVerificationEnabled: boolean;
    certificateVerificationLevel: string;
    importCert: string;
    importCrl: string;
    deleteCrl: string;
    LdapGroups: ILdapGroup[];
    cerInfo: { server?: any, middle?: any, root?: any };
}

// ldap特定Controlnal下后端Group组结构
export interface ILdapGroupItem {
    Domain: string;
    Folder: string;
    ID: number;
    LoginInterface: string[];
    LoginRule: ILoginRule[];
    Name: string;
    RoleID: string;
}

// 中间证书和服务器证书，证书链结构
export interface ILdapCertSigle {
    type?: string;
    Issuer?: string;
    SerialNumber?: string;
    Subject?: string;
    ValidNotAfter?: string;
    ValidNotBefore?: string;
}

// ldap证书信息后端结构
export interface ILdapCert {
    CRLValidNotAfter: string;
    CRLValidNotBefore: string;
    CrlVerification: boolean;
    IntermediateCert: ILdapCertSigle[];
    ServerCert: ILdapCertSigle;
    RootCert: ILdapCertSigle;
    VerificationEnabled: boolean;
    VerificationLevel: string;
}

// ldap controller信息后端结构
export interface ILdapController {
    BindDN: string;
    Domain: string;
    Folder: string;
    BindPwd?: string;
    IPAddress: string;
    Port: number;
}

// ldap组后端结构
export interface IBLdapGroup {
    Member: ILdapGroupItem[];
}


// 前端LDAPgroupRow
export interface ILdapGroupRow {
    groupFolder: string;
    groupLoginInterface: string[];
    groupLoginRule: string[];
    groupName?: string;
    groupRole: string;
    memberId?: number;
}
