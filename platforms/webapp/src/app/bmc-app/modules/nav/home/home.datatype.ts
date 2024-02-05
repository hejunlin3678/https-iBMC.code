import { DeviceInfo } from './models/device-info.model';
import { EChartsCoreOption } from 'echarts';


export interface IMonitorInfo {
    diskThresholdPercent?: number;
    diskUsagePercent?: number;
    proThresholdPercent?: number;
    proUseagePercent?: number;
    memThresholdPercent?: number;
    memUseagePercent?: number;
    inletTemperature?: number;
    inletTempMinorThreshold?: number;
    inletTempMajorThreshold?: number;
    powerConsumedWatts?: number;
    powerCapacityWatts?: number;
    upperPowerThresholdWatts?: number;
}

export interface IResourceInfo {
    proMaxNum?: number;
    proCurrentNum?: number;
    processorStatus?: number;
    memMaxNum?: number;
    memCurrentNum?: number;
    capacityGiB?: number;
    memoryStatus?: number;
    raidControllerNum?: number;
    logicalDriveNum?: number;
    physicalDriveNum?: number;
    storageStatus?: number;
    powMaxNum?: number;
    powCurrentNum?: number;
    presentPSUNum?: number;
    powerStatus?: number;
    fanMaxNum?: number;
    fanCurrentNum?: number;
    fanStatus?: number;
    netMaxNum?: number;
    netCurrentNum?: number;
    netadapterStatus?: number;
}

export interface IAccessInfo {
    nodeIp: string;
    FusionPartition: SystemModel;
    VGAUSBDVDEnabled: boolean;
    mode: string;
    address: string;
}

export enum SystemModel {
    SingleSystem = 'SingleSystem',
    DualSystem = 'DualSystem'
}

export interface IAccess {
    show: boolean;
    title: string;
    icon: string;
    id: string;
    imgUrl: string;
    state?: string;
    disable?: boolean;
    onKeySelect?: boolean;
    restoreSet?: boolean;
    nodeChange?: boolean;
    nodeIp?: string;
    lock?: boolean;
}
export interface IServerDetail {
    productName: string;
    aliasName: string;
    productImg: string;
    errorImg: any;
    deviceInfo: DeviceInfo;
}
export interface IDetailInfo {
    id: string;
    label: string;
    detail: string;
}
export interface IResourceDetail {
    label: string;
    infor: string | number;
}
export interface IResource {
    title: string;
    healthStatus?: number;
    backgroundClassName: string;
    details: IResourceDetail[];
    targetNav: string[];
    id: string;
    hide?: boolean;
}
export interface IVirtualButton {
    label: string;
    disabled: boolean;
    id: string;
    show: boolean;
}
export enum VirtualBtnId {
    h5Only = 'h5Only',
    h5Share = 'h5Share',
    javaOnly = 'javaOnly',
    javaShare = 'javaShare',
    more = 'more'
}

export interface IEChartOption {
    id: string;
    navigatorUrl: string[];
    option: EChartsCoreOption;
    routerParam?: string;
    show?: boolean;
    title?: string;
}

export interface IMonitorParams {
    value: number;
    formatter?: string;
    minorValue: number;
    title?: string;
    lineHeight?: number;
    startAngle?: number;
    clockwise?: boolean;
    computedValue?: number;
    majorVal?: number;
    minVal?: number;
    maxVal?: number;
    isSimpleTemp?: boolean;
}

export interface ICollectProgress {
    state: string;
    taskPercentage: string;
    downloadUrl?: string;
}
