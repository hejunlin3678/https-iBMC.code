import { FatherObj } from './fatherObj';

export class DIMM extends FatherObj {
    private name: [string, string];
    private serialNumber: [string, string];
    private liveTime: [string, string];
    private deviceType: [string, string];
    private version: [string, string];
    private voltage: [string, string];
    private location: [string, string];
    private partNumber: [string, string];
    private capacity: [string, string];
    private dataWidthBits: [string, string];
    private mainFrequency: [string, string];
    private technology: [string, string];
    private rankCount: [string, string];

    constructor(
        name: string,
        serialNumber: string,
        liveTime: string,
        deviceType: string,
        version: string,
        voltage: string,
        location: string,
        partNumber: string,
        capacity: string,
        dataWidthBits: string,
        mainFrequency: string,
        technology: string,
        rankCount: string,
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.deviceType = ['FDM_ALARM_TYPE', deviceType];
        this.version = ['FDM_ALARM_VENDOR', version];
        this.voltage = ['FDM_MIN_VOLTAGE', voltage];
        this.location = ['FDM_LOCATION', location];
        this.partNumber = ['OTHER_PART_NUMBER', partNumber];
        this.capacity = ['COMMON_CAPACITY', capacity];
        this.dataWidthBits = ['FDM_DATAWIDTH_BITS', dataWidthBits];
        this.mainFrequency = ['FDM_ALARM_MAIN_FREQUENCY', mainFrequency];
        this.technology = ['FDM_TECHNOLOGY', technology];
        this.rankCount = ['FDM_RANK_COL', rankCount];
    }
}
