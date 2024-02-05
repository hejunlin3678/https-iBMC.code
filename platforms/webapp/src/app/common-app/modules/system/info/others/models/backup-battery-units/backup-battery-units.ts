export class BackupBatteryUnits {
    private boardName: string;
    private id: number;
    private firmwareVersion: string;
    private workStatus: string;
    private capacity: string;
    private batteryModel: string;
    private serialNumber: string;
    private manufacturer: string;
    private inPosition: string;

    constructor(
        boardName: string,
        id: number,
        firmwareVersion: string,
        workStatus: string,
        capacity: string,
        batteryModel: string,
        serialNumber: string,
        manufacturer: string,
        inPosition: string
    ) {
        this.boardName = boardName;
        this.id = id;
        this.firmwareVersion = firmwareVersion;
        this.workStatus = workStatus;
        this.capacity = capacity;
        this.batteryModel = batteryModel;
        this.serialNumber = serialNumber;
        this.manufacturer = manufacturer;
        this.inPosition = inPosition;
    }
}
