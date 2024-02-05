
export class PeripheralCard {
     // 名称
     private boardName: string;
     // 类型
    private description: string;
    // 厂商
    private manufacturer: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;

    constructor(boardName: string, description: string, manufacturer: string, pCBVersion: string, boardId: string) {
        this.boardName = boardName;
        this.description = description;
        this.manufacturer = manufacturer;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
    }
}
