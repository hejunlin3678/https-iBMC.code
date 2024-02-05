import { IMonitorInfo, IMonitorParams } from '../home.datatype';
import { TranslateService } from '@ngx-translate/core';
import { EChartsCoreOption } from 'echarts/types/dist/echarts';
import { MonitorBeautiful } from '../models/monitor-beautiful.model';

export function getTempMonitorBeautiful(monitorInfo: IMonitorInfo, translate: TranslateService, params: IMonitorParams):
    EChartsCoreOption {
    const inletTemperature = monitorInfo.inletTemperature;
    const formatter = `{div|\n\n${inletTemperature}}{span| ℃\n\n\n}`;
    const title = translate.instant('FAN_HEAT_TEMPERATURE_TITLE');
    params.formatter = formatter;
    params.title = title;
    return new MonitorBeautiful(params).gaugeOptions;
}

export function getPowerMonitorBeautiful(monitorInfo: IMonitorInfo, translate: TranslateService, params: IMonitorParams):
    EChartsCoreOption {
    const powerConsumedWatts = monitorInfo.powerConsumedWatts;
    const innerHeight = window.innerWidth;
    const title = translate.instant('HOME_POWERW');
    let formatter = `{div|\n\n${powerConsumedWatts}}{span| W\n\n\n}`;
    if (innerHeight < 1746 && (innerHeight < 1280 || powerConsumedWatts > 999)) {
        formatter = `{div|\n\n${powerConsumedWatts}\n}{span| W\n\n\n}`;
    }
    params.title = title;
    params.formatter = formatter;
    return new MonitorBeautiful(params).gaugeOptions;
}

export function getRamMonitorBeautiful(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitMemory = monitorInfo.memThresholdPercent == null ? 100 : monitorInfo.memThresholdPercent;
    const usagePercent = monitorInfo.memUseagePercent;
    const ramData = Number(usagePercent) || 0;
    const formatter = `{div|\n${ramData}}{p| %\n}`;
    const title = `${translate.instant('MONITOR_MEMORY_USAGE')}`;
    const params: IMonitorParams = {
        computedValue: ramData,
        value: ramData,
        formatter,
        title,
        minorValue: limitMemory
    };
    return new MonitorBeautiful(params).pieOptions;
}

export function getCPUMonitorBeautiful(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitCPU = monitorInfo.proThresholdPercent == null ? 100 : monitorInfo.proThresholdPercent;
    const cpuData = monitorInfo.proUseagePercent || 0;
    const formatter = `{div|\n${cpuData}}{p| %\n}`;
    const title = translate.instant('MONITOR_CPU_USAGE');
    const params: IMonitorParams = {
        computedValue: cpuData,
        value: cpuData,
        formatter,
        title,
        minorValue: limitCPU
    };
    return new MonitorBeautiful(params).pieOptions;
}

export function getDiskMonitorBeautiful(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitValDisk = monitorInfo.diskThresholdPercent == null ? 100 : monitorInfo.diskThresholdPercent;
    const diskData = monitorInfo.diskUsagePercent;
    const title = translate.instant('MONITOR_DISK_USAGE');
    const formatter = `{div|\n${diskData}}{p| %\n}`;
    const params: IMonitorParams = {
        computedValue: diskData,
        value: diskData,
        formatter,
        title,
        minorValue: limitValDisk
    };
    return new MonitorBeautiful(params).pieOptions;
}
