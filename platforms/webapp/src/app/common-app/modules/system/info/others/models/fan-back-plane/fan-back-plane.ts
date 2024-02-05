export class FanBackPlane {
    // 名称
    private boardName: string;
    // 位置
    private position: string;
    // 厂商
    private manufacturer: string;
    // 类型
    private deviceType: string;
    // PCB版本
    private pCBVersion: string;
    // 单板ID
    private boardId: string;

    constructor(boardName: string, position: string, manufacturer: string, deviceType: string, pCBVersion: string, boardId: string) {
        this.boardName = boardName;
        this.position = position;
        this.manufacturer = manufacturer;
        this.deviceType = deviceType;
        this.pCBVersion = pCBVersion;
        this.boardId = boardId;
    }

}
