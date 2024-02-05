import { EChartsCoreOption } from 'echarts';

export interface IOverview {
    deviceInfo: IDeviceInfo;
    monitorInfo: IMonitorInfo;
    resourceInfo: IResourceInfo;
    accessInfo: IAccessInfo;
}

export interface IDeviceInfo {
    productName: string;
    aliasName: string;
    productPicture: string;
    productSN: string;
    biosVersion: string;
    systemSerialNumber: string;
    serverName: string;
    firmwareVersion: string;
    permanentMACAddress: string;
    GUID: string;
}

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
    memMaxNum?: number;
    memCurrentNum?: number;
    capacityGiB?: number;
    raidControllerNum?: number;
    logicalDriveNum?: number;
    physicalDriveNum?: number;
    powMaxNum?: number;
    powCurrentNum?: number;
    presentPSUNum?: number;
    fanMaxNum?: number;
    fanCurrentNum?: number;
    netMaxNum?: number;
    netCurrentNum?: number;
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

// 健康状态
export enum ResourceHealth {
  Critical = 'Critical',
  Warning = 'Warning',
  OK = 'OK',
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
export interface IServerDetail {
  productName?: string;
  aliasName?: string;
  productImg?: string;
  options: IDetailInfo[];
}
interface IDetailInfo {
  id: string;
  label: string;
  detail: string;
  value?: string;
  isEdit?: boolean;
  validation?: any;
  formControl?: any;
}
export interface IResourceDetail {
  label: string;
  infor: string | number;
}
export interface IResource {
  title: string;
  status: string;
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
export interface ILoginInfor {
  label: string;
  value: string;
}
export interface IEChartOption {
  id: string;
  navigatorUrl: string[];
  option: EChartsCoreOption;
  title: string;
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
