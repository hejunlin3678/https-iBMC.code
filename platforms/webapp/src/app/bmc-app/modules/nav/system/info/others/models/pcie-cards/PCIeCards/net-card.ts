import { ExtendAttr } from '../../extend-attribute';

export class NetCard extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // 单板ID
    private boardId: [string, string];
    // PCB版本
    private pcbVersion: [string, string];
    // CPLD版本
    private cpldVersion: [string, string];
    // 固件版本
    private firmwareVersion: [string, string];
    // 当前带宽
    private linkWidth: [string, string];
    // 带宽规格
    private linkWidthAbility: [string, string];
    // 当前速率
    private linkSpeed: [string, string];
    // 连接速率规格
    private linkSpeedAbility: [string, string];
    // 子厂商ID
    private subsystemVendorId: [string, string];
    // 子设备ID
    private subsystemId: [string, string];

    constructor(
        productName: string,
        boardId: string,
        pcbVersion: string,
        cpldVersion: string,
        firmwareVersion: string,
        linkWidth: string,
        linkWidthAbility: string,
        linkSpeed: string,
        linkSpeedAbility: string,
        subsystemVendorId: string,
        subsystemId: string
    ) {
        super();
        if (productName !== '--') {
            this.productName = ['COMMON_NAME', productName];
        }
        if (boardId !== '--') {
            this.boardId = ['OTHER_BOARD_ID', boardId];
        }
        if (pcbVersion !== '--') {
            this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        }
        if (cpldVersion !== '--') {
            this.cpldVersion = ['COMMIN_CPLD_VERSION', cpldVersion];
        }
        if (firmwareVersion !== '--') {
            this.firmwareVersion = ['COMMON_FIXED_VERSION', firmwareVersion];
        }
        if (linkWidth !== '--') {
            this.linkWidth = ['OTHER_CURRENT_BROAD_BAND', linkWidth];
        }
        if (linkWidthAbility !== '--') {
            this.linkWidthAbility = ['OTHER_BROAD_BAND_SPECIFICATIONS', linkWidthAbility];
        }
        if (linkSpeed !== '--') {
            this.linkSpeed = ['OTHER_CURRENT_RATE', linkSpeed];
        }
        if (linkSpeedAbility !== '--') {
            this.linkSpeedAbility = ['OTHER_CURRENT_RATE_SPECIFICATIONS', linkSpeedAbility];
        }
        if (subsystemVendorId !== '--') {
            this.subsystemVendorId = ['OTHER_SON_VENDOR_ID', subsystemVendorId];
        }
        if (subsystemId !== '--') {
            this.subsystemId = ['OTHER_SON_DEVICE_ID', subsystemId];
        }
    }
}
