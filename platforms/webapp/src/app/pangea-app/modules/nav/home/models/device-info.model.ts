import { IDeviceInfo } from './overview.datatype';

export class DeviceInfo {
    private serialNumber: [string, string];
    private serverName: [string, string];
    private firmwareVersion: [string, string];
    private biosVersion: [string, string];
    private macAddress: [string, string];
    private guid: [string, string];

    constructor(deviceInfo: IDeviceInfo) {
        this.serialNumber = ['HOME_PROJECTNO', deviceInfo.systemSerialNumber || '--'];
        this.serverName = ['HOME_HOST_NAME_DETAIL', deviceInfo.serverName || '--'];
        this.firmwareVersion = ['HOME_IBMC_VERSION', deviceInfo.firmwareVersion || '--'];
        this.biosVersion = ['HOME_BIOS_VERSION', deviceInfo.biosVersion || '--'];
        this.macAddress = ['NET_MAC_ADDRESS', deviceInfo.permanentMACAddress || '--'];
        this.guid = ['HOME_GUID_TAG', deviceInfo.GUID || '--'];
    }

    get getServerName(): string {
        return this.serverName[1];
    }
}
