export class LeakDetection {

    private boardName: string;

    private pCBVersion: string;

    private boardId: string;

    private partNumber: string;

    constructor(
        boardName: string,
        pCBVersion: string,
        boardId: string,
        partNumber: string,
    ) {
        this.boardName = boardName;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
        this.partNumber = partNumber;
    }
}
