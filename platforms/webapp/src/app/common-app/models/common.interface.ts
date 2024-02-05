export enum WebStyle {
    Default = 'default',
    Beautiful = 'beautiful',
    Simple = 'simple',
    Standard = 'standard',
    StandardShenma = 'standard_a',
}

export enum PortType {
    SSH = 'SSH',
    SNMP = 'SNMP',
    KVM = 'KVM',
    VMM = 'VMM',
    VIDEO = 'VIDEO',
    VNC = 'VNC',
    HTTP = 'HTTP',
    HTTPS = 'HTTPS',
    IPMILANRMCP = 'IPMI',
    IPMILANRMCPPlus = 'IPMI1'
}

export enum InputLimit {
    len16 = 16,
    len20 = 20,
    len255 = 255
}

export enum PasswordLength {
    NOPWDCHECKLENGTH = 1,
    MIN = 8,
    CUSTOM = 12,
    MAX = 20
}

export interface IFatherInter {
    name: string;
}

export interface ITitle {
    title: string;
    width?: string;
}

export interface IOptions extends IFatherInter {
    id: string | number | boolean;
    label: string;
    disabled?: boolean;
    maxCapacity?: number;
}

export interface IHealthState extends IFatherInter {
    key: string | number | boolean;
    label: string;
    className: string;
}

export interface IRadio extends IFatherInter {
    id: string | number | boolean;
    disable: boolean;
    label: string;
}

export interface ISwitch extends IFatherInter {
    id: string | number | boolean;
    label: string;
    state: boolean;
}


export interface ICheckBok extends IFatherInter {
    id: string;
    label: string;
    checked: boolean;
    disable: boolean;
}

export interface IValue extends IFatherInter {
    label: string;
    value: string | number | boolean | [];
}

export interface IReturnVal {
    state: boolean;
    label: string;
}

export interface IWriteCachePolicy {
    ConfiguredDrive: string;
    HBADrive: string;
    UnconfiguredDrive: string;
}

export interface IMainMenuItem {
    id: string;
    hide: boolean;
    lable: string;
    router: string[];
    children: ISecondMenuItem[];
}

export interface ISecondMenuItem {
    id: string;
    navId: string;
    hide: boolean;
    lable: string;
    router: string[];
    iconClass: string;
    isIE: boolean;
}

export interface ILoginInfor {
    label: string;
    value: string;
}

const interNameArr = [
    'IOptions',
    'IHealthState',
    'IRadio',
    'ISwitch',
    'ICheckBok',
    'IValue',
    'IDetailInfo'
];

const classNameArr = [
    'IHealthState'
];
export { interNameArr, classNameArr };
