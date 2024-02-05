import { Router } from '@angular/router';
import { TranslateService } from '@ngx-translate/core';

import { Component, OnInit, Input, OnChanges } from '@angular/core';
import { CommonService } from 'src/app/common-app/service/common.service';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { IDeviceInfo, IMonitorInfo } from '../../model/home.datatype';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-home-irm-detail',
    templateUrl: './home-irm-detail.component.html',
    styleUrls: ['./home-irm-detail.component.scss'],
})
export class HomeIrmDetailComponent implements OnInit, OnChanges {
    @Input() loading = true;

    @Input() serverDetail: IDeviceInfo;
    @Input() monitorInfo: IMonitorInfo;
    @Input() isRM110: boolean;

    public systemLocked: boolean = true;
    public optionUse: any;
    public optionDevice: any;
    public optionPower: any;
    public maxRatedPower: any;

    constructor(
        private router: Router,
        private commonService: CommonService,
        private translate: TranslateService,
        private lockService: SystemLockService
    ) { }

    ngOnInit() {
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
        this.initOptionUse();
        this.initOptionDevice();
        this.initOptionPower();
    }
    ngOnChanges() {
        this.initOptionUse();
        this.initOptionDevice();
        this.initOptionPower();
    }

    public showMore() {
        this.router.navigate(['/navigate/system/info/product']);
    }

    public assetDetails() {
        this.router.navigate(['/navigate/system/asset']);
    }

    public powerDetails() {
        this.router.navigate(['/navigate/system/power']);
    }

    public initOptionUse() {
        let usageData = 0;
        let usageDataColor = '';
        const used = this.translate.instant('IRM_HOME_USE');
        if (this.monitorInfo.usedU && this.monitorInfo.usedU > 0) {
            usageData = Math.ceil(this.monitorInfo.usedU / this.monitorInfo.totalU * 100);
            usageDataColor = '#86A6FC';
        } else {
            usageData = 0;
            usageDataColor = 'transparent';
        }

        const cabinetUsageData = {
            seriesData: [{
                value: usageData,
                itemStyle: {
                    normal: {
                        color: usageDataColor
                    }
                },
                label: {
                    normal: {
                        show: true,
                        position: 'center',
                        rich: {
                            div: {
                                fontSize: document.documentElement.clientWidth > 1440 ? 32 : 20,
                                fontFamily: CommonData.fontFamily,
                                color: '#fff',
                                lineHeight: 32,
                                verticalAlign: 'bottom'
                            },
                            span: {
                                fontSize: 16,
                                fontFamily: CommonData.fontFamily,
                                color: '#B8BECC',
                                lineHeight: 32
                            },
                            name: {
                                fontSize: 14,
                                fontFamily: CommonData.fontFamily,
                                color: '#9EA4B3',
                                lineHeight: 20

                            }
                        },
                        formatter() {
                            return ('{div|' + usageData + '}' + '{span|' + ' %' + '}' + '{name|' + '\n' + used + '}');
                        }
                    }
                }
            }, {
                value: 100 - usageData,
                itemStyle: {
                    normal: {
                        color: 'transparent'
                    }
                }
            }]
        };
        this.optionUse = this.sysMonitorChart(cabinetUsageData, false);
    }

    public initOptionDevice() {
        let totalData = 0;
        const serviceDataColor = '#86a6fc';
        const netDataColor = '#85caff';
        const storyDataColor = '#91e6b1';
        const otherDataColor = '#f5a623';

        const total = this.translate.instant('IRM_DEVICE_TOTAL');

        totalData = this.monitorInfo.serverNumber + this.monitorInfo.storeNumber +
            this.monitorInfo.networkNumber + this.monitorInfo.otherNumber;

        const deviceChartData = {
            seriesData: [{
                name: this.translate.instant('IRM_HOME_SERVER'),
                value: this.monitorInfo.serverNumber,
                itemStyle: {
                    normal: {
                        color: serviceDataColor
                    }
                },
                label: {
                    normal: {
                        show: true,
                        position: 'center',
                        rich: {
                            div: {
                                fontSize: document.documentElement.clientWidth > 1440 ? 32 : 20,
                                fontFamily: CommonData.fontFamily,
                                color: '#fff'

                            },
                            span: {
                                fontSize: 18,
                                fontFamily: CommonData.fontFamily,
                                color: '#B8BECC'

                            },
                            name: {
                                fontSize: 14,
                                fontFamily: CommonData.fontFamily,
                                color: '#9EA4B3',
                                lineHeight: 20

                            }
                        },
                        formatter() {
                            return (String('{div|' + totalData + '}' + '{span|') + '}' + '{name|' + '\n' + total + '}');
                        }
                    }
                }
            }
                , {
                name: this.translate.instant('IRM_HOME_NETWORK'),
                value: this.monitorInfo.networkNumber,
                itemStyle: {
                    normal: {
                        color: netDataColor
                    }
                }
            }
                , {
                name: this.translate.instant('IRM_HOME_STORE'),
                value: this.monitorInfo.storeNumber,
                itemStyle: {
                    normal: {
                        color: storyDataColor
                    }
                }
            }
                , {
                name: this.translate.instant('IRM_HOME_OTHER'),
                value: this.monitorInfo.otherNumber,
                itemStyle: {
                    normal: {
                        color: otherDataColor
                    }
                }
            }
            ]
        };
        this.optionDevice = this.sysMonitorChart(deviceChartData, true);
    }

