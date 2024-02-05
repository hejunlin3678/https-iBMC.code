import { IRadio, IOptions, ICheckBok, ISwitch } from 'src/app/common-app/models';
export enum Store {
    JBOD = 'JBOD',
    Online = 'Online',
    Offline = 'Offline',
    UnconfiguredGood = 'UnconfiguredGood',
    UnconfiguredBad = 'UnconfiguredBad'
}

export enum RaidType {
    PMC = 'PMC',
    BRCM = 'BRCM',
    ARIES = 'ARIES',
    HBA = 'HBA'
}

export interface IFromSet {
    lineHeight: string;
    fieldVerticalAlign: string;
    formType: string;
}


export interface IChildData {
    volumeSupport: boolean;
    oobSupport: boolean;
    maxSize: number;
    minSize: number;
    readPolicyConfig: boolean;
    writePolicyConfig: boolean;
    IOPolicyConfig: boolean;
    drivePolicyConfig: boolean;
    accessPolicyConfig: boolean;
}

export interface IVolumnDisabled {
    twoCache: boolean;
    readPolicyConfig: boolean;
    writePolicyConfig: boolean;
    iOPolicyConfig: boolean;
    drivePolicyConfig: boolean;
    accessPolicyConfig: boolean;
    isSSCD: boolean;
}

export interface IDriverEditData {
    indicatorLED: ISwitch;
    hotspareType: IRadio;
    firmwareStatus: IOptions;
    spareforLogicalDrive: IOptions | IOptions[];
    bootEnabled: IStorageRadio;
    bootPriority: IOptions;
    patrolState: IOptions;
}
export interface IVolumePolicyData {
    name: string;
    createCacheCadeFlag: boolean;
    optimumIOSizeBytes: IOptions;
    defaultReadPolicy: IOptions;
    defaultWritePolicy: IOptions;
    defaultCachePolicy: IOptions;
    driveCachePolicy: IOptions;
    accessPolicy: IOptions;
    initializationMode: IOptions;
    cacheLineSize?: IOptions;
    associationLogical?: IOptions;
}

export interface IRaidEditData {
    jBODState: boolean;
    copyBackState: boolean;
    smarterCopyBackState: boolean;
    mode: IOptions;
    noBatteryWriteCacheEnabled: boolean;
    readCachePercent: number;
    bootDevice1: IOptions;
    bootDevice2: IOptions;
    writeCacheType: IOptions;
    writeCachePolicy: IOptions;
    enabled: boolean;
    runningStatus: boolean;
    autoRepairEnabled: boolean;
    rate?: IOptions;
    period?: number;
    delayForStart?: number;
}

export interface IVolumeEditData {
    name: string;
    bootEnabled: IStorageRadio;
    sSDCachingEnabled: IStorageRadio;
    driveCachePolicy: IOptions;
    accessPolicy: IOptions;
    bGIEnabled: IOptions;
    defaultCachePolicy: IOptions;
    defaultWritePolicy: IOptions;
    defaultReadPolicy: IOptions;
}

export interface IVolumePMCEditData {
    name?: string;
    bootPriority?: IOptions;
    optimumIOSizeBytes?: IOptions;
    accelerationMethod?: IOptions;
    capacityBytes?: string;
    capacityUnit?: IOptions;
    defaultWritePolicy?: IOptions;
}
export interface IVolumeAriesEditData {
    name?: string;
    bootEnabled?: IOptions;
    defaultWritePolicy?: IOptions;
    defaultReadPolicy?: IOptions;
}

export interface IVolumeSPEditData {
    name?: string;
    bootEnabled?: IOptions;
    defaultWritePolicy?: IOptions;
    defaultReadPolicy?: IOptions;
    driveCachePolicy?: IOptions;
}

export interface IVolumeMembersData {
    volumeRaidLevel: IOptions;
    spanNumber: number;
    capacityMBytes: number;
    drives: number[];
}

export interface ISelect {
    disable: boolean;
    options: IOptions[];
}

export interface IDriveBox extends ICheckBok {
    driveId: string;
    isDisable: boolean;
    capacity: number;
    related: any;
    mediaType: string;
    firmware: string;
}

export interface IRaidButton {
    isShow: boolean;
    button: IButton[];
}

export interface IButton {
    id: string;
    label: string;
    isShow: boolean;
    disable: boolean;
}

export interface ISmartSwitch extends ISwitch {
    disable: boolean;
}

export interface ICapacity {
    value: number;
    unit: IOptions;
    options: IOptions[];
    batyValue: number;
    oldValue?: number;
}

export interface ISpanNum {
    value: number;
    disable: boolean;
    rang: ISpanCount;
}

export interface IStorageRadio extends IRadio {
    attrName: string;
}

export interface ISpanCount {
    min: number;
    max: number;
}
