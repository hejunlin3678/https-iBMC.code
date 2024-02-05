import { FatherObj } from './fatherObj';

export class Chassis extends FatherObj {

    private name: [string, string];
    private liveTime: [string, string];

    constructor(liveTime: string) {
        super();
        this.name = ['COMMON_NAME', 'chassis'];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
    }
}