    public sysMonitorChart(dataJson: any, showTip: any) {
        let tooltip: any = {
            show: false
        };
        if (showTip) {
            tooltip = {
                trigger: 'item',
                formatter: '{b} : {c} ({d}%)'
            };
        }
        const option = {
            'tooltip': tooltip,
            series: [{
                name: '',
                type: 'pie',
                radius: ['78%', '80%'],
                center: ['50%', '50%'],
                cursor: 'pointer',
                hoverAnimation: false,
                startAngle: 90,
                silent: true,
                clockwise: true,
                label: {
                    normal: {
                        show: false
                    },
                    emphasis: {
                        show: false
                    }
                },
                labelLine: {
                    normal: {
                        show: false
                    }
                },
                data: [{
                    value: 100,
                    itemStyle: {
                        normal: {
                            color: '#545966'
                        }
                    }
                }]
            }, {
                name: '',
                type: 'pie',
                radius: ['89%', '90%'],
                center: ['50%', '50%'],
                cursor: 'pointer',
                hoverAnimation: false,
                startAngle: 90,
                silent: true,
                clockwise: true,
                label: {
                    normal: {
                        show: false
                    },
                    emphasis: {
                        show: false
                    }
                },
                labelLine: {
                    normal: {
                        show: false
                    }
                },
                data: [{
                    value: 100,
                    itemStyle: {
                        normal: {
                            color: '#545966'
                        }
                    }
                }]
            }, {
                name: '',
                type: 'pie',
                radius: ['69%', '80%'],
                center: ['50%', '50%'],
                cursor: 'pointer',
                startAngle: 90,
                selectedOffset: '55',
                label: {
                    normal: {
                        show: false
                    },
                    emphasis: {
                        show: false
                    }
                },
                labelLine: {
                    normal: {
                        show: false
                    }
                },
                data: dataJson.seriesData
            }]
        };
        return option;
    }

    public initOptionPower() {
        let maxPower: number = 0;
        let powerStr: string = '';
        const power = this.translate.instant('IRM_POWER');
        if (this.monitorInfo.maxRatedPower > 0) {
            this.maxRatedPower = this.monitorInfo.maxRatedPower / 1000;
            maxPower = this.monitorInfo.currentPower;
            const roundPower = parseInt((maxPower / 1000) + '', 10);
            let decimalPower: any = maxPower % 1000 / 1000;
            if (decimalPower === 0) {
                powerStr = '{div|' + roundPower + '}' + '{span|' + ' kW' + '}' + '{name|' + '\n' + power + '}';
            } else {
                decimalPower = (String(decimalPower)).split('.')[1];
                powerStr = '{div|' + roundPower + '}' + '{p|' + '.' + decimalPower + '}' +
                    '{span|' + ' kW' + '}' + '{name|' + '\n' + power + '}';
            }
        } else {
            this.monitorInfo.maxRatedPower = 0;
        }
        let poweData = 0;
        let poweDataColor = '';
        if (this.monitorInfo.maxRatedPower && maxPower > 0) {
            poweData = this.monitorInfo.currentPower / 1000 / 36 * 100;
            poweDataColor = '#f5a632';
        } else {
            poweData = 0;
            poweDataColor = 'transparent';
        }

        const isClockwise = true;
        const startAngle = 225;
        const ratedPowerData = {
            seriesData: [{
                value: poweData * 3 / 4,
                itemStyle: {
                    normal: {
                        color: poweDataColor
                    }
                },
                label: {
                    normal: {
                        show: true,
                        position: 'center',
                        rich: {
                            div: {
                                fontSize: document.documentElement.clientWidth > 1440 ? 32 : 16,
                                fontFamily: CommonData.fontFamily,
                                color: '#fff'

                            },
                            span: {
                                fontSize: document.documentElement.clientWidth > 1440 ? 16 : 12,
                                fontFamily: CommonData.fontFamily,
                                color: '#B8BECC'
                            },
                            p: {
                                fontSize: 16,
                                fontFamily: CommonData.fontFamily,
                                color: '#fff'
                            },
                            name: {
                                fontSize: 14,
                                fontFamily: CommonData.fontFamily,
                                color: '#9EA4B3',
                                lineHeight: 20

                            }
                        },
                        formatter() {
                            return powerStr;
                        }
                    }
                }
            }, {
                value: 100 - Number(poweData * 3 / 4),
                itemStyle: {
                    normal: {
                        color: 'transparent'
                    }
                }
            }]
        };
        this.optionPower = this.sysMonitorChartGauge(ratedPowerData, isClockwise, startAngle, 0, 36);
    }

