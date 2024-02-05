export class DetailInfo {
    private fruFileId: [string, string];
    private manufacturer: [string, string];
    private manufacturingDate: [string, string];
    private partNumber: [string, string];
    private productName: [string, string];
    private serialNumber: [string, string];

    constructor(
        fruFileId: string,
        manufacturer: string,
        manufacturingDate: string,
        partNumber: string,
        productName: string,
        serialNumber: string,
    ) {
        this.fruFileId = ['EMM_FAN_FILE_ID', fruFileId];
        this.manufacturer = ['EMM_FAN_MANUFACTURER', manufacturer];
        this.manufacturingDate = ['EMM_FAN_MANUFACTURER_DATE', manufacturingDate];
        this.partNumber = ['EMM_FAN_PARTNUMBER', partNumber];
        this.productName = ['INFO_PRODUCT_NAME', productName];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
    }
}
