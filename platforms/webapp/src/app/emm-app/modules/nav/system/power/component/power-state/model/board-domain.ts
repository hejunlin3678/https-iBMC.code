
export class BoardDomain {
   public fileId: string;
   public manufacturer: string;
   public manufacturingDate: string;
   public partNumber: string;
   public productName: string;
   public serialNum: string;

   constructor(board: BoardDomain) {
        const {
            fileId,
            manufacturer,
            manufacturingDate,
            partNumber,
            productName,
            serialNum
        } = board;
        this.fileId = fileId;
        this.manufacturer = manufacturer;
        this.manufacturingDate = manufacturingDate;
        this.partNumber = partNumber;
        this.productName = productName;
        this.serialNum = serialNum;
    }
}
