
export interface IOverview {
    deviceInfo: IDeviceInfo;
    monitorInfo: IMonitorInfo;
    resourceInfo: IResourceInfo;
    accessInfo: IAccessInfo;
}

export interface IDeviceInfo {
    productName: string;
    position: string;
    capacity: string;
    serialNumber: string;
    firmwareVersion: string;
}

export interface IMonitorInfo {
    totalU: number;
    usedU: number;
    deviceTotalNumber: number;
    serverNumber?: number;
    networkNumber?: number;
    storeNumber?: number;
    otherNumber?: number;
    maxRatedPower: number;
    currentPower: number;
}

export interface IResourceInfo {
    totalPower: number;
    currentPower: number;
    totalBattary: number;
    currentBattary: number;
    totalFun: number;
    currentFun: number;
}

export interface IAccessInfo {
    nodeIp: string;
    VGAUSBDVDEnabled: boolean;
    mode: string;
    address: string;
}

export interface IAccess {
    show: boolean;
    title: string;
    icon: string;
    id: string;
    state?: string;
    disable?: boolean;
    onKeySelect?: boolean;
    restoreSet?: boolean;
    nodeChange?: boolean;
    nodeIp?: string;
    lock?: boolean;
}

export interface IResourceDetail {
    label: string;
    infor: string | number;
}

export interface IResource {
    title: string;
    backgroundClassName: string;
    details: IResourceDetail[];
    targetNav: string[];
    id: string;
    hide?: boolean;
}

export interface ILoginInfor {
    label: string;
    value: string;
}

export interface IMonitorParams {
    value: number;
    formatter: string;
    minorValue: number;
    lineHeight?: number;
    startAngle?: number;
    clockwise?: boolean;
    computedValue?: number;
    majorVal?: number;
    minVal?: number;
    maxVal?: number;
}

export interface ICollectProgress {
    state: string;
    taskPercentage: string;
    downloadUrl?: string;
}
