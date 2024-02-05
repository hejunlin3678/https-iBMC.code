import { FatherObj } from './fatherObj';

export class PSU extends FatherObj {
    private liveTime: [string, string];
    private firmwareVersion: [string, string];
    private slot: [string, number];
    private powerCapacityWatts: [string, string];
    private version: [string, string];
    private powerSupplyType: [string, string];
    private model: [string, string];
    private partNumber: [string, string];
    private serialNumber: [string, string];

    constructor(
        liveTime: string,
        firmwareVersion: string,
        slot: number,
        powerCapacityWatts: string,
        version: string,
        powerSupplyType: string,
        model: string,
        partNumber: string,
        serialNumber: string,
    ) {
        super();
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.firmwareVersion = ['COMMON_FIXED_VERSION', firmwareVersion];
        this.slot = ['FDM_SLOT', slot];
        this.powerCapacityWatts = ['FDM_RATED_POWER', powerCapacityWatts];
        this.version = ['FDM_ALARM_VENDOR', version];
        this.powerSupplyType = ['FDM_INPUT_MODE', powerSupplyType];
        this.model = ['FDM_TYPE', model];
        this.partNumber = ['OTHER_PART_NUMBER', partNumber];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
    }
}
