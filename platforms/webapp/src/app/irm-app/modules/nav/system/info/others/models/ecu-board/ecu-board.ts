export class ECUBoard {
    // 名称
    private name: string;
    // 厂商
    private manufacturer: string;
    // 序列号
    private serialNumber: number;

    constructor(
        name: string,
        manufacturer: string,
        serialNumber: number
    ) {
        this.name = name;
        this.manufacturer = manufacturer;
        this.serialNumber = serialNumber;
    }
}
