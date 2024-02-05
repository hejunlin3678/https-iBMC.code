import { Component, OnInit, Input, OnChanges } from '@angular/core';
import { PowerCapping } from '../../model/power-capping';
import { TiDateValue, TiDatetimeFormat, TiMessageService, TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { PowerService } from '../../service/power.service';
import { PowerCappingHistory } from '../../model/power-capping-history';
import { TranslateService } from '@ngx-translate/core';
import { W_TO_BTU_COEFF } from '../../interface';
import { AlertMessageService, CommonService, UserInfoService, SystemLockService } from 'src/app/common-app/service';
import { GlobalData } from 'src/app/common-app/models';
import { htmlEncode, formatEntry } from 'src/app/common-app/utils';
import { PowerVaild } from './valid/power.valid';
import Date from 'src/app/common-app/utils/date';

@Component({
    selector: 'app-power-history',
    templateUrl: './power-history.component.html',
    styleUrls: ['./power-history.component.scss']
})
export class PowerHistoryComponent implements OnInit, OnChanges {

    constructor(
        private service: PowerService,
        private alert: AlertMessageService,
        private common: CommonService,
        private tiMessage: TiMessageService,
        private user: UserInfoService,
        private translate: TranslateService,
        private lockService: SystemLockService

    ) { }

    @Input() isPrivileges: boolean;
    @Input() powerCappingInfo: PowerCapping;
    @Input() powerHistory: PowerCappingHistory;
    @Input() powerUnit: string;
    @Input() timeLists: string[];
    @Input() powerPeakWattsList: any[];
    @Input() powerAverageWattsList: any[];
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public showCharts: boolean = true;
    public isLowBrowserVersion: boolean = GlobalData.getInstance().getIsLowBrowserVer;
    public timeList: string[] = [];
    public localtion: string = 'right';
    public units: string = 'W';
    public totalUnits: string = 'kWh';
    public generationTime: string = '--';
    public clearTip: string = this.user.systemLockDownEnabled ? '' : this.translate.instant('COMMON_CLEAR');
    public downTip: string = this.translate.instant('COMMON_DOWN');
    public isDown: boolean = false;
    public powerHistoryModal;
    public searchDataFormat: TiDatetimeFormat = {
        date: 'yyyy.MM.dd',
        time: 'HH:mm'
    };
    public searchDataValue: TiDateValue = {
        begin: null,
        end: null
    };
    public searchDataMin: Date = null;
    public searchDataMax: Date = null;
    public option = null;
    public segmentedPeakPower: number = null;
    public showWarnLimit: boolean = false;
    public alarmThresholdValidation: TiValidationConfig = {
        tip: '',
        tipPosition: 'top',
        errorMessage: {
            integer: this.translate.instant('VALID_INTEGER_INFO'),
            number: this.translate.instant('VALID_NUMBER_INFO'),
            required: '',
            isRangeValue: ''
        }
    };
    public alarmThreshold: {
        value: string, iconTip: string, validationWRules: TiValidators[],
        validationBTURules: TiValidators[], editor(): void, cancel(): void, confirm(value: string): void
    } = {
            value: '',
            iconTip: this.translate.instant('COMMON_EDIT'),
            validationWRules: [TiValidators.required],
            validationBTURules: [TiValidators.required],
            editor: () => {
                let highPowerThresholdWatts = this.powerCappingInfo.getHighPowerThresholdWatts();
                if (highPowerThresholdWatts && this.units !== 'W') {
                    highPowerThresholdWatts = this.service.wattstoBTUs(highPowerThresholdWatts);
                }
                this.alarmThreshold.value = highPowerThresholdWatts ? highPowerThresholdWatts.toString() : null;
            },
            cancel: () => {
                let highPowerThresholdWatts = this.powerCappingInfo.getHighPowerThresholdWatts();
                if (highPowerThresholdWatts && this.units !== 'W') {
                    highPowerThresholdWatts = this.service.wattstoBTUs(highPowerThresholdWatts);
                }
                this.alarmThreshold.value = highPowerThresholdWatts ? highPowerThresholdWatts.toString() + ` (${this.units})` : '--';
            },
            confirm: (value) => {
                let currentThresholdWatts = parseInt(value, 10);
                if (currentThresholdWatts && this.units !== 'W') {
                    currentThresholdWatts = Math.ceil(currentThresholdWatts * W_TO_BTU_COEFF);
                    // 向上取整的结果不能大于最大值
                    const highPowerThresholdWatts = this.powerCappingInfo.getMaxPowerLimitInWatts();
                    currentThresholdWatts = Math.min(currentThresholdWatts, highPowerThresholdWatts);
                }
                const param = {
                    HighPowerThresholdWatts: currentThresholdWatts
                };
                this.service.setPower(param).subscribe((response) => {
                    const powerCapping = this.service.getPowerCapping(response.body, this.units);
                    this.powerCappingInfo.setHighPowerThresholdWatts(powerCapping.highPowerThresholdWatts);
                    this.init();
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                });
            }
        };

    private queryTask(url) {
        this.service.queryTask(url).subscribe((response) => {
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

    public downClick() {
        this.isDown = true;
        this.service.downLoadHistory().subscribe((response) => {
            const url = response.body.url;
            this.queryTask(url);
        }, () => {
            this.isDown = false;
        });
    }

    public clearClick() {
        if (this.isSystemLock) {
            return;
        }
        this.powerHistoryModal = this.tiMessage.open({
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
                this.service.clearHistoryData().subscribe((response) => {
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    setTimeout(() => {
                        this.service.getPower().subscribe((historyResp) => {
                            const historyInfo = this.service.getPowerHistoryInfo(historyResp.body);
                            this.powerHistory.setTimeList(historyInfo.timeList);
                            this.powerHistory.setPowerPeakWattsList(historyInfo.powerPeakWattsList);
                            this.powerHistory.setPowerAverageWattsList(historyInfo.powerAverageWattsList);
                            this.timeLists = historyInfo.timeList;
                            this.powerAverageWattsList = historyInfo.powerAverageWattsList;
                            this.powerPeakWattsList = historyInfo.powerAverageWattsList;
                            this.init();
                        });
                    }, 2000);
                });
            },
            dismiss: () => { }
        });

    }

    private getBeginTime(startTime) {
        const dataRange = this.timeList;
        const startCount = dataRange.indexOf(startTime);
        if (startCount < 0) {
            let time = new Date(startTime).getTime();
            time = time + 60 * 1000;
            const arr0 = new Date(dataRange[dataRange.length - 1]).getTime();
            if (time > arr0) {
                return -1;
            } else {
                const _time = new Date(time).pattern('yyyy/MM/dd HH:mm');
                return this.getBeginTime(_time);
            }
        } else {
            return startCount;
        }
    }

    private getEndTime(endTime) {
        const dataRange = this.timeList;
        const endCount = dataRange.lastIndexOf(endTime);
        if (endCount < 0) {
            let time = new Date(endTime).getTime();
            time = time - 60 * 1000;
            const arr0 = new Date(dataRange[0]).getTime();
            if (time < arr0) {
                return -1;
            } else {
                const _time = new Date(time).pattern('yyyy/MM/dd HH:mm');
                return this.getEndTime(_time);
            }
        } else {
            return endCount;
        }
    }

    public searchDataOK() {
        const beginTime = new Date(this.searchDataValue.begin).pattern('yyyy/MM/dd HH:mm');
        const endTime = new Date(this.searchDataValue.end).pattern('yyyy/MM/dd HH:mm');
        const beginTimeBadged = this.getBeginTime(beginTime);
        const endTimeBadged = this.getEndTime(endTime);
        if (beginTimeBadged > -1 && endTimeBadged > -1 && endTimeBadged >= beginTimeBadged) {
            const timeList = this.powerHistory.getTimeList().slice(beginTimeBadged, endTimeBadged + 1);
            const powerPeakWattsList = this.powerHistory.getPowerPeakWattsList().slice(beginTimeBadged, endTimeBadged + 1);
            const powerAverageWattsList = this.powerHistory.getPowerAverageWattsList().slice(beginTimeBadged, endTimeBadged + 1);
            this.showCharts = true;
            this.initCharts(timeList, powerPeakWattsList, powerAverageWattsList);
        } else {
            this.showCharts = false;
            this.generationTime = '--';
            this.segmentedPeakPower = null;
        }
    }

    private init() {
        this.service.getPower().subscribe(res => {
            this.showWarnLimit = res.body?.PowerLimit !== null;
        });
        let powerPeakWattsList = [];
        let powerAverageWattsList = [];
        let timeList = [];
        const timeListSource = this.powerHistory.getTimeList();
        if (timeListSource.length > 144) {
            const len = timeListSource.length - 144;
            timeList = timeListSource.slice(len);
            powerPeakWattsList = this.powerHistory.getPowerPeakWattsList().slice(len);
            powerAverageWattsList = this.powerHistory.getPowerAverageWattsList().slice(len);
        } else {
            timeList = timeListSource;
            powerPeakWattsList = this.powerHistory.getPowerPeakWattsList();
            powerAverageWattsList = this.powerHistory.getPowerAverageWattsList();
        }
        if (timeListSource.length) {
            this.showCharts = true;
            const min = timeListSource[0].split(' ')[0] + ' 00:00:00';
            const max = timeListSource[timeListSource.length - 1].split(' ')[0] + ' 23:59:59';
            this.searchDataMin = new Date(min);
            this.searchDataMax = new Date(max);
            this.searchDataValue = {
                begin: new Date(timeList[0]),
                end: new Date(timeList[timeList.length - 1])
            };
            this.initCharts(timeList, powerPeakWattsList, powerAverageWattsList);
        } else {
            this.showCharts = false;
        }
        let highPowerThresholdWatts = this.powerCappingInfo.getHighPowerThresholdWatts();
        if (highPowerThresholdWatts && this.units !== 'W') {
            highPowerThresholdWatts = this.service.wattstoBTUs(highPowerThresholdWatts);
        }
        this.alarmThreshold.value = highPowerThresholdWatts ? highPowerThresholdWatts.toString() + `(${this.units})` : '--';
    }

    private initCharts(timeList, powerPeakWattsList, powerAverageWattsList) {
        this.segmentedPeakPower = null;
        powerPeakWattsList.forEach(element => {
            if (Number(element[1]) > Number(this.segmentedPeakPower)) {
                this.generationTime = element[0];
                this.segmentedPeakPower = element[1];
            }
        });
        const highPowerThresholdWatts = this.powerCappingInfo.getHighPowerThresholdWatts();
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
                    const axisValue = param[0].axisValue;
                    let dateTime = '';
                    if (axisValue.indexOf(':') > -1) {
                        dateTime = new Date(axisValue).pattern('yyyy/MM/dd HH:mm:ss');
                    } else {
                        dateTime = new Date(parseInt(axisValue, 10)).pattern('yyyy/MM/dd HH:mm:ss');
                    }
                    return `<span class="toolTipTime">${htmlEncode(dateTime)}</span><br>${res}`;
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
                    name: this.translate.instant('POWER_PEAK'),
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
                markLine: {
                    symbol: '',
                    lineStyle: {
                        normal: {
                            color: '#F45C5E',
                            width: 1
                        }
                    },
                    label: {
                        normal: {
                            show: false
                        }
                    },
                    data: [{
                        type: '',
                        yAxis: this.units === 'W' ? highPowerThresholdWatts : this.service.wattstoBTUs(highPowerThresholdWatts)
                    }]
                }
            },
            {
                // 峰值功率
                name: this.translate.instant('POWER_PEAK'),
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
                markLine: {
                    symbol: '',
                    lineStyle: {
                        normal: {
                            color: '#F45C5E',
                            width: 1
                        }
                    },
                    label: {
                        normal: {
                            show: false
                        }
                    },
                    data: [{
                        type: '',
                        yAxis: this.units === 'W' ? highPowerThresholdWatts : this.service.wattstoBTUs(highPowerThresholdWatts)
                    }]
                }
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



    // 切换单位为BTU 数据的转化和校验规则更新
    private utilBtu() {
        const powerPeakWattsList = [];
        const powerAverageWattsList = [];
        this.powerPeakWattsList.forEach(element => {
            powerPeakWattsList.push([element[0], this.service.wattstoBTUs(element[1])]);
        });
        this.powerAverageWattsList.forEach(element => {
            powerAverageWattsList.push([element[0], this.service.wattstoBTUs(element[1])]);
        });
        this.powerHistory.setPowerPeakWattsList(powerPeakWattsList);
        this.powerHistory.setPowerAverageWattsList(powerAverageWattsList);
        const maxBTU = this.service.wattstoBTUs(this.powerCappingInfo.getMaxPowerLimitInWatts());
        this.alarmThreshold.validationBTURules = [
            TiValidators.required,
            TiValidators.number,
            PowerVaild.decimalPlacesNumber(1, this.translate.instant('VALID_OUT_DECIMAL_PPOINT')),
            TiValidators.rangeValue(3.4, maxBTU),
        ];
        const errorTip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [3.4, maxBTU]);
        this.alarmThresholdValidation.tip = errorTip,
            this.alarmThresholdValidation.errorMessage.required = errorTip;
        this.alarmThresholdValidation.errorMessage.rangeValue = errorTip;
        this.init();
    }

    // 切换单位为W 数据的转化和校验规则更新
    private utilw() {
        const powerPeakWattsList = [];
        const powerAverageWattsList = [];
        this.powerPeakWattsList.forEach(element => {
            powerPeakWattsList.push(element);
        });
        this.powerAverageWattsList.forEach(element => {
            powerAverageWattsList.push(element);
        });
        this.powerHistory.setPowerPeakWattsList(this.powerPeakWattsList);
        this.powerHistory.setPowerAverageWattsList(this.powerAverageWattsList);
        this.alarmThreshold.validationWRules = [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(1, this.powerCappingInfo.getMaxPowerLimitInWatts()),
        ];
        const errorTip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [1, this.powerCappingInfo.getMaxPowerLimitInWatts()]);
        this.alarmThresholdValidation.tip = errorTip,
            this.alarmThresholdValidation.errorMessage.required = errorTip;
        this.alarmThresholdValidation.errorMessage.rangeValue = errorTip;
        this.init();
    }

    ngOnChanges(): void {
        if (this.powerUnit === 'W') {
            this.units = 'W';
            this.totalUnits = 'kWh';
            this.utilw();
        } else {
            this.units = 'BTU/h';
            this.totalUnits = 'BTU';
            this.utilBtu();
        }
    }

    ngOnInit(): void {
        this.showCharts = this.timeLists.length !== 0;
        const times = this.powerHistory.getTimeList();
        times.forEach(element => {
            this.timeList.push(element.substring(0, 16));
        });
        const errorTip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [1, this.powerCappingInfo.getMaxPowerLimitInWatts()]);
        this.alarmThresholdValidation.tip = errorTip,
            this.alarmThresholdValidation.errorMessage.required = errorTip;
        this.alarmThresholdValidation.errorMessage.isRangeValue = errorTip;
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.powerHistoryModal) {
                this.powerHistoryModal.dismiss();
            }
        });
        this.init();
    }

}
