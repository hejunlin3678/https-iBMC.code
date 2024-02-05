export class PowerSleep {
    // 电源是否存在不健康
    public isPowerHealth: boolean;
    // 是否支持电源休眠设置,E9000,TCE产品差异
    public isPowerSleep: boolean;
    public sleepEnable: boolean;
    public sleepMode: string;
    constructor(param: PowerSleep) {
        const {
            isPowerHealth,
            isPowerSleep,
            sleepEnable,
            sleepMode
        } = param;
        this.isPowerHealth = isPowerHealth;
        this.isPowerSleep = isPowerSleep;
        this.sleepEnable = sleepEnable;
        this.sleepMode = sleepMode;
    }
}

