export class PowerItem {
    id: string;
    powerInputWatts: string;
    isAbsent: string;
    isAbsentText: string;
    slot: string;
    manufacturer: string;
    model: string;
    serialNumber: string;
    firmwareVersion: string;
    partNumber: string;
    powerCapacityWatts: string;
    powerSupplyType: string;
    lineInputVoltage: string;
    outputVoltage: string;
    powerSupplyChannel: string;

    constructor(param: {
        id,
        powerInputWatts,
        isAbsent,
        isAbsentText,
        slot,
        manufacturer,
        model,
        serialNumber,
        firmwareVersion,
        partNumber,
        powerCapacityWatts,
        powerSupplyType,
        lineInputVoltage,
        outputVoltage,
        powerSupplyChannel
    }) {
        this.id = param.id;
        this.powerInputWatts = param.powerInputWatts;
        this.isAbsent = param.isAbsent;
        this.isAbsentText = param.isAbsentText;
        this.slot = param.slot;
        this.manufacturer = param.manufacturer;
        this.model = param.model;
        this.serialNumber = param.serialNumber;
        this.firmwareVersion = param.firmwareVersion;
        this.partNumber = param.partNumber;
        this.powerCapacityWatts = param.powerCapacityWatts,
        this.powerSupplyType = param.powerSupplyType,
        this.lineInputVoltage = param.lineInputVoltage,
        this.outputVoltage = param.outputVoltage,
        this.powerSupplyChannel = param.powerSupplyChannel;
    }
}
