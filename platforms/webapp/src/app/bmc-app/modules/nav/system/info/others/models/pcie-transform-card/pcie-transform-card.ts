export class PCIETransformCard {
    // 名称
    private boardName: string;
    // 描述
    private description: string;
    // 编号
    private cardNo: number;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;
    private componentUniqueID: string;

    constructor(boardName: string, description: string, cardNo: number, pCBVersion: string, boardId: string, componentUniqueID: string) {
        this.boardName = boardName;
        this.description = description;
        this.cardNo = cardNo;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.componentUniqueID = componentUniqueID;
    }
}
