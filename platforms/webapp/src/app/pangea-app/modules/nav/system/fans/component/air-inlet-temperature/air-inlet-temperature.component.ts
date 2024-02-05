import { Component, Input, OnChanges } from '@angular/core';
import { AirInletTemperature } from '../../model/airInletTemperature';
import { FanService } from '../../services/fan.service';
import { TiMessageService } from '@cloud/tiny3';
import { htmlEncode } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';
import { FanInfo } from '../../model/fanInfo';
import { GlobalData } from 'src/app/common-app/models';
import { AlertMessageService, UserInfoService } from 'src/app/common-app/service';


@Component({
  selector: 'app-air-inlet-temperature',
  templateUrl: './air-inlet-temperature.component.html',
  styleUrls: ['./air-inlet-temperature.component.scss']
})
export class AirInletTemperatureComponent implements OnChanges {

    public option = {};
    public inletTemperature: string | number = '--';
    @Input() airInletTemperature: AirInletTemperature;
    @Input() fanInfoList: FanInfo[];
    public refreshState: boolean = false;
    public isCharts: boolean = true;
    public isLowBrowserVersion: boolean = GlobalData.getInstance().getIsLowBrowserVer;
    public clearTip: string = this.translate.instant('COMMON_CLEAR');
    public refreshTip: string = this.translate.instant('COMMON_REFRESH');
    constructor(
        private service: FanService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private user: UserInfoService,
        private translate: TranslateService
    ) { }

    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    init() {
        this.inletTemperature = this.airInletTemperature.getInletTemperature();
        this.isCharts = this.airInletTemperature.getTime().length !== 0;
        this.option = {
            tooltip: {
                trigger: 'axis',
                extraCssText: 'padding:8px 16px; border-radius:1px;',
                backgroundColor: 'rgba(62, 66, 77, 0.8)',
                formatter (param) {
                    let res = '';
                    for (const item of param) {
                        res += `<span class="toolTipValue">${htmlEncode(item.data)}(℃)</span>`;
                    }
                    return `<span class="toolTipTime">${htmlEncode(param[0].axisValue)}</span><br>${res}`;
                }
            },
            xAxis: {
                show: true,
                type: 'category',
                boundaryGap: false,
                data: this.airInletTemperature.getTime(),
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
                splitLine : {
                    show: false
                },
                axisLabel: {
                    margin: 15,
                    color: '#9EA4B3',
                    fontSize: 10,
                    formatter(v) {
                        return v.slice(5).replace(' ', '\n');
                    }
                }
            },
            yAxis: {
                type: 'value',
                axisLine: {
                    show: false
                },
                splitLine: {
                    lineStyle: {
                        type: 'dashed',
                        color: '#E6EBF5'
                    }
                },
                offset: 2,
                axisTick: {
                    show: false
                },
                axisLabel: {
                    margin: 8 ,
                    textStyle: {
                        align: 'right',
                        baseline: 'bottom',
                        color: '#9EA4B3',
                        fontSize: 12
                    }
                },
                splitNumber: 4

            },
            grid: {
                borderWidth: 0,
                containLabel: true,
                x: 12,
                y: 12,
                x2: 24,
                y2: 0
            },
            series: [
                {
                    name: '',
                    smooth: true,
                    showSymbol: false,
                    type: 'line',
                    lineStyle: {
                        normal: {
                            width: 2,
                            color: '#6C92FA'
                        }
                    },
                    itemStyle: {
                        normal: {
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
                                    offset: 0, color: '#6C92FA'
                                }, {
                                    offset: 1, color: '#fff'
                                }]
                            },
                            opacity: 0.2
                        }
                    },
                    data: this.airInletTemperature.getInletTempCelsius()
                }
            ]
        };
    }

    public refresh() {
        this.refreshState = true;
        this.service.getThermals().subscribe((response) => {
            const airInletInfo = this.service.getAirInletTemperature(response['body']);
            this.airInletTemperature = new AirInletTemperature(airInletInfo.inletTemperature, airInletInfo.time,
                airInletInfo.inletTempCelsius);
            this.init();
	        this.refreshState = false;
        }, (error) => {
		    this.refreshState = false;
	    });
    }

    public tempClear() {
        if (this.isSystemLock) {
            return;
        }
        this.tiMessage.open({
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                autofocus: false,
                primary: true,
            },
            cancelButton: { },
            close: () => {
                this.refreshState = true;
                this.service.clearInletHistoryTemperature().subscribe((response) => {
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                    this.refresh();
                }, (error) => {
                    this.refreshState = false;
                });
            },
            dismiss(): void { }
        });
    }

    ngOnInit(): void {
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.airInletTemperature.currentValue !== null) {
            this.init();
        }
    }

}
