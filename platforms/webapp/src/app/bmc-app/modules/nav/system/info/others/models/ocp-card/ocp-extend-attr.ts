import { ExtendAttr } from '../extend-attribute';


export class OCPExtendAttr extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // 子厂商ID
    private subsystemVendorId: [string, number];
    // 子设备ID
    private subsystemId: [string, number];

    constructor(
        productName: string,
        subsystemVendorId: number,
        subsystemId: number,
    ) {
        super();
        this.productName = ['COMMON_NAME', productName];
        this.subsystemVendorId = ['OTHER_SON_VENDOR_ID', subsystemVendorId];
        this.subsystemId = ['OTHER_SON_DEVICE_ID', subsystemId];
    }
}
