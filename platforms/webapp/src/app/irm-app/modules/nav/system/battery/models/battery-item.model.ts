export class BatteryItem {
    private id: number;
    private slot: any;
    private deviceLocator: string;
    private manufacturer: string;
    private model: string;
    private firmwareVersion: string;
    private partNumber: string;
    private workingState: string;
    private capacity: string;
    private dischargeTimes: string;
    private usageDuration: string;
    private SOC: string;
    private SOH: string;

    constructor(param: {
        id,
        slot,
        deviceLocator,
        manufacturer,
        model,
        firmwareVersion,
        partNumber,
        workingState,
        capacity,
        dischargeTimes,
        usageDuration,
        SOC,
        SOH
    }) {
        this.id = param.id;
        this.slot = param.slot;
        this.deviceLocator = param.deviceLocator;
        this.manufacturer = param.manufacturer;
        this.model = param.model;
        this.firmwareVersion = param.firmwareVersion;
        this.partNumber = param.partNumber;
        this.workingState = param.workingState,
        this.capacity = param.capacity,
        this.dischargeTimes = param.dischargeTimes,
        this.usageDuration = param.usageDuration,
        this.SOC = param.SOC;
        this.SOH = param.SOH;
    }
}
