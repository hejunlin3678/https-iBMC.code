export class ProductDomain {
    public manufacturer: string;
    public partNumber: string;
    public productName: string;
    public serialNum: string;
    public version: string;
    constructor(product: ProductDomain) {
        const {
            manufacturer,
            partNumber,
            productName,
            serialNum,
            version
        } = product;
        this.manufacturer = manufacturer;
        this.partNumber = partNumber;
        this.productName = productName;
        this.serialNum = serialNum;
        this.version = version;
    }
}
