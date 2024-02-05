import { IMonitorInfo, IMonitorParams } from '../home.datatype';
import { TranslateService } from '@ngx-translate/core';
import { GlobalData } from 'src/app/common-app/models/global-data.model';
import { WebStyle } from 'src/app/common-app/models';
import { getTempMonitorDefault, getPowerMonitorDefault, getRamMonitorDefault, getDiskMonitorDefault, getCPUMonitorDefault } from './monitor-default.util';
import { getTempMonitorBeautiful, getPowerMonitorBeautiful, getRamMonitorBeautiful, getDiskMonitorBeautiful, getCPUMonitorBeautiful } from './monitor-beautiful.util';
import { getTempMonitorSimple, getPowerMonitorSimple, getRamMonitorSimple, getDiskMonitorSimple, getCPUMonitorSimple } from './monitor-simple.util';
import { getTempMonitorStandard, getPowerMonitorStandard, getRamMonitorStandard, getDiskMonitorStandard, getCPUMonitorStandard } from './monitor-standard.util';
import { EChartsCoreOption } from 'echarts/types/dist/echarts';

export function getTempMonitorOption(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const inletTemperature = monitorInfo.inletTemperature;
    const clockwise = inletTemperature >= 0;
    const startAngle = 90;
    const lineHeight = 31;
    const tempData = Math.abs(inletTemperature) * 135 / 360;
    let inletTempMin = monitorInfo.inletTempMinorThreshold;
    let inletTempMax = monitorInfo.inletTempMajorThreshold;
    if (inletTempMin == null && inletTempMax != null) {
        inletTempMin = inletTempMax + 1;
    }
    if (inletTempMin != null && inletTempMin == null) {
        inletTempMax = inletTempMin - 1;
    }
    if (inletTempMin == null && inletTempMax == null) {
        inletTempMax = inletTemperature + 1;
        inletTempMin = inletTemperature + 1;
    }
    const params: IMonitorParams = {
        value: tempData,
        minorValue: inletTempMin,
        lineHeight,
        startAngle,
        clockwise,
        computedValue: inletTemperature,
        majorVal: inletTempMax,
        minVal: -100,
        maxVal: 100,
    };
    switch (GlobalData.getInstance().getWebStyle) {
        case WebStyle.Default:
            return getTempMonitorDefault(monitorInfo, translate, params);
        case WebStyle.Beautiful:
            return getTempMonitorBeautiful(monitorInfo, translate, params);
        case WebStyle.Simple:
            return getTempMonitorSimple(monitorInfo, translate, params);
        case WebStyle.Standard:
            return getTempMonitorStandard(monitorInfo, translate, params);
        default:
            return getTempMonitorDefault(monitorInfo, translate, params);
    }
}

export function getPowerMonitorOption(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    let limitPower = monitorInfo.upperPowerThresholdWatts;
    const powerConsumedWatts = monitorInfo.powerConsumedWatts;
    const powerCapacityWatts = monitorInfo.powerCapacityWatts;
    const clockwise = true;
    const lineHeight = 31;
    const startAngle = 225;
    if (limitPower == null && powerConsumedWatts != null) {
        limitPower = powerConsumedWatts + 1;
    }
    let power: number;
    if (powerCapacityWatts) {
        power = Math.ceil(powerConsumedWatts / powerCapacityWatts * 100) * 3 / 4;
    } else {
        power = 45;
    }
    const params: IMonitorParams = {
        value: power,
        minorValue: limitPower,
        lineHeight,
        startAngle,
        clockwise,
        computedValue: powerConsumedWatts,
        majorVal: limitPower,
        minVal: 0,
        maxVal: powerCapacityWatts,
    };
    switch (GlobalData.getInstance().getWebStyle) {
        case WebStyle.Default:
            return getPowerMonitorDefault(monitorInfo, translate, params);
        case WebStyle.Beautiful:
            return getPowerMonitorBeautiful(monitorInfo, translate, params);
        case WebStyle.Simple:
            return getPowerMonitorSimple(monitorInfo, translate, params);
        case WebStyle.Standard:
            return getPowerMonitorStandard(monitorInfo, translate, params);
        default:
            return getPowerMonitorDefault(monitorInfo, translate, params);
    }
}

export function getRamMonitorOption(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    switch (GlobalData.getInstance().getWebStyle) {
        case WebStyle.Default:
            return getRamMonitorDefault(monitorInfo, translate);
        case WebStyle.Beautiful:
            return getRamMonitorBeautiful(monitorInfo, translate);
        case WebStyle.Simple:
            return getRamMonitorSimple(monitorInfo, translate);
        case WebStyle.Standard:
            return getRamMonitorStandard(monitorInfo, translate);
        default:
            return getRamMonitorDefault(monitorInfo, translate);
    }
}

export function getCPUMonitorOption(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    switch (GlobalData.getInstance().getWebStyle) {
        case WebStyle.Default:
            return getCPUMonitorDefault(monitorInfo, translate);
        case WebStyle.Beautiful:
            return getCPUMonitorBeautiful(monitorInfo, translate);
        case WebStyle.Simple:
            return getCPUMonitorSimple(monitorInfo, translate);
        case WebStyle.Standard:
            return getCPUMonitorStandard(monitorInfo, translate);
        default:
            return getCPUMonitorDefault(monitorInfo, translate);
    }
}

export function getDiskMonitorOption(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    switch (GlobalData.getInstance().getWebStyle) {
        case WebStyle.Default:
            return getDiskMonitorDefault(monitorInfo, translate);
        case WebStyle.Beautiful:
            return getDiskMonitorBeautiful(monitorInfo, translate);
        case WebStyle.Simple:
            return getDiskMonitorSimple(monitorInfo, translate);
        case WebStyle.Standard:
            return getDiskMonitorStandard(monitorInfo, translate);
        default:
            return getDiskMonitorDefault(monitorInfo, translate);
    }
}
