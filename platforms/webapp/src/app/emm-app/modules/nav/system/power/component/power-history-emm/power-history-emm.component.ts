import { Component, OnInit } from '@angular/core';
import { TiButtonItem, TiMessageService, TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { GlobalData } from 'src/app/common-app/models';
import { UserInfoService, LoadingService, AlertMessageService, CommonService } from 'src/app/common-app/service';
import { formatEntry, htmlEncode } from 'src/app/common-app/utils';
import Date from 'src/app/common-app/utils/date';
import { PowerHistoryService } from './service/power-history.service';

@Component({
    selector: 'app-power-history-emm',
    templateUrl: './power-history-emm.component.html',
    styleUrls: ['./power-history-emm.component.scss']
})
export class PowerHistoryEmmComponent implements OnInit {

    constructor(
        private user: UserInfoService,
        private powerHistoryService: PowerHistoryService,
        private translate: TranslateService,
        private loading: LoadingService,
        private tiMessage: TiMessageService,
        private common: CommonService,
        private alert: AlertMessageService,
    ) { }

    public powerUnit: string = 'week';
    public isPrivileges = this.user.hasPrivileges(['OemPowerControl']);
    public isDown: boolean = false;
    public units = 'W';
    // 权限判断
    public isSystemLock = this.user.systemLockDownEnabled;
    public clearTip: string = this.translate.instant('COMMON_CLEAR');
    public powerUnits: TiButtonItem[] = [ {
        id: 'week',
        text: this.translate.instant('POWER_HISTORY_WEEK')
    }, {
        id: 'day',
        text: this.translate.instant('POWER_HISTORY_DAY')
    }];
    public showCharts: boolean = true;
    public isLowBrowserVersion: boolean = GlobalData.getInstance().getIsLowBrowserVer;
    public option = null;
    public alarmThresholdValue = '';
    public powerInfoList = [{
            label: this.translate.instant('POWER_MAX'),
            value: 0
        }, {
            label: this.translate.instant('POWER_MIN'),
            value: 0
        }, {
            label: this.translate.instant('POWER_AVG'),
            value: 0
        }
    ];
    public selected: TiButtonItem =  this.powerUnits[0];

    ngOnInit(): void {
        this.loading.state.next(true);
        this.init();
    }
    // 切换近一天和近一周数据
    public powerUnitsChange() {
        this.powerUnit = this.selected.id;
        this.loading.state.next(true);
        this.init(this.selected.id);
    }

    private init(days = 'week') {
        let day: number;
        day = days === 'week' ? 7 : 1;
        this.powerHistoryService.getPowerHistory().subscribe(resp => {
            const {chartDate, historyInfo} = this.powerHistoryService.getPowerHistoryInfo(resp.body?.History, day);
            this.powerInfoList[0].value = historyInfo.maximumPower;
            this.powerInfoList[1].value = historyInfo.minimumPower;
            this.powerInfoList[2].value = historyInfo.averagePower;
            if (chartDate.xData?.length === 0) {
                this.showCharts = false;
            } else {
                this.initCharts(chartDate.xData, chartDate.realData, chartDate.avgData);
            }
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }
    // 处理图表数据
    private initCharts(timeList, powerPeakWattsList, powerAverageWattsList) {
        this.option = {
            tooltip: {
                trigger: 'axis',
                axisPointer: {
                    lineStyle: {
                        color: '#999',
                        type: 'dashed'
                    }
                },
                backgroundColor: 'rgba(62, 66, 77, 0.8)',
                extraCssText: 'padding:8px 16px; border-radius:1px;',
                formatter(param) {
                    let res = '';
                    for (const item of param) {
                        res += `<span class="toolTipTime">${htmlEncode(item.seriesName)}: </span>
                                <span class="toolTipValue">${htmlEncode(item.data[1])}</span><br>`;
                    }
                    return `<span class="toolTipTime">${htmlEncode(new Date(parseInt(param[0].axisValue, 10)).pattern('yyyy/MM/dd HH:mm:ss'))}</span><br>${res}`;
                }
            },
            color: ['#6C92FA', '#48DDD0'],
            legend: {
                itemWidth: 10,
                data: [{
                    name: this.translate.instant('POWER_AVG'),
                    icon: 'circle'
                },
                {
                    name: this.translate.instant('POWER_HISTORY_ACTUAL'),
                    icon: 'circle'
                }],
                textStyle: {
                    padding: [4, 0, 0, 0]
                },
                left: 'center',
                itemGap: 32,
                bottom: -4
            },
            grid: {
                top: 12,
                left: 48,
                right: 32,
                bottom: 60
            },
            xAxis: {
                show: true,
                type: 'category',
                boundaryGap: false,
                interval: this.generateXLabels(timeList),
                axisLine: {
                    show: true,
                    lineStyle: {
                        width: 2,
                        type: 'solid',
                        color: '#E6EBF5'

                    }
                },
                axisTick: {
                    show: true,
                    length: 3,
                    lineStyle: {
                        width: 2,
                        color: '#E6EBF5'
                    }
                },
                axisLabel: {
                    margin: 15,
                    color: '#9EA4B3',
                    fontSize: 10,
                    formatter(item) {
                        if (timeList.length === 1) {
                            return item.replace(' ', '\n');
                        }
                        return ' ' + new Date(parseInt(item, 10)).pattern('yyyy/MM/dd HH:mm').replace(' ', '\n');
                    }
                },
                splitLine: {
                    show: false
                },
                data: this.timeArr(timeList)
            },
            yAxis: [{
                type: 'value',
                axisLine: {
                    show: false
                },
                splitLine: {
                    lineStyle: {
                        type: 'soild',
                        color: '#E6EBF5'
                    }
                },
                offset: 2,
                axisTick: {
                    show: false
                },
                axisLabel: {
                    margin: 8,
                    textStyle: {
                        align: 'right',
                        baseline: 'bottom',
                        color: '#9EA4B3',
                        fontSize: 12
                    }
                },
                splitNumber: 4
            }],
            // 平均功率
            series: [{
                name: this.translate.instant('POWER_AVG'),
                type: 'line',
                smooth: true,
                showSymbol: timeList.length === 1 ? true : false,
                symbolSize: 6,
                lineStyle: {
                    normal: {
                        width: 2,
                        color: '#6C92FA'
                    }
                },
                areaStyle: {
                    normal: {
                        color: {
                            type: 'linear',
                            x: 0,
                            y: 0,
                            x2: 0,
                            y2: 1,
                            colorStops: [{
                                offset: 0,
                                color: '#6C92FA'
                            },
                            {
                                offset: 1,
                                color: '#fff'
                            }
                            ]
                        },
                        opacity: 0.2
                    }
                },
                data: powerAverageWattsList.map((item, index, arr) => {
                    if (arr.length === 1) {
                        return item;
                    }
                    return [new Date(item[0]).getTime(), item[1]];
                }),
            },
            {
                // 峰值功率
                name: this.translate.instant('POWER_HISTORY_ACTUAL'),
                type: 'line',
                showSymbol: timeList.length === 1 ? true : false,
                symbolSize: 6,
                symbolRotate: -45,
                smooth: true,
                lineStyle: {
                    // 系列级个性化折线样式
                    normal: {
                        width: 2,
                        color: '#48DDD0'
                    }
                },
                data: powerPeakWattsList.map((item, index, arr) => {
                    if (arr.length === 1) {
                        return item;
                    }
                    return [new Date(item[0]).getTime(), item[1]];
                }),
            }
            ]
        };
    }

    private timeArr(timeArray) {
        let arr = [];
        if (timeArray.length === 1) {
            arr = timeArray;
        } else {
            arr = timeArray.map((item) => {
                const time = new Date(item);
                const _time = time.getTime() - (time.getMinutes() % 10) * 60 * 1000 - time.getSeconds() * 1000;
                return _time;
            });
        }
    }
    private generateXLabels(timeArray) {
        let interval = 0;
        const start = new Date(timeArray[timeArray.length - 1]).getTime();
        const end = new Date(timeArray[0]).getTime();
        const len = Math.ceil((start - end) / 600000);
        switch (true) {
            case len < 11:
                // 10分钟
                interval = 600 * 1000;
                return interval;
            case len < 16:
                // 15分钟
                interval = 900 * 1000;
                return interval;
            case len < 31:
                // 30分钟
                interval = 1800 * 1000;
                return interval;
            case len < 61:
                // 1个小时
                interval = 3600 * 1000;
                return interval;
            case len < 121:
                // 2个小时
                interval = 2 * 3600 * 1000;
                return interval;
            case len < 361:
                // 6个小时
                interval = 6 * 3600 * 1000;
                return interval;
            case len < 721:
                // 12个小时
                interval = 12 * 3600 * 1000;
                return interval;
            case len < 1009:
                // 24个小时
                interval = 24 * 3600 * 1000;
                return interval;
            default:
                interval = Math.ceil((start - end) / 9);
                return interval;
        }
    }
    // 删除历史功率
    public clearPowerHistory() {
        this.tiMessage.open({
            id: 'powerHistoryDialog',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: {},
            close: () => {
                this.powerHistoryService.clearHistoryData().subscribe((response) => {
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    setTimeout(() => {
                        this.loading.state.next(true);
                        this.init(this.selected.id);
                    }, 1000);
                });
            },
            dismiss: () => { }
        });
    }

    // 下载历史功率
    public downPowerHistory() {
        this.isDown = true;
        this.powerHistoryService.downLoadHistory().subscribe((response) => {
            const url = response.body.url;
            this.queryTask(url);
        }, () => {
            this.isDown = false;
        });
    }

    // 下载文件处理
    private queryTask(url) {
        this.powerHistoryService.queryTask(url).subscribe((response) => {
            const data = response.body;
            if (data) {
                if (data.prepare_progress === 100 && response.body.downloadurl) {
                    this.common.restDownloadFile(response.body.downloadurl);
                    this.isDown = false;
                } else {
                    setTimeout(() => {
                        this.queryTask(url);
                    }, 3000);
                }
            }
        }, () => {
            this.isDown = false;
        });
    }
}

