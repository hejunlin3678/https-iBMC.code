import { IMonitorInfo, IMonitorParams } from '../home.datatype';
import { TranslateService } from '@ngx-translate/core';
import { EChartsCoreOption } from 'echarts/types/dist/echarts';
import { MonitorSimple } from '../models/monitor-simple.model';

export function getTempMonitorSimple(monitorInfo: IMonitorInfo, translate: TranslateService, params: IMonitorParams): EChartsCoreOption {
    const formatter = `${translate.instant('FAN_HEAT_TEMPERATURE_TITLE')}(?)`;
    params.formatter = formatter;
    params.isSimpleTemp = true;
    return new MonitorSimple(params).gaugeOptions;
}

export function getPowerMonitorSimple(monitorInfo: IMonitorInfo, translate: TranslateService, params: IMonitorParams): EChartsCoreOption {
    const powerConsumedWatts = monitorInfo.powerConsumedWatts;
    const innerHeight = window.innerWidth;
    let formatter = `${translate.instant('HOME_POWERW')}(W)`;
    if (innerHeight < 1746 && (innerHeight < 1280 || powerConsumedWatts > 999)) {
        formatter = `${translate.instant('HOME_POWERW')}(W)`;
    }
    params.formatter = formatter;
    return new MonitorSimple(params).gaugeOptions;
}

export function getRamMonitorSimple(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitMemory = monitorInfo.memThresholdPercent == null ? 100 : monitorInfo.memThresholdPercent;
    const usagePercent = monitorInfo.memUseagePercent;
    const ramData = Number(usagePercent) || 0;
    const formatter = `${translate.instant('MONITOR_MEMORY_USAGE')}`;
    const params: IMonitorParams = {
        computedValue: ramData,
        value: ramData,
        formatter,
        minorValue: limitMemory
    };
    return new MonitorSimple(params).pieOptions;
}

export function getCPUMonitorSimple(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitCPU = monitorInfo.proThresholdPercent == null ? 100 : monitorInfo.proThresholdPercent;
    const cpuData = monitorInfo.proUseagePercent || 0;
    const formatter = `${translate.instant('MONITOR_CPU_USAGE')}`;
    const params: IMonitorParams = {
        computedValue: cpuData,
        value: cpuData,
        formatter,
        minorValue: limitCPU
    };
    return new MonitorSimple(params).pieOptions;
}

export function getDiskMonitorSimple(monitorInfo: IMonitorInfo, translate: TranslateService): EChartsCoreOption {
    const limitValDisk = monitorInfo.diskThresholdPercent == null ? 100 : monitorInfo.diskThresholdPercent;
    const diskData = monitorInfo.diskUsagePercent;

    const formatter = `${translate.instant('MONITOR_DISK_USAGE')}`;
    const params: IMonitorParams = {
        computedValue: diskData,
        value: diskData,
        formatter,
        minorValue: limitValDisk
    };
    return new MonitorSimple(params).pieOptions;
}
