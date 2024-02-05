export class MemoryBoard {
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
    // 单板ID
    private boardId: string;
    private componentUniqueID: string;

    constructor(
        boardName: string,
        manufacturer: string,
        id: number,
        deviceType: string,
        pCBVersion: string,
        boardId: string,
        componentUniqueID: string
    ) {
        this.boardName = boardName;
        this.manufacturer = manufacturer;
        this.id = id;
        this.deviceType = deviceType;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.componentUniqueID = componentUniqueID;
    }
}
