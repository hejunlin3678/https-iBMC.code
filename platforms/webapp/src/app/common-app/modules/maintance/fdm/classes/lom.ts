import { FatherObj } from './fatherObj';

export class LOM extends FatherObj {
    private name: [string, string];
    private model: [string, string];
    private liveTime: [string, string];
    private vendor: [string, string];
    private cardModel: [string, string];
    private pcbVersion: [string, string];
    private boardId: [string, string];
    private version: [string, string];

    constructor(
        name: string,
        model: string,
        liveTime: string,
        vendor: string,
        cardModel: string,
        pcbVersion: string,
        boardId: string,
        version: string,
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.model = ['NET_CHIP_MODEL', model];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.vendor = ['COMMON_MANUFACTURER', vendor];
        this.cardModel = ['FDM_ALARM_MODEL', cardModel];
        this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        this.boardId = ['OTHER_BOARD_ID', boardId];
        this.version = ['NET_CHIP_VENDOR', version];
    }
}
