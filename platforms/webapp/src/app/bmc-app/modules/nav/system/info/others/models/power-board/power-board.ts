export class PowerBoard {
    // 名称
    private boardName: string;
    // 类型
    private deviceType: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;

    constructor(boardName: string, deviceType: string, pCBVersion: string, boardId: string) {
        this.boardName = boardName;
        this.deviceType = deviceType;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
    }
}
