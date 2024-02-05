import { TiValidationConfig } from '@cloud/tiny3';
import { UntypedFormControl } from '@angular/forms';

export interface IFormOption {
    id?: string;
    label?: string;
    value?: any;
    initValue: any;
    show?: boolean;
    associated: IAuthorityEnum;
    controll?: UntypedFormControl;
    change?: (paramas?) => void;
    disabled?: boolean;
}
export interface ISwitchOption extends IFormOption {
    value: boolean;
    initValue: boolean;
    tip?: string;
}
export interface ICheckOption {
    label: string;
    tip?: string;
    options: ICheckboxItem[];
    value: ICheckboxItem[];
    initValue: ICheckboxItem[];
}
export interface ICheckboxItem extends IFormOption {
    id?: TLSVersions;
    text: TLSVersions;
    value: boolean;
    initValue: boolean;
}

export interface IInputOption extends IFormOption {
    controll: UntypedFormControl;
    validation: TiValidationConfig;
    initValue: string | number;
    blur?: () => void;
    clearErrorInfo?: () => void;
    recommendValue?: string;
}
export interface ISelectOption extends IFormOption {
    options: ISelectItem[];
    value: ISelectItem;
    initValue: string;
    tip?: string;
}

export interface ILoginFailSelect extends ISelectOption {
    options: ILoginFailOption[];
    value: ILoginFailOption;
    changeFn: (option: ILoginFailOption) => void;
}

export interface ISelectItem {
    id: string | number;
    label: string;
}

export interface ILoginFailOption extends ISelectItem {
    isIgnoreTime: boolean;
}

export interface IAuthorityData {
    user?: IAccount[];
    systemLockDownEnabled?: boolean;
    OSUserManagementEnabled?: boolean;
    tls12Configurable: boolean;
    passwordComplexityCheckEnabled?: boolean;
    sshPasswordAuthenticationEnabled?: boolean;
    passwordValidityDays?: number;
    emergencyLoginUser?: string;
    previousPasswordsDisallowedCount?: number;
    minimumPasswordAgeDays?: number;
    minimumPasswordLength?: number;
    accountInactiveTimelimit?: number;
    certificateOverdueWarningTime?: number;
    tlsVersion: TLSVersions[];
    accountLockoutThreshold: number;
    accountLockoutDuration: number;
    antiDNSRebindEnabled: boolean;
    error?: any;
}

export enum IAuthorityEnum {
    systemLockDownEnabled = 'SystemLockDownEnabled',
    OSUserManagementEnabled = 'OSUserManagementEnabled',
    passwordComplexityCheckEnabled = 'PasswordComplexityCheckEnabled',
    SSHPasswordAuthenticationEnabled = 'SSHAuthenticationMode',
    passwordValidityDays = 'PasswordValidityDays',
    emergencyLoginUser = 'EmergencyLoginUser',
    previousPasswordsDisallowedCount = 'PreviousPasswordsDisallowedCount',
    minimumPasswordAgeDays = 'MinimumPasswordAgeDays',
    minimumPasswordLength = 'MinimumPasswordLength',
    accountInactiveTimelimit = 'AccountInactiveTimelimitDays',
    certificateOverdueWarningTime = 'CertificateOverdueWarningTime',
    tlsVersion = 'TLSVersion',
    accountLockoutThreshold = 'AccountLockoutThreshold',
    accountLockoutDuration = 'AccountLockoutDuration',
    antiDNSRebindEnabled = 'AntiDNSRebindEnabled'
}

export interface IAccount {
    id: string;
    label: string;
}

export enum TLSVersions {
    'TLS10' = 'Tls1.0',
    'TLS11' = 'Tls1.1',
    'TLS12' = 'Tls1.2',
}

export enum SSHAuthenticationMode {
    password = 'Password',
    publicKey = 'PublicKey',
}
