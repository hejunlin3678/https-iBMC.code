export class SDStorage {

    // 厂商
    private manufacturer: string;
    // 固件版本
    private firmwareVersion: string;

    constructor(manufacturer: string, firmwareVersion: string) {
        this.manufacturer = manufacturer;
        this.firmwareVersion = firmwareVersion;
    }
}
