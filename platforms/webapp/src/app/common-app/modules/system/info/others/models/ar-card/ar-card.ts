export class ArCard {
    // 单板名称
    private boardName: string;
    // 槽位
    private id: number;
    // 类型
    private description: string;
    // 位置
    private location: string;
    // 厂商
    private manufacturer: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;

    constructor(
        boardName: string,
        id: number,
        description: string,
        location: string,
        manufacturer: string,
        pCBVersion: string,
        boardId: string
    ) {
        this.boardName = boardName;
        this.id = id;
        this.description = description;
        this.location = location;
        this.manufacturer = manufacturer;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
    }
}
