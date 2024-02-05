export class IntelSpeedAdjust {

    public fanSmartCoolingModeText: string;
    public fanSpeedCustom: boolean;
    public cpuTargetTemperatureCelsius: number;
    public npuHbmTargetTemperatureCelsius: number;
    public npuAiCoreTargetTemperatureCelsius: number;
    public outletTargetTemperatureCelsius: number;
    public temperatureRangeCelsius: number[];
    public fanSpeedPercents: number[];
    private fanSmartCoolingMode: string;
    private coolingMedium: string;
    private minCPUTargetTemperatureCelsius: number;
    private maxCPUTargetTemperatureCelsius: number;
    private minHdmTargetTemperatureCelsius: number;
    private maxHdmTargetTemperatureCelsius: number;
    private minAiTargetTemperatureCelsius: number;
    private maxAiTargetTemperatureCelsius: number;
    private minOutletTargetTemperatureCelsius: number;
    private maxOutletTargetTemperatureCelsius: number;
    private minTemperatureRangeCelsius: number;
    private maxTemperatureRangeCelsius: number;
    private minFanSpeedPercents: number;
    private maxFanSpeedPercents: number;

    constructor() { }

    public getFanSmartCoolingMode () {
        return this.fanSmartCoolingMode;
    }

    public setFanSmartCoolingMode(value: string) {
        this.fanSmartCoolingMode = value;
    }

    public getFanSmartCoolingModeText () {
        return this.fanSmartCoolingModeText;
    }

    public setFanSmartCoolingModeText (value: string) {
        this.fanSmartCoolingModeText = value;
    }

    public getCoolingMedium () {
        return this.coolingMedium;
    }

    public setCoolingMedium(value: string) {
        this.coolingMedium = value;
    }

    public getFanSpeedCustom() {
        return this.fanSpeedCustom;
    }

    public setFanSpeedCustom(value: boolean) {
        this.fanSpeedCustom = value;
    }

    public getCPUTargetTemperatureCelsius () {
        return this.cpuTargetTemperatureCelsius;
    }

    public setCPUTargetTemperatureCelsius(value: number) {
        this.cpuTargetTemperatureCelsius = value;
    }

    public getMinCPUTargetTemperatureCelsius () {
        return this.minCPUTargetTemperatureCelsius;
    }

    public setMinCPUTargetTemperatureCelsius(value: number) {
        this.minCPUTargetTemperatureCelsius = value;
    }

    public getMaxCPUTargetTemperatureCelsius () {
        return this.maxCPUTargetTemperatureCelsius;
    }

    public setMaxCPUTargetTemperatureCelsius(value: number) {
        this.maxCPUTargetTemperatureCelsius = value;
    }

    public getHbmTargetTemperatureCelsius () {
        return this.npuHbmTargetTemperatureCelsius;
    }

    public setHbmTargetTemperatureCelsius(value: number) {
        this.npuHbmTargetTemperatureCelsius = value;
    }

    public getMinHbmTargetTemperatureCelsius () {
        return this.minHdmTargetTemperatureCelsius;
    }

    public setMinHbmTargetTemperatureCelsius(value: number) {
        this.minHdmTargetTemperatureCelsius = value;
    }

    public getMaxHbmTargetTemperatureCelsius () {
        return this.maxHdmTargetTemperatureCelsius;
    }

    public setMaxHbmTargetTemperatureCelsius(value: number) {
        this.maxHdmTargetTemperatureCelsius = value;
    }

    public getAiTargetTemperatureCelsius () {
        return this.npuAiCoreTargetTemperatureCelsius;
    }

    public setAiTargetTemperatureCelsius(value: number) {
        this.npuAiCoreTargetTemperatureCelsius = value;
    }

    public getMinAiTargetTemperatureCelsius () {
        return this.minAiTargetTemperatureCelsius;
    }

    public setMinAiTargetTemperatureCelsius(value: number) {
        this.minAiTargetTemperatureCelsius = value;
    }

    public getMaxAiTargetTemperatureCelsius () {
        return this.maxAiTargetTemperatureCelsius;
    }

    public setMaxAiTargetTemperatureCelsius(value: number) {
        this.maxAiTargetTemperatureCelsius = value;
    }

    public getOutletTargetTemperatureCelsius () {
        return this.outletTargetTemperatureCelsius;
    }

    public setOutletTargetTemperatureCelsius(value: number) {
        this.outletTargetTemperatureCelsius = value;
    }

    public getMinOutletTargetTemperatureCelsius () {
        return this.minOutletTargetTemperatureCelsius;
    }

    public setMinOutletTargetTemperatureCelsius(value: number) {
        this.minOutletTargetTemperatureCelsius = value;
    }

    public getMaxOutletTargetTemperatureCelsius () {
        return this.maxOutletTargetTemperatureCelsius;
    }

    public setMaxOutletTargetTemperatureCelsius(value: number) {
        this.maxOutletTargetTemperatureCelsius = value;
    }

    public getTemperatureRangeCelsius () {
        return this.temperatureRangeCelsius;
    }

    public setTemperatureRangeCelsius(value: number[]) {
        this.temperatureRangeCelsius = value;
    }

    public getFanSpeedPercents () {
        return this.fanSpeedPercents;
    }

    public setFanSpeedPercents(value: number[]) {
        this.fanSpeedPercents = value;
    }

    public getMinTemperatureRangeCelsius () {
        return this.minTemperatureRangeCelsius;
    }

    public setMinTemperatureRangeCelsius(value: number) {
        this.minTemperatureRangeCelsius = value;
    }

    public getMaxTemperatureRangeCelsius () {
        return this.maxTemperatureRangeCelsius;
    }

    public setMaxTemperatureRangeCelsius(value: number) {
        this.maxTemperatureRangeCelsius = value;
    }

    public getMinFanSpeedPercents() {
        return this.minFanSpeedPercents;
    }

    public setMinFanSpeedPercents(value: number) {
        this.minFanSpeedPercents = value;
    }

    public getMaxFanSpeedPercents() {
        return this.maxFanSpeedPercents;
    }

    public setMaxFanSpeedPercents(value: number) {
        this.maxFanSpeedPercents = value;
    }

}
