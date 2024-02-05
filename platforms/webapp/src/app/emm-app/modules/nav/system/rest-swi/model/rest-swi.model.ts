export class SwapInfo {
    public slotNo: string;
    public boardType: string;
    public planeType: string;
    public flieName: string;
    public backCount: number;
    public detailList: string[];

    constructor(
        slotNo: string,
        boardType: string,
        planeType: string,
        flieName: string,
        backCount: number,
        detailList: string[]
    ) {
        this.slotNo = slotNo;
        this.boardType = boardType;
        this.planeType = planeType;
        this.flieName = flieName;
        this.backCount = backCount;
        this.detailList = detailList;
    }
}
