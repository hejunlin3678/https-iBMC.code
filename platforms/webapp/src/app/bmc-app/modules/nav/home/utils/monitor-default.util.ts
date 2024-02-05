import { IMonitorInfo, IMonitorParams } from '../home.datatype';
import { TranslateService } from '@ngx-translate/core';
import { EChartsCoreOption } from 'echarts/types/dist/echarts';
import { MonitorDefault } from '../models/monitor-default.model';
import { hideFansAndPower } from 'src/app/common-app/utils/fan.util';

export function getTempMonitorDefault(monitorInfo: IMonitorInfo, translate: TranslateService, params: IMonitorParams): EChartsCoreOption {
    const inletTemperature = monitorInfo.inletTemperature;
    const formatter = `{div|\n\n${inletTemperature}}{span| ℃\n\n\n${translate.instant('HOME_INLET_TEMPERATURE')}}`;
    params.formatter = formatter;
    return new MonitorDefault(params).gaugeOptions;
}

export function getPowerMonitorDefault(monitorInfo: IMonitorInfo, translate: TranslateService, params: IMonitorParams): EChartsCoreOption {
    const powerConsumedWatts = monitorInfo.powerConsumedWatts;
    const innerHeight = window.innerWidth;
    let formatter = `{div|\n\n${powerConsumedWatts}}{span| W\n\n\n${hideFansAndPower() ? translate.instant('HOME_POWER') : translate.instant('HOME_POWERW')}\n}`;
    if (innerHeight < 1746 && (innerHeight < 1280 || powerConsumedWatts > 999)) {
        formatter = `{div|\n\n${powerConsumedWatts}\n}{span| W\n\n\n${hideFansAndPower() ? translate.instant('HOME_POWER') : translate.instant('HOME_POWERW')}\n}`;
    }
    params.formatter = formatter;
    return new MonitorDefault(params).gaugeOptions;
}

export function getRamMonitorDefault(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitMemory = monitorInfo.memThresholdPercent == null ? 100 : monitorInfo.memThresholdPercent;
    const usagePercent = monitorInfo.memUseagePercent;
    const ramData = Number(usagePercent) || 0;
    const formatter = `{div|\n${ramData}}{p| %\n}{span|${translate.instant('HOME_MEMORY_OCCUPANCY_RATE')}\n\n}`;
    const params: IMonitorParams = {
        computedValue: ramData,
        value: ramData,
        formatter,
        minorValue: limitMemory
    };
    return new MonitorDefault(params).pieOptions;
}

export function getCPUMonitorDefault(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitCPU = monitorInfo.proThresholdPercent == null ? 100 : monitorInfo.proThresholdPercent;
    const cpuData = monitorInfo.proUseagePercent || 0;
    const formatter = `{div|\n${cpuData}}{p| %\n}{span|${translate.instant('HOME_CPU_USAGE')}\n\n}`;
    const params: IMonitorParams = {
        computedValue: cpuData,
        value: cpuData,
        formatter,
        minorValue: limitCPU
    };
    return new MonitorDefault(params).pieOptions;
}

export function getDiskMonitorDefault(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitValDisk = monitorInfo.diskThresholdPercent == null ? 100 : monitorInfo.diskThresholdPercent;
    const diskData = monitorInfo.diskUsagePercent;

    const formatter = `{div|\n${diskData}}{p| %\n}{span|${translate.instant('HOME_DISK_USAGE')}\n\n}`;
    const params: IMonitorParams = {
        computedValue: diskData,
        value: diskData,
        formatter,
        minorValue: limitValDisk
    };
    return new MonitorDefault(params).pieOptions;
}
