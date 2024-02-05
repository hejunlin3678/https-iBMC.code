import { Ports } from '../../ports';

export class SDIExtendCard {
    // 标题
    private title: string;
    // 名称
    private productName: [string, string];
    // PCB版本
    private pcbVersion: [string, string];
    // 单板名称
    private boardName: [string, string];
    // 单板ID
    private boardId: [string, string];
    // 设备ID
    private deviceId: [string, string];
    // 制造商ID
    private vendorId: [string, string];
    // 子厂商ID
    private subsystemVendorId: [string, string];
    // 子设备ID
    private subsystemId: [string, string];
    // 厂商
    private manufacturer: [string, string];
    // 描述
    private description: [string, string];
    // 芯片厂商
    private chipManufacturer: [string, string];
    // 芯片型号
    private chipModel: [string, string];
    // 端口属性
    private ports: Ports;

    constructor(
        title: string,
        productName: string,
        pcbVersion: string,
        boardName: string,
        boardId: string,
        deviceId: string,
        vendorId: string,
        subsystemVendorId: string,
        subsystemId: string,
        manufacturer: string,
        description: string,
        chipManufacturer: string,
        chipModel: string,
        ports: Ports
    ) {
        this.title = title;
        if (productName !== '--') {
            this.productName = ['COMMON_NAME', productName];
        }
        if (pcbVersion !== '--') {
            this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        }
        if (boardName !== '--') {
            this.boardName = ['OTHER_BOARDNAME', boardName];
        }
        if (boardId !== '--') {
            this.boardId = ['OTHER_BOARD_ID', boardId];
        }
        if (deviceId !== '--') {
            this.deviceId = ['OTHER_DEVICE_ID', deviceId];
        }
        if (vendorId !== '--') {
            this.vendorId = ['OTHER_MANUFACTURER_ID', vendorId];
        }
        if (subsystemVendorId !== '--') {
            this.subsystemVendorId = ['OTHER_SON_VENDOR_ID', subsystemVendorId];
        }
        if (subsystemId !== '--') {
            this.subsystemId = ['OTHER_SON_DEVICE_ID', subsystemId];
        }
        if (manufacturer !== '--') {
            this.manufacturer = ['COMMON_MANUFACTURER', manufacturer];
        }
        if (description !== '--') {
            this.description = ['COMMON_DESCRIPTION', description];
        }
        if (chipManufacturer !== '--') {
            this.chipManufacturer = ['OTHER_CHIP_VENDOR', chipManufacturer];
        }
        if (chipModel !== '--') {
            this.chipModel = ['OTHER_CHIP_MODEL', chipModel];
        }
        this.ports = ports;
    }
}
