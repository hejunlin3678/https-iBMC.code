export class CPUBoard {
    // 名称
    private boardName: string;
    // 厂商
    private manufacturer: string;
    // 槽位
    private id: number;
    // 类型
    private deviceType: string;
    // PCB版本
    private pCBVersion: string;
    // CPLD版本
    private cPLDVersion: string;
    // 单板ID
    private boardId: string;
    // CPLD版本
    private power: string;

    constructor(
        boardName: string,
        manufacturer: string,
        id: number,
        deviceType: string,
        pCBVersion: string,
        cPLDVersion: string,
        boardId: string,
        power: string
    ) {
        this.boardName = boardName;
        this.manufacturer = manufacturer;
        this.id = id;
        this.deviceType = deviceType;
        this.pCBVersion = pCBVersion;
        this.cPLDVersion = cPLDVersion;
        this.boardId = boardId;
        this.power = power;
    }

}
