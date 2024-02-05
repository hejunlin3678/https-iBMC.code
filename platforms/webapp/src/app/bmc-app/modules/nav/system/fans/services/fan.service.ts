import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
@Injectable({
    providedIn: 'root'
})
export class FanService {
    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

    public intelligentUpdate(param) {
        return this.http.patch('/UI/Rest/System/Thermal', param);
    }

    public getThermals() {
        return this.http.get('/UI/Rest/System/Thermal');
    }

    public clearInletHistoryTemperature() {
       return this.http.post('/UI/Rest/System/Thermal/ClearInletHistory', {});
    }

    public getAirInletTemperature(data) {
        const history = data.Inlet.History;
        const inletTime: string[] = [];
        const inletTemp: number[] = [];
        history.forEach(element => {
            inletTime.push(element.Time.replace(' ', '\n'));
            inletTemp.push(element.InletTempCelsius);
        });
        const airInletInfo = {
            inletTemperature: (data.Inlet && data.Inlet.CurrentValue !== null
                && data.Inlet.CurrentValue !== undefined) ? data.Inlet.CurrentValue : '--',
            time: inletTime,
            inletTempCelsius: inletTemp,
        };
        return airInletInfo;
    }


    private intelligentInit(fanSmartCoolingMode: string) {
        let fanSmartCoolingModeText = '';
        if (fanSmartCoolingMode === 'EnergySaving') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE1');
        } else if (fanSmartCoolingMode === 'LowNoise') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE2');
        } else if (fanSmartCoolingMode === 'HighPerformance') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE3');
        } else if (fanSmartCoolingMode === 'LiquidCooling') {
            fanSmartCoolingModeText = this.translate.instant('FAN_LIQUID_COOLING');
        } else if (fanSmartCoolingMode === 'Custom') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE4');
        } else {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_NONE');
        }
        return fanSmartCoolingModeText;

    }

    public getIntelSpeedAdjustInfo(data: any) {
        const thermalInfo = data.SmartCooling;
        const customConfig = thermalInfo?.CustomConfig;
        let intelSpeedAdjust = null;
        if (customConfig) {
            intelSpeedAdjust = {
                fanSmartCoolingMode: thermalInfo.Mode,
                fanSmartCoolingModeText: this.intelligentInit(thermalInfo.Mode),
                coolingMedium: thermalInfo.CoolingMedium,
                cpuTargetTemperatureCelsius: customConfig.CPUTargetTemperatureCelsius,
                minCPUTargetTemperatureCelsius: customConfig.MinCPUTargetTemperatureCelsius,
                bcuInletTargetTemperatureCelsius: customConfig.SoCBoardInletTargetTemperatureCelsius,
                minBCUInletTargetTemperatureCelsius: customConfig.SoCBoardInletMinTargetTemperatureCelsius,
                maxBCUInletTargetTemperatureCelsius: customConfig.SoCBoardInletMaxTargetTemperatureCelsius,
                bcuOutletTargetTemperatureCelsius: customConfig.SoCBoardOutletTargetTemperatureCelsius,
                minBCUOutletTargetTemperatureCelsius: customConfig.SoCBoardOutletMinTargetTemperatureCelsius,
                maxBCUOutletTargetTemperatureCelsius: customConfig.SoCBoardOutletMaxTargetTemperatureCelsius,
                maxCPUTargetTemperatureCelsius: customConfig.MaxCPUTargetTemperatureCelsius,
                npuHbmTargetTemperatureCelsius: customConfig.NPUHbmTargetTemperatureCelsius,
                minHdmTargetTemperatureCelsius: customConfig.NPUHbmMinTargetTemperatureCelsius,
                maxHdmTargetTemperatureCelsius: customConfig.NPUHbmMaxTargetTemperatureCelsius,
                npuAiCoreTargetTemperatureCelsius: customConfig.NPUAiCoreTargetTemperatureCelsius,
                minAiTargetTemperatureCelsius: customConfig.NPUAiCoreMinTargetTemperatureCelsius,
                maxAiTargetTemperatureCelsius: customConfig.NPUAiCoreMaxTargetTemperatureCelsius,
                npuBoardTargetTemperatureCelsius: customConfig.NPUBoardTargetTemperatureCelsius,
                maxBoardTargetTemperatureCelsius: customConfig.NPUBoardMaxTargetTemperatureCelsius,
                minBoardTargetTemperatureCelsius: customConfig.NPUBoardMinTargetTemperatureCelsius,
                outletTargetTemperatureCelsius: customConfig.OutletTargetTemperatureCelsius,
                minOutletTargetTemperatureCelsius: customConfig.MinOutletTargetTemperatureCelsius,
                maxOutletTargetTemperatureCelsius: customConfig.MaxOutletTargetTemperatureCelsius,
                memoryTargetTemperatureCelsius : customConfig.MemoryTargetTemperatureCelsius,
                minMemoryTargetTemperatureCelsius: customConfig.MinMemoryTargetTemperatureCelsius,
                maxMemoryTargetTemperatureCelsius: customConfig.MaxMemoryTargetTemperatureCelsius,
                temperatureRangeCelsius: customConfig.TemperatureRangeCelsius || [],
                minTemperatureRangeCelsius: customConfig.MinTemperatureRangeCelsius,
                maxTemperatureRangeCelsius: customConfig.MaxTemperatureRangeCelsius,
                fanSpeedPercents: customConfig.FanSpeedPercents || [],
                minFanSpeedPercents: customConfig.MinFanSpeedPercents,
                maxFanSpeedPercents: customConfig.MaxFanSpeedPercents
            };
        } else {
            intelSpeedAdjust = false;
        }
        return intelSpeedAdjust;
    }

}
