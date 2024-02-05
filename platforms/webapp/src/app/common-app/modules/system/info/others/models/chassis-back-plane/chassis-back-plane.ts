export class ChassisBackplane {
    // 名称
    private productName: string;
    // 位置
    private location: string;
    // 厂商
    private manufacturer: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;

    constructor(productName: string, location: string, manufacturer: string, pCBVersion: string, boardId: string) {
        this.productName = productName;
        this.location = location;
        this.manufacturer = manufacturer;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
    }
}
