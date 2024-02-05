export class PowerInfoCard {
    private name: string;
    private slotId: number;
    private powerInputWatts: number;
    private activeStandby: string;
    private serialNumber: string;
    private model: string;
    private manufacturer: string;
    private firmwareVersion: string;
    private partNumber: string;
    private powerCapacityWatts: number;
    private powerSupplyType: string;
    private lineInputVoltage: number;
    private outputVoltage: number;
    private isAbsent: boolean;

    constructor(param) {
        this.name = param.name;
        this.slotId = param.slotId;
        this.powerInputWatts = param.powerInputWatts;
        this.activeStandby = param.activeStandby;
        this.serialNumber = param.serialNumber;
        this.model = param.model;
        this.manufacturer = param.manufacturer;
        this.firmwareVersion = param.firmwareVersion;
        this.partNumber = param.partNumber;
        this.powerCapacityWatts = param.powerCapacityWatts;
        this.powerSupplyType = param.powerSupplyType;
        this.lineInputVoltage = param.lineInputVoltage;
        this.outputVoltage = param.outputVoltage;
        this.isAbsent = param.isAbsent;
    }


}
