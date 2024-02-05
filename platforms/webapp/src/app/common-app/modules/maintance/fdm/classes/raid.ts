import { FatherObj } from './fatherObj';

export class RAID extends FatherObj {

    private name: [string, string];
    private raidLevels: [string, string];
    private liveTime: [string, string];
    private pcbVersion: [string, string];
    private location: [string, string];
    private cpldVersion: [string, string];
    private version: [string, string];
    private boardId: [string, string];
    private cardNo: [string, string];
    private resourceOwner: [string, string];
    private description: [string, string];

    constructor(
        name: string,
        raidLevels: string,
        liveTime: string,
        pcbVersion: string,
        location: string,
        cpldVersion: string,
        version: string,
        boardId: string,
        cardNo: string,
        resourceOwner: string,
        description: string
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.raidLevels = ['FDM_ALARM_EVENT_LEVEL', raidLevels];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        this.location = ['FDM_LOCATION', location];
        this.cpldVersion = ['COMMIN_CPLD_VERSION', cpldVersion];
        this.version = ['FDM_ALARM_VENDOR', version];
        this.boardId = ['OTHER_BOARD_ID', boardId];
        this.cardNo = ['COMMON_NUMBER', cardNo];
        this.resourceOwner = ['COMMON_RESOURCE_OWNER', resourceOwner];
        this.description = ['FDM_ALARM_TYPE', description];
    }
}
