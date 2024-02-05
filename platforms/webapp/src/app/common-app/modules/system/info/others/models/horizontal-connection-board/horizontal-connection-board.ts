export class HorizontalAdapter {
    // 名称
    private boardName: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;

    constructor(boardName: string, pCBVersion: string, boardId: string) {
        this.boardName = boardName;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
    }
}
