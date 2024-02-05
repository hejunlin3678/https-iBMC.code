import { Blade } from './blade';

export class Chassis {
    public enable: boolean;
    public actualPower: number | string;
    public capStatus: string;
    public capValue: number;
    public maxCapValue: number;
    public minCapValue: number;
    public recommendMinCapValue: number;
    public capThreshold: string;
    public capMode: string;
    public bladeList: Blade[];
    constructor(param: Chassis) {
        const {
            enable,
            actualPower,
            capStatus,
            capValue,
            maxCapValue,
            minCapValue,
            recommendMinCapValue,
            capThreshold,
            capMode,
            bladeList
        } = param;
        this.enable = enable;
        this.actualPower = actualPower;
        this.capStatus = capStatus;
        this.capValue = capValue;
        this.maxCapValue = maxCapValue;
        this.minCapValue = minCapValue;
        this.recommendMinCapValue = recommendMinCapValue;
        this.capThreshold = capThreshold;
        this.capMode = capMode;
        this.bladeList = bladeList;
    }
}
