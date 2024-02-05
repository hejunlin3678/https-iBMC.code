export interface ITableRow {
    id: number;
    isOperation: boolean;
    roleName: string;
    userConfigEnabled: boolean;
    basicConfigEnabled: boolean;
    remoteControlEnabled: boolean;
    VMMEnabled: boolean;
    securityConfigEnabled: boolean;
    powerControlEnabled: boolean;
    diagnosisEnabled: boolean;
    queryEnabled: boolean;
    configureSelfEnabled: boolean;
    isSave: boolean;
    isEdit: boolean;
    sequence: number;
}

export interface IUpdateParam {
    ID: number;
    UserConfigEnabled: boolean;
    BasicConfigEnabled: boolean;
    RemoteControlEnabled: boolean;
    VMMEnabled: boolean;
    SecurityConfigEnabled: boolean;
    PowerControlEnabled: boolean;
    DiagnosisEnabled: boolean;
    ConfigureSelfEnabled: boolean;
    ReauthKey: string;
}

export enum RoleName {
    administrator = 'Administrator',
    operator = 'Operator',
    commonuser = 'Commonuser',
    noAccess = 'NoAccess',
    customRole1 = 'CustomRole1',
    customRole2 = 'CustomRole2',
    customRole3 = 'CustomRole3',
    customRole4 = 'CustomRole4',
}
