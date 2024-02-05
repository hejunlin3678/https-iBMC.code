import { IEChartOption, IMonitorInfo } from '../home.datatype';
import { getDiskMonitorOption, getCPUMonitorOption, getRamMonitorOption, getPowerMonitorOption, getTempMonitorOption } from '../utils/monitor.util';
import { TranslateService } from '@ngx-translate/core';

export class EchartsOption {
    private echartOptions: IEChartOption[];

    constructor(monitorInfo: IMonitorInfo, showmointer: boolean, translate: TranslateService) {
        const echartOptions = [
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
