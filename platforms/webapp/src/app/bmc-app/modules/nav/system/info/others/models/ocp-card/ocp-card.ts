import { ExtendAttr } from '../extend-attribute';

export class OCPCard {
    // 描述
    private description: string;
    // 位置
    private position: string;
    // 厂商
    private manufacturer: string;
    // 槽位
    private id: number;
    // 制造商ID
    private vendorId: number;
    // 设备ID
    private deviceId: number;
    // 子厂商ID
    private subsystemVendorId: number;
    // 子设备ID
    private subsystemId: number;
    // 资源归属
    private associatedResource: string;
    // 扩展信息
    private extendAttr: ExtendAttr;

    constructor(
        description: string,
        position: string,
        manufacturer: string,
        id: number,
        vendorId: number,
        deviceId: number,
        subsystemVendorId: number,
        subsystemId: number,
        associatedResource: string,
        extendAttr: ExtendAttr
    ) {
        this.description = description;
        this.position = position;
        this.manufacturer = manufacturer;
        this.id = id;
        this.vendorId = vendorId;
        this.deviceId = deviceId;
        this.subsystemVendorId = subsystemVendorId;
        this.subsystemId = subsystemId;
        this.associatedResource = associatedResource;
        this.extendAttr = extendAttr;
    }
}
