import { IEChartOption, IMonitorInfo } from '../home.datatype';
import { getDiskMonitorOption, getCPUMonitorOption, getRamMonitorOption, getPowerMonitorOption, getTempMonitorOption } from '../utils/monitor.util';
import { TranslateService } from '@ngx-translate/core';

export class EchartsOption {
    private echartOptions: IEChartOption[];

    constructor(monitorInfo: IMonitorInfo, showmointer: boolean, translate: TranslateService) {
        const echartOptions = [
            {
                id: 'homeDiskMonitor',
                option: getDiskMonitorOption(monitorInfo, translate),
                navigatorUrl: ['/navigate/system/monitor'],
                show: monitorInfo.diskUsagePercent !== undefined && showmointer,
                title: translate.instant('MONITOR_DISK_USAGE')
            },
            {
                id: 'homeCPUMonitor',
                option: getCPUMonitorOption(monitorInfo, translate),
                navigatorUrl: ['/navigate/system/monitor'],
                show: monitorInfo.proUseagePercent !== undefined && showmointer,
                title: translate.instant('MONITOR_CPU_USAGE')
            },
            {
                id: 'homeRamMonitor',
                option: getRamMonitorOption(monitorInfo, translate),
                navigatorUrl: ['/navigate/system/monitor'],
                show: monitorInfo.memUseagePercent !== undefined && showmointer,
                title: translate.instant('MONITOR_MEMORY_USAGE')
            },
            {
                id: 'homeTempMonitor',
                option: getTempMonitorOption(monitorInfo, translate),
                navigatorUrl: ['/navigate/system/fans'],
                show: !!monitorInfo.inletTemperature,
                title: translate.instant('FAN_HEAT_TEMPERATURE_TITLE')
            },
            {
                id: 'homePowerMonitor',
                option: getPowerMonitorOption(monitorInfo, translate),
                navigatorUrl: ['/navigate/system/power'],
                routerParam: '1',
                show: !!monitorInfo.powerConsumedWatts,
                title: translate.instant('HOME_POWERW')
            }
        ];
        this.echartOptions = echartOptions.filter((echartOption: IEChartOption) => echartOption.show);
    }

    get getEchartOption(): IEChartOption[] {
        return this.echartOptions;
    }
}
