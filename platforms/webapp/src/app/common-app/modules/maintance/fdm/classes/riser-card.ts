import { FatherObj } from './fatherObj';

export class RiserCard extends FatherObj {

    private name: [string, string];
    private description: [string, string];
    private liveTime: [string, string];
    private pcbVersion: [string, string];
    private version: [string, string];
    private boardId: [string, string];
    private cardNo: [string, string];

    constructor(
        name: string,
        description: string,
        liveTime: string,
        pcbVersion: string,
        version: string,
        boardId: string,
        cardNo: string,
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.description = ['FDM_ALARM_TYPE', description];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        this.version = ['FDM_ALARM_VENDOR', version];
        this.boardId = ['OTHER_BOARD_ID', boardId];
        this.cardNo = ['FDM_SLOT', cardNo];
    }
}
