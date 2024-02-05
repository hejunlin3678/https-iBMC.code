export class LoginType {
    public readonly undetermined = -1;
    public readonly common = 0;
    public readonly twofactor = 1;
    public readonly sso = 2;
}

export interface IRustPrivil {
    ID?: number;
    Name?: string;
    BasicConfigEnabled?: boolean;
    ConfigureSelfEnabled?: boolean;
    DiagnosisEnabled?: boolean;
    PowerControlEnabled?: boolean;
    QueryEnabled?: boolean;
    RemoteControlEnabled?: boolean;
    SecurityConfigEnabled?: boolean;
    UserConfigEnabled?: boolean;
    VMMEnabled?: boolean;
}
