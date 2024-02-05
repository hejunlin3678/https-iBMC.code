import { FatherObj } from './fatherObj';


export class Fans extends FatherObj {
    private name: [string, string];
    private partNumber: [string, string];
    private onlineTime: [string, string];
    private speed: [string, string];
    private model: [string, string];
    private speedRatio: [string, string];

    constructor(
        name: string,
        partNumber: string,
        onlineTime: string,
        speed: string,
        model: string,
        speedRatio: string
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.partNumber = ['OTHER_PART_NUMBER', partNumber];
        this.onlineTime = ['FDM_ALARM_LIVE_TIME', onlineTime];
        this.speed = ['FDM_ROTATIONAL_SPEED', this.changeSpeed(speed)];
        this.model = ['FDM_ALARM_MODEL', model];
        this.speedRatio = ['FAN_HEAT_RATE', this.changeSpeedRatio(speedRatio)];
    }
    public changeSpeed(speed): string {
        let speed1 = '';
        if (!speed) {
            speed1 = '0';
        } else {
            speed1 = speed;
        }
        return speed1;
    }
    public changeSpeedRatio(speedRatio): string {
        let speedRatio1 = '';
        if (speedRatio > 100) {
            speedRatio1 = '100';
        } else {
            speedRatio1 = speedRatio;
        }
        return speedRatio1;
    }
}
