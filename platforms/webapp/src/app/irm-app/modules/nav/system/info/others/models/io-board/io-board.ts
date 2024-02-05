export class IOBoard {
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
    // 单板ID
    private boardId: string;
    // 部件编码
    private partNumber: string;
    // 序列号
    private serialNumber: number;

    constructor(
        productName: string,
        location: string,
        manufacturer: string,
        cardNo: number,
        description: string,
        pCBVersion: string,
        boardId: string,
        partNumber: string,
        serialNumber: number
    ) {
        this.productName = productName;
        this.location = location;
        this.manufacturer = manufacturer;
        this.cardNo = cardNo;
        this.description = description;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
    }
}
