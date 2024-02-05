import { FatherObj } from './fatherObj';

export class DiskBP extends FatherObj {

    private liveTime: [string, string];
    private type: [string, string];
    private location: [string, string];
    private pcbVersion: [string, string];
    private version: [string, string];
    private cpldVersion: [string, string];
    private fdmNumber: [string, string];
    private boardId: [string, string];

    constructor(
        liveTime: string,
        type: string,
        location: string,
        pcbVersion: string,
        version: string,
        cpldVersion: string,
        fdmNumber: string,
        boardId: string,
    ) {
        super();
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.type = ['FDM_ALARM_TYPE', type];
        this.location = ['FDM_LOCATION', location];
        this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        this.version = ['FDM_ALARM_VENDOR', version];
        this.cpldVersion = ['COMMIN_CPLD_VERSION', cpldVersion];
        this.fdmNumber = ['COMMON_NUMBER', fdmNumber];
        this.boardId = ['OTHER_BOARD_ID', boardId];
    }

}
