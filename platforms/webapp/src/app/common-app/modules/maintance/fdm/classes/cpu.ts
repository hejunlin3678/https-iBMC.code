import { FatherObj } from './fatherObj';

export class CPU extends FatherObj {
    private name: [string, string];
    private frequencyMHz: [string, string];
    private liveTime: [string, string];
    private status: [string, string];
    private version: [string, string];
    private totalCores: [string, string];
    private mode: [string, string];
    private cacheKiB: [string, string];
    private processorID: [string, string];
    private partNumber: [string, string];
    private otherParameters: [string, string];
    private serialNumber: [string, string];

    constructor(
        name: string,
        frequencyMHz: string,
        liveTime: string,
        status: string,
        version: string,
        totalCores: string,
        mode: string,
        cacheKiB: string,
        processorID: string,
        partNumber: string,
        otherParameters: string,
        serialNumber: string
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.frequencyMHz = ['FDM_ALARM_MAIN_FREQUENCY', frequencyMHz];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.status = ['FDM_ALARM_EVENT_STATUS', status];
        this.version = ['COMMON_MANUFACTURER', version];
        this.totalCores = ['FDM_CORECOUNT_THREADCOUNT', totalCores];
        this.mode = ['FDM_ALARM_MODEL', mode];
        this.cacheKiB = ['FDM_ALARM_L1L2L3_CACHE', cacheKiB];
        this.processorID = ['FDM_ALARM_PROCESSOR_ID', processorID];
        this.partNumber = ['OTHER_PART_NUMBER', partNumber];
        this.otherParameters = ['FDM_ALARM_OTHER_PARAMETERS', otherParameters];
        this.serialNumber = ['PPIN', serialNumber];
    }
}
