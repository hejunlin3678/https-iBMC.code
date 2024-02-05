export class GPUBoard {
    // 名称
    private boardName: string;
    // 槽位
    private slot: string;
    // 单板ID
    private boardId: string;
    private componentUniqueID: string;
    // PCB版本
    private pCBVersion: string;
    // CPLD版本
    private cPLDVersion: string;

    constructor(boardName: string, slot: string, boardId: string, componentUniqueID: string, pCBVersion: string, cPLDVersion: string) {
        this.boardName = boardName;
        this.slot = slot;
        this.boardId = boardId;
        this.componentUniqueID = componentUniqueID;
        this.pCBVersion = pCBVersion;
        this.cPLDVersion = cPLDVersion;
    }
}
