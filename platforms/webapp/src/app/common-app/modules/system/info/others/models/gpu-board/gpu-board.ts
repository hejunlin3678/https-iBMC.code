export class GPUBoard {
    // 名称
    private boardName: string;
    // 单板ID
    private boardId: string;
    // PCB版本
    private pCBVersion: string;
    // CPLD版本
    private cPLDVersion: string;

    constructor(boardName: string, boardId: string, pCBVersion: string, cPLDVersion: string) {
        this.boardName = boardName;
        this.boardId = boardId;
        this.pCBVersion = pCBVersion;
        this.cPLDVersion = cPLDVersion;
    }
}
