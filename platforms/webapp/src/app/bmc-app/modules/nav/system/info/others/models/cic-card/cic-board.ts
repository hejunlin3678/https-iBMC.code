import { ExtendAttr } from '../extend-attribute';
export class CicBoard {
    private boardName: string;
    private manufacturer: string;
    private pcbVer: string;
    private boardId: string;
    private componentUniqueID: string;
    private description: string;
    private serialNumber: string;
    private partNum: string;
    private extendAttr: ExtendAttr;

    constructor(
        boardName: string,
        manufacturer: string,
        pcbVer: string,
        boardId: string,
        componentUniqueID: string,
        description: string,
        serialNumber: string,
        partNum: string,
        extendAttr: ExtendAttr
    ) {
        this.boardName = boardName;
        this.manufacturer = manufacturer;
        this.pcbVer = pcbVer;
        this.boardId = boardId;
        this.componentUniqueID = componentUniqueID;
        this.description = description;
        this.serialNumber = serialNumber;
        this.partNum = partNum;
        this.extendAttr = extendAttr;
    }
}
