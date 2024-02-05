export class M2TransformCard {

    // 名称
    private boardName: string;
    // 描述
    private description: string;
    // 槽位
    private id: number;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;
    // 部件编码
    private partNumber: string;
    // 序列号
    private serialNumber: number;

    constructor(
        boardName: string,
        description: string,
        id: number,
        pCBVersion: string,
        boardId: string,
        partNumber: string,
        serialNumber: number
    ) {
        this.boardName = boardName;
        this.description = description;
        this.id = id;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
    }
}
