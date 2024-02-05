export class ExpandBoard {
    private boardName: string;
    private boardID: string;
    private cPLDVersion: string;
    private cardNo: string;
    private description: string;
    private location: string;
    private manufacturer: string;
    private pCBVersion: string;
    private partNumber: string;
    private serialNumber: string;

    constructor(
        boardName: string,
        boardID: string,
        cPLDVersion: string,
        cardNo: string,
        description: string,
        location: string,
        manufacturer: string,
        pCBVersion: string,
        partNumber: string,
        serialNumber: string
    ) {
        this.boardName = boardName;
        this.boardID = boardID;
        this.cPLDVersion = cPLDVersion;
        this.cardNo = cardNo;
        this.description = description;
        this.location = location;
        this.manufacturer = manufacturer;
        this.pCBVersion = pCBVersion;
        this.partNumber = partNumber;
        this.serialNumber = serialNumber;
    }
}
