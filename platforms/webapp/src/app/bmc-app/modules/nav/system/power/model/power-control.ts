export class PowerControl {

    private chassisType: string;
    public powerState: string;
    private powerOnStrategy: string;
    private powerRestoreDelayMode: string;
    private powerRestoreDelaySeconds: number;
    private safePowerOffTimoutSeconds: number;
    private wakeOnLANStatus: boolean;
    private panelPowerButtonEnabled: boolean;
    private safePowerOffMaxTimeoutSeconds: number;
    private safePowerOffMinTimeoutSeconds: number;
    private safePowerOffDefaultTimeoutSeconds: number;
    public safePowerOffTimeoutEnabled: boolean;
    public powerOffDefaultSeconds: number;
    public leakDetectionSupport: boolean;
    public leakStrategy: string;

    constructor(param) {
        this.powerState = param.powerState;
        this.powerOnStrategy = param.powerOnStrategy;
        this.powerRestoreDelayMode = param.powerRestoreDelayMode;
        this.powerRestoreDelaySeconds = param.powerRestoreDelaySeconds;
        this.safePowerOffTimoutSeconds = param.safePowerOffTimoutSeconds;
        this.wakeOnLANStatus = param.wakeOnLANStatus;
        this.panelPowerButtonEnabled = param.panelPowerButtonEnabled;
        this.safePowerOffMaxTimeoutSeconds = param.safePowerOffMaxTimeoutSeconds;
        this.safePowerOffMinTimeoutSeconds = param.safePowerOffMinTimeoutSeconds;
        this.safePowerOffDefaultTimeoutSeconds = param.safePowerOffDefaultTimeoutSeconds;
        this.safePowerOffTimeoutEnabled = param.safePowerOffTimeoutEnabled;
        this.powerOffDefaultSeconds = param.powerOffDefaultSeconds;
        this.leakDetectionSupport = param.leakDetectionSupport;
        this.leakStrategy = param.leakStrategy;

    }

    public getChassisType() {
        return this.chassisType;
    }

    public setChassisType(value: string) {
        this.chassisType = value;
    }

    public getPowerState() {
        return this.powerState;
    }

    public setPowerState(value: string) {
        this.powerState = value;
    }

    public getPowerOnStrategy() {
        return this.powerOnStrategy;
    }

    public setPowerOnStrategy(value: string) {
        this.powerOnStrategy = value;
    }

    public getPowerRestoreDelayMode() {
        return this.powerRestoreDelayMode;
    }

    public setPowerRestoreDelayMode(value: string) {
        this.powerRestoreDelayMode = value;
    }

    public getPowerRestoreDelaySeconds() {
        return this.powerRestoreDelaySeconds;
    }

    public setPowerRestoreDelaySeconds(value: number) {
        this.powerRestoreDelaySeconds = value;
    }

    public getSafePowerOffTimoutSeconds() {
        return this.safePowerOffTimoutSeconds;
    }

    public setSafePowerOffTimoutSeconds(value: number) {
        this.safePowerOffTimoutSeconds = value;
    }

    public getWakeOnLANStatus() {
        return this.wakeOnLANStatus;
    }

    public setWakeOnLANStatus(value: boolean) {
        this.wakeOnLANStatus = value;
    }

    public getPanelPowerButtonEnabled() {
        return this.panelPowerButtonEnabled;
    }

    public setPanelPowerButtonEnabled(value: boolean) {
        this.panelPowerButtonEnabled = value;
    }

    public getSafePowerOffMaxTimeoutSeconds() {
        return this.safePowerOffMaxTimeoutSeconds;
    }

    public setSafePowerOffMaxTimeoutSeconds(value: number) {
        this.safePowerOffMaxTimeoutSeconds = value;
    }

    public getSafePowerOffMinTimeoutSeconds() {
        return this.safePowerOffMinTimeoutSeconds;
    }

    public setSafePowerOffMinTimeoutSeconds(value: number) {
        this.safePowerOffMinTimeoutSeconds = value;
    }

    public getSafePowerOffDefaultTimeoutSeconds() {
        return this.safePowerOffDefaultTimeoutSeconds;
    }

    public setSafePowerOffDefaultTimeoutSeconds(value: number) {
        this.safePowerOffDefaultTimeoutSeconds = value;
    }

    public getSafePowerOffTimeoutEnabled() {
        return this.safePowerOffTimeoutEnabled;
    }

    public setSafePowerOffTimeoutEnabled(value: boolean) {
        this.safePowerOffTimeoutEnabled = value;
    }

    public getLeakDetectionSupport() {
        return this.leakDetectionSupport;
    }

    public setLeakDetectionSupport(value: boolean) {
        this.leakDetectionSupport = value;
    }

    public getLeakStrategy() {
        return this.leakStrategy;
    }

    public setLeakStrategy(value: string) {
        this.leakStrategy = value;
    }

}
