import { Component, Input, OnChanges, SimpleChanges } from '@angular/core';
import { htmlEncode } from 'src/app/common-app/utils/common';
import { IChartsData } from '../../model/monitor.datatype';

@Component({
    selector: 'app-monitor-charts',
    templateUrl: './monitor-charts.component.html',
    styleUrls: ['./monitor-charts.component.scss']
})
export class MonitorChartsComponent implements OnChanges {

    constructor() { }

    @Input() chartsData: IChartsData;
    public option = {};
    private singleWireCharts(): void {
        const max =  Math.max.apply(null, this.chartsData.seriesData);
        const limit = this.chartsData.limitVal / max;
        let cunstomColor = {
            type: 'linear',
            x: 0,
            y: 1,
            x2: 0,
            y2: 0,
            colorStops: [{
                offset: 0, color: '#fff'
            }, {
                offset: limit, color: '#6C92FA'
            }, {
                offset: limit, color: '#F45B5D'
            }, {
                offset: 1, color: '#F45B5D'
            }]
        };
        if (this.chartsData.limitVal >= max) {
            cunstomColor = {
                type: 'linear',
                x: 0,
                y: 1,
                x2: 0,
                y2: 0,
                colorStops: [{
                    offset: 0, color: '#fff'
                }, {
                    offset: 1, color: '#6C92FA'
                }]
            };
        }
        this.option = {
            tooltip: {
                trigger: 'axis',
                backgroundColor: 'rgba(62, 66, 77, 0.8)',
                extraCssText: 'padding:8px 16px; border-radius:1px;',
                formatter: (param) => {
                    return  `<span class="toolTipTime">${htmlEncode(param[0].axisValue)}</span><br>
                            <span class="toolTipValue">${htmlEncode(param[0].data)}(%)</span>`;
                }
            },
            visualMap: {
                show: false,
                pieces: [{
                    gt: 0,
                    lte: this.chartsData.limitVal,
                    color: '#6C92FA'
                }, {
                    gt: this.chartsData.limitVal,
                    lte: 100,
                    color: '#F45C5E'
                }]
            },
            xAxis: {
                show: true,
                type: 'category',
                boundaryGap: false,
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
                    formatter(v) {
                        return v.slice(5).replace(' ', '\n');
                    }
                },
                splitLine : {
                    show: false
                },
                data: this.chartsData.xData
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
                    margin: 8,
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
                x: 12,
                y: 12,
                x2: 0,
                y2: 0,
                containLabel: true
            },
            series: [
                {
                    name: '',
                    type: 'line',
                    smooth: true,
                    showSymbol: false,
                    markLine: {
                        silent: false,
                        symbol: '',
                        lineStyle: {
                            width:  1,
                            color: '#F45C5E'
                        },
                        label: {
                            normal: {
                                show: false
                            }
                        },
                        data: [{
                            type: '',
                            yAxis: this.chartsData.limitVal
                        }]
                    },
                    markPoint: {
                        symbol: 'triangle',
                        symbolSize: 10,
                        label: {
                            normal: {
                                show: false
                            }
                        },
                        itemStyle: {
                            normal: {
                                color: '#F45B5D'
                            }
                        },
                        data: [{
                            type: 'max',
                            symbolOffset: [0, -8]
                        }]
                    },
                    areaStyle: {
                        normal: {
                            color: cunstomColor,
                            opacity: 0.2
                        }
                    },
                    data: this.chartsData.seriesData
                }
            ]
        };
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.chartsData.currentValue !== null ) {
            this.singleWireCharts();
        }
    }

}
