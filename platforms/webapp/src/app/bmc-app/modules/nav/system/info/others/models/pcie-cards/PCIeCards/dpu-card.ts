import { ExtendAttr } from '../../extend-attribute';

export class DpuCard extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // IPv4地址
    private storageManagementIP: [string, string];
    // IPv6地址
    private storageManagementIPv6: [string, string];
    // 带宽规格
    private linkWidthAbility: [string, string];
    // 子厂商ID
    private subsystemVendorId: [string, string];
    // 子设备ID
    private subsystemId: [string, string];
    // PCB版本
    private pcbVersion: [string, string];
    // 端口状态
    private portStatus: [string, string];
    // Vrd版本
    private vrdInfo: {VrdName: string, VrdVersion: string}[];

    constructor(
        productName: string,
        storageManagementIP:  string,
        storageManagementIPv6: string,
        linkWidthAbility: string,
        subsystemVendorId: string,
        subsystemId: string,
        pcbVersion: string,
        portStatus: string,
        vrdInfo: {VrdName: string, VrdVersion: string}[]
    ) {
        super();
        if (productName !== '--') {
            this.productName = ['COMMON_NAME', productName];
        }
        if (storageManagementIP !== '--') {
          this.storageManagementIP = ['HBA_STORAGE_IPV4', storageManagementIP];
        }
        if (storageManagementIPv6 !== '--') {
          this.storageManagementIPv6 = ['HBA_STORAGE_IPV6', storageManagementIPv6];
        }
        if (linkWidthAbility !== '--') {
          this.linkWidthAbility = ['OTHER_BROAD_BAND_SPECIFICATIONS', linkWidthAbility];
        }
        if (subsystemVendorId !== '--') {
          this.subsystemVendorId = ['OTHER_SON_VENDOR_ID', subsystemVendorId];
        }
        if (subsystemId !== '--') {
            this.subsystemId = ['OTHER_SON_DEVICE_ID', subsystemId];
        }
        if (pcbVersion !== '--') {
          this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        }
        if (portStatus !== '--') {
          this.portStatus = ['HBA_PORT_STATUS', portStatus];
        }
        if (vrdInfo?.length > 0) {
          this.vrdInfo = vrdInfo;
        }
    }
}
