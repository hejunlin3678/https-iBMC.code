export class RaidCard {
    // 名称
    private productName: string;
    // 位置
    private location: string;
    // 厂商
    private manufacturer: string;
    // 编号
    private cardNo: number;
    // 类型
    private description: string;
    // PCB版本
    private pCBVersion: string;
    // CPLD版本
    private cPLDVersion: string;
    // 部件编码
    private partNumber: string;
    // 序列号
    private serialNumber: number;
    // 单板ID
    private boardId: string;
    private componentUniqueID: string;
    // 资源归属
    private associatedResource: string;

    constructor(
        productName: string,
        location: string,
        manufacturer: string,
        cardNo: number,
        description: string,
        pCBVersion: string,
        cPLDVersion: string,
        boardId: string,
        partNumber: string,
        serialNumber: number,
        associatedResource: string,
        componentUniqueID: string
    ) {
        this.productName = productName;
        this.location = location;
        this.manufacturer = manufacturer;
        this.cardNo = cardNo;
        this.description = description;
        this.pCBVersion = pCBVersion;
        this.cPLDVersion = cPLDVersion;
        this.boardId = boardId;
        this.componentUniqueID = componentUniqueID;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
        this.associatedResource = associatedResource;
    }
}
