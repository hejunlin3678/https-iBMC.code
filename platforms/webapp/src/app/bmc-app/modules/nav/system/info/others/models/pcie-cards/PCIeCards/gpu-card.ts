import { ExtendAttr } from '../../extend-attribute';

export class GPUCard extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // 序列号
    private serialNumber: [string, string];
    // 固件版本
    private firmwareVersion: [string, string];

    constructor(productName: string, serialNumber: string, firmwareVersion: string) {
        super();
        if (productName !== '--') {
            this.productName = ['COMMON_NAME', productName];
        }
        if (serialNumber !== '--') {
            this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        }
        if (firmwareVersion !== '--') {
            this.firmwareVersion = ['COMMON_FIXED_VERSION', firmwareVersion];
        }
    }

}
