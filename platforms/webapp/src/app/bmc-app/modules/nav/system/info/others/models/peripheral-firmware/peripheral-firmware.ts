export class PeripheralFirmware {
    // 名称
    private boardName: string;
    // 厂商
    private manufacturer: string;
    // 版本
    private version: string;

    constructor(boardName: string, manufacturer: string, version: string) {
        this.boardName = boardName;
        this.manufacturer = manufacturer;
        this.version = version;
    }
}
