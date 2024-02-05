export class MezzCard {
    // 名称
    private productName: string;
    // 描述
    private description: string;
    // 编号
    private cardNo: number;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;
    // 资源归属
    private associatedResource: string;
    // 当前带宽
    private linkWidth: string;
    // 当前速率
    private linkSpeed: string;
    // 带宽规格
    private linkWidthAbility: string;
    // 速率规格
    private linkSpeedAbility: string;
    // 部件编码
    private partNumber: string;
    // 序列号
    private serialNumber: number;

    constructor(
        productName: string,
        description: string,
        cardNo: number,
        pCBVersion: string,
        boardId: string,
        associatedResource: string,
        linkWidth: string,
        linkSpeed: string,
        linkWidthAbility: string,
        linkSpeedAbility: string,
        partNumber: string,
        serialNumber: number
    ) {
        this.productName = productName;
        this.description = description;
        this.cardNo = cardNo;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.associatedResource = associatedResource;
        this.linkWidth = linkWidth;
        this.linkSpeed = linkSpeed;
        this.linkWidthAbility = linkWidthAbility;
        this.linkSpeedAbility = linkSpeedAbility;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
    }
}
