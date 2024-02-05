export class IOBoard {
    // 名称
    private boardName: string;
    // 位置
    private location: string;
    // 厂商
    private manufacturer: string;
    // 类型
    private description: string;
    // PCB版本
    private pCBVersion: string;
    // CPLD版本
    private cPLDVersion: string;
    // 单板ID
    private boardId: string;
    // 功率
    private power: string;
    // 部件编码
    private partNumber: string;
    // 序列号
    private serialNumber: number;

    constructor(
        boardName: string,
        location: string,
        manufacturer: string,
        description: string,
        pCBVersion: string,
        cPLDVersion: string,
        boardId: string,
        power: string,
        partNumber: string,
        serialNumber: number
    ) {
        this.boardName = boardName;
        this.location = location;
        this.manufacturer = manufacturer;
        this.description = description;
        this.pCBVersion = pCBVersion;
        this.cPLDVersion = cPLDVersion;
        this.boardId = boardId;
        this.power = power;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
    }
}
