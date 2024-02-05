export class HorizontalAdapter {
    // 名称
    private boardName: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;
    private componentUniqueID: string;

    constructor(boardName: string, pCBVersion: string, boardId: string, componentUniqueID: string) {
        this.boardName = boardName;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.componentUniqueID = componentUniqueID;
    }
}
