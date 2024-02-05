import { PowerInfoCard } from './power-info-card';
import { ICheckList } from '../interface';

export class Power {

    public powerInfoList: PowerInfoCard[];
    public normalAndRedundancy: boolean;
    private isActiveAndStandby: string;
    private activePSU: ICheckList[];
    private deepSleep: boolean;

    public getIsActiveAndStandby() {
        return this.isActiveAndStandby;
    }

    public setIsActiveAndStandby(value: string) {
        this.isActiveAndStandby = value;
    }

    public getAactivePSU() {
        return this.activePSU;
    }

    public setAactivePSU(value: ICheckList[]) {
        this.activePSU = value;
    }

    public getDeepSleep() {
        return this.deepSleep;
    }

    public setDeepSleep(value: boolean) {
        this.deepSleep = value;
    }
}
