import { Component, Input, OnChanges, SimpleChanges } from '@angular/core';
import { NetworkMonitor } from '../../model/networkMonitor';
import { htmlEncode } from 'src/app/common-app/utils/common';

@Component({
    selector: 'app-network-charts',
    templateUrl: './network-charts.component.html',
    styleUrls: ['./network-charts.component.scss']
})
export class NetworkChartsComponent implements OnChanges {

    constructor() { }
    @Input() chartsData: NetworkMonitor;
    @Input() networkThresholdPercent: number;

    public option = {};

    private networkCharts(): void {
        const data = this.chartsData.getCharts().getDatas();
        const port = this.chartsData.getCharts().getPorts();
        const legendData = data.map((item: any, i: number) => {
            return {
                name: port[i],
                icon: 'circle'
            };
        });
        const seriesData = data.map((item: any, i: number) => {
            const dataObj: any = {
                name: port[i],
                type: 'line',
                smooth: true,
                showSymbol: false,
                data: item,
                areaStyle: {
                    normal: {
                        opacity: 0.2
                    }
                }
            };
            if (i === 0) {
                dataObj.markLine = {
                    silent: false,
                    symbol: '',
                    lineStyle: {
                        width: 1,
                        color: 'red'
                    },
                    label: {
                        normal: {
                            show: false
                        }
                    },
                    data: [{
                        type: '',
                        yAxis: this.networkThresholdPercent
                    }]
                };
            }
            return dataObj;
        });
        const colorArr = ['#6C92FA', '#62DAD0', '#A7DD82', '#F45C5E', '#6CBFFF', '#EBD457', '#F4B853', '#FA8E5A'];
        const visualMapArr = colorArr.map((color, index) => {
            return {
                show: false,
                seriesIndex: index,
                pieces: [{
                    gt: 0,
                    lte: Number(this.networkThresholdPercent),
                    color
                }, {
                    gt: Number(this.networkThresholdPercent),
                    lte: 100,
                    color: 'red'
                }]
            };
        });
        this.option = {
            color: colorArr,
            tooltip: {
                trigger: 'axis',
                backgroundColor: 'rgba(62, 66, 77, 0.8)',
                extraCssText: 'padding:8px 16px; border-radius:1px; ',
                formatter (param) {
                    let res = '';
                    for (const item of param) {
                        res += `<span class="toolTipTime">${htmlEncode(item.seriesName)}: </span>
                                <span class="toolTipValue">${htmlEncode(item.data)}</span><br>`;
                    }
                    return `<span class="toolTipTime">${htmlEncode(param[0].axisValue)}</span><br>${res}`;
                }
            },
            visualMap: visualMapArr,
            legend: {
                align: 'left',
                top:  256,
                itemGap: 32,
                itemWidth: 10,
                textStyle: {
                    fontSize: 12,
                    padding: [4, 0, 0, 0]
                },
                smooth: false,
                data: legendData
            },
            grid: {
                borderWidth: 0,
                containLabel: true,
                x: 12,
                y: 12,
                x2: 12,
                y2: 32
            },
            xAxis: {
                show: true,
                type: 'category',
                boundaryGap: false,
                data: this.chartsData.getCharts().getTimes(),
                bottom: 32,
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
                        if (v === undefined) {
                            return false;
                        }
                        return v.slice(5).replace(' ', '\n');
                    }
                },
                splitLine: {
                    show: false
                }
            },
            yAxis: {
                type: 'value',
                axisLine: {
                    show: false
                },
                splitLine: {
                    show: false
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
            series: seriesData
        };
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.chartsData.currentValue) {
            this.networkCharts();
        }
    }

}
