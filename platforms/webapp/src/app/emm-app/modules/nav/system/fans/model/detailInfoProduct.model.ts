
export class DetailInfoProduct {
    private manufacturer: [string, string];
    private partNumber: [string, string];
    private productName: [string, string];
    private serialNumber: [string, string];
    private version: [string, string];

    constructor(
        manufacturer: string,
        partNumber: string,
        productName: string,
        serialNumber: string,
        version: string,
    ) {
        this.manufacturer = ['EMM_FAN_MANUFACTURER', manufacturer];
        this.partNumber = ['EMM_FAN_PARTNUMBER', partNumber];
        this.productName = ['INFO_PRODUCT_NAME', productName];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        this.version = ['OTHER_VERSION', version];
    }
}
