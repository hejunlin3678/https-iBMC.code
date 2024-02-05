import { IDeviceInfo } from './overview.datatype';

export class DeviceInfo {
    private serialNumber: [string, string];
    private serverName: [string, string];
    private firmwareVersion: [string, string];
    private biosVersion: [string, string];
    private teeOsVersion: [string, string];
    private macAddress: [string, string];
    private guid: [string, string];
    private systemNumber: [string, string];

    constructor(deviceInfo: IDeviceInfo) {
        this.serialNumber = ['HOME_PROJECTNO', deviceInfo.systemSerialNumber || '--'];
        if (deviceInfo.systemSN !== deviceInfo.systemSerialNumber) {
            this.systemNumber = ['INFO_SYSTEM_NUMBER', deviceInfo.systemSN || '--'];
        }
        this.serverName = ['HOME_HOST_NAME_DETAIL', deviceInfo.serverName || '--'];
        this.firmwareVersion = ['HOME_IBMC_VERSION', deviceInfo.firmwareVersion || '--'];
        let biosVersionInfo = '';
        if (deviceInfo.biosVersion !== undefined) {
            biosVersionInfo = deviceInfo.biosVersion || '--';
        }
        this.biosVersion = ['HOME_BIOS_VERSION', biosVersionInfo];
        this.teeOsVersion = ['HOME_TEEOS_FIRMWARE_VERSION', deviceInfo.teeOsVersion !== '' ? deviceInfo.teeOsVersion : '--' ];
        this.macAddress = ['NET_MAC_ADDRESS', deviceInfo.permanentMACAddress || '--'];
        this.guid = ['HOME_GUID_TAG', deviceInfo.GUID || '--'];
    }

    get getServerName(): string {
        return this.serverName[1];
    }
}
