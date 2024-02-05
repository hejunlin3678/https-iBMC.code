import { ResourceInfo, Accesses, EchartsOption, ServerDetail } from '../models';

export interface IOverview {
    deviceInfo: ServerDetail;
    monitorInfo: EchartsOption;
    resourceInfo: ResourceInfo;
    accessInfo: Accesses;
}

export interface IDeviceInfo {
    productName: string;
    aliasName: string;
    productPicture: string;
    productSN: string;
    systemSN: string;
    biosVersion: string;
    teeOsVersion: string;
    systemSerialNumber: string;
    serverName: string;
    firmwareVersion: string;
    permanentMACAddress: string;
    GUID: string;
}
