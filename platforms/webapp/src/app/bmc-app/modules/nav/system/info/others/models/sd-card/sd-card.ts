export class SDCard {
    // 槽位
    private id: number;
    // 厂商
    private manufacturer: string;
    // 序列号
    private serialNumber: string;
    // 容量
    private capacity: number;

    constructor(id: number, manufacturer: string, serialNumber: string, capacity: number) {
        this.id = id;
        this.manufacturer = manufacturer;
        this.serialNumber = serialNumber;
        this.capacity = capacity;
    }
}