    public sysMonitorChartGauge(dataJson, clockwise, startAngle, minValue, maxValue) {
        const option = {
            tooltip: {
                show: false
            },
            series: [{
                name: '',
                type: 'pie',
                radius: ['78%', '80%'],
                center: ['50%', '50%'],
                cursor: 'pointer',
                hoverAnimation: false,
                startAngle: 225,
                silent: true,
                clockwise: true,
                label: {
                    normal: {
                        show: false
                    },
                    emphasis: {
                        show: false
                    }
                },
                labelLine: {
                    normal: {
                        show: false
                    }
                },
                data: [{
                    value: 75,
                    itemStyle: {
                        normal: {
                            color: '#545966'
                        }
                    }
                }, {
                    value: 25,
                    itemStyle: {
                        normal: {
                            color: 'transparent'
                        }
                    }
                }]
            }, {
                name: '',
                type: 'pie',
                radius: ['69%', '80%'],
                center: ['50%', '50%'],
                cursor: 'pointer',
                hoverAnimation: false,
                'startAngle': startAngle,
                silent: true,
                'clockwise': clockwise,
                label: {
                    normal: {
                        show: false
                    },
                    emphasis: {
                        show: false
                    }
                },
                labelLine: {
                    normal: {
                        show: false
                    }
                },
                data: dataJson.seriesData
            }, {
                name: '',
                type: 'pie',
                radius: ['89%', '90%'],
                center: ['50%', '50%'],
                cursor: 'pointer',
                hoverAnimation: false,
                startAngle: 225,
                silent: true,
                clockwise: true,
                label: {
                    normal: {
                        show: false
                    },
                    emphasis: {
                        show: false
                    }
                },
                labelLine: {
                    normal: {
                        show: false
                    }
                },
                data: [{
                    value: 75,
                    itemStyle: {
                        normal: {
                            color: '#545966'
                        }
                    }
                }, {
                    value: 25,
                    itemStyle: {
                        normal: {
                            color: 'transparent'
                        }
                    }
                }]
            }, {
                name: '',
                type: 'gauge',
                min: minValue,
                max: maxValue,
                splitNumber: 12,
                axisLine: {
                    show: true,
                    lineStyle: {
                        width: 0
                    }
                },
                axisTick: {
                    show: false
                },
                axisLabel: {
                    distance: 0,
                    textStyle: {
                        color: '#B8BECC'
                    },
                    rich: {
                        span: {
                            fontSize: 9,
                            fontFamily: CommonData.fontFamily,
                            lineHeight: 9,
                            color: '#B8BECC'

                        }
                    },
                    formatter(val) {
                        return ('{span|' + parseInt(val, 10) + '}');
                    }
                },
                splitLine: {
                    show: true,
                    length: 5,
                    lineStyle: {
                        color: 'transparent'
                    }
                },
                pointer: {
                    width: 0
                },
                title: {
                    show: false
                },
                detail: {
                    show: false
                },
                data: [{ value: 50 }]
            }]
        };

        return option;
    }
}
