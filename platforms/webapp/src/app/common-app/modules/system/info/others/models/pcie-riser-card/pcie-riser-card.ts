export class PCIERiserCard {

    // 名称
    private boardName: string;
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
        boardName: string,
        manufacturer: string,
        cardNo: number,
        description: string,
        pCBVersion: string, boardId: string,
        partNumber: string,
        serialNumber: number
    ) {
        this.boardName = boardName;
        this.manufacturer = manufacturer;
        this.cardNo = cardNo;
        this.description = description;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
    }

}
