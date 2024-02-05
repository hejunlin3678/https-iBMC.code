export class PowerCapping {
    public powerMetrics: boolean;
    private powerConsumedWatts: number;
    private maxConsumedWatts: number;
    private powerLimitState: boolean;
    public isLimitState: boolean;
    public isPowerCapSet: boolean;
    private averageConsumedWatts: number;
    private currentCPUPowerWatts: number;
    private currentMemoryPowerWatts: number;
    private totalConsumedPowerkWh: number;
    public statisticsCollected: string;
    private maxConsumedOccurred: string;
    private limitInWatts: number;
    private limitException: boolean;
    private minPowerLimitInWatts: number;
    private maxPowerLimitInWatts: number;
    private highPowerThresholdWatts: number;
    public powerCapMode: boolean;

    constructor(param) {
        this.powerMetrics = param.powerMetrics;
        this.powerConsumedWatts = param.powerConsumedWatts;
        this.maxConsumedWatts = param.maxConsumedWatts;
        this.powerLimitState = param.powerLimitState;
        this.isLimitState = param.isLimitState;
        this.isPowerCapSet = param.isPowerCapSet;
        this.averageConsumedWatts = param.averageConsumedWatts;
        this.limitInWatts = param.limitInWatts;
        this.currentCPUPowerWatts = param.currentCPUPowerWatts;
        this.currentMemoryPowerWatts = param.currentMemoryPowerWatts;
        this.totalConsumedPowerkWh = param.totalConsumedPowerkWh;
        this.limitException = param.limitException;
        this.statisticsCollected = param.statisticsCollected;
        this.maxConsumedOccurred = param.maxConsumedOccurred;
        this.minPowerLimitInWatts = param.minPowerLimitInWatts;
        this.maxPowerLimitInWatts = param.maxPowerLimitInWatts;
        this.highPowerThresholdWatts = param.highPowerThresholdWatts;
        this.powerCapMode = param.powerCapMode;
    }


    public getPowerConsumedWatts() {
        return this.powerConsumedWatts;
    }

    public setPowerConsumedWatts(value: number) {
        this.powerConsumedWatts = value;
    }

    public getMaxConsumedWatts() {
        return this.maxConsumedWatts;
    }

    public setMaxConsumedWatts(value: number) {
        this.maxConsumedWatts = value;
    }

    public getPowerLimitState() {
        return this.powerLimitState;
    }

    public setPowerLimitState(value: boolean) {
        this.powerLimitState = value;
    }

    public getAverageConsumedWatts() {
        return this.averageConsumedWatts;
    }

    public setAverageConsumedWatts(value: number) {
        this.averageConsumedWatts = value;
    }

    public getLimitInWatts() {
        return this.limitInWatts;
    }

    public setLimitInWatts(value: number) {
        this.limitInWatts = value;
    }

    public getCurrentCPUPowerWatts() {
        return this.currentCPUPowerWatts;
    }

    public setCurrentCPUPowerWatts(value: number) {
        this.currentCPUPowerWatts = value;
    }

    public getCurrentMemoryPowerWatts() {
        return this.currentMemoryPowerWatts;
    }

    public setCurrentMemoryPowerWatts(value: number) {
        this.currentMemoryPowerWatts = value;
    }

    public getTotalConsumedPowerkWh() {
        return this.totalConsumedPowerkWh;
    }

    public setTotalConsumedPowerkWh(value: number) {
        this.totalConsumedPowerkWh = value;
    }

    public getLimitException() {
        return this.limitException;
    }

    public setLimitException(value: boolean) {
        this.limitException = value;
    }

    public getStatisticsCollected() {
        return this.statisticsCollected;
    }

    public setStatisticsCollected(value: string) {
        this.statisticsCollected = value;
    }

    public getMaxConsumedOccurred() {
        return this.maxConsumedOccurred;
    }

    public setMaxConsumedOccurred(value: string) {
        this.maxConsumedOccurred = value;
    }

    public getMinPowerLimitInWatts() {
        return this.minPowerLimitInWatts;
    }

    public setMinPowerLimitInWatts(value: number) {
        this.minPowerLimitInWatts = value;
    }

    public getMaxPowerLimitInWatts() {
        return this.maxPowerLimitInWatts;
    }

    public setMaxPowerLimitInWatts(value: number) {
        this.maxPowerLimitInWatts = value;
    }

    public getHighPowerThresholdWatts() {
        return this.highPowerThresholdWatts;
    }
    public setHighPowerThresholdWatts(value: number) {
        this.highPowerThresholdWatts = value;
    }
}

