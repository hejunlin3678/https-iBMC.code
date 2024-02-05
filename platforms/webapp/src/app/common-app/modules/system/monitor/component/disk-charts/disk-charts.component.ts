import { Component, Input, OnChanges } from '@angular/core';
import { number } from 'echarts';
import { DiskCharts } from '../../model/diskCharts';

@Component({
  selector: 'app-disk-charts',
  templateUrl: './disk-charts.component.html',
  styleUrls: ['./disk-charts.component.scss']
})
export class DiskChartsComponent implements OnChanges {

    constructor() { }
    @Input() chartsData: DiskCharts[];
    @Input() hardDiskThresholdPercent: number;
    public options = [];
    private diskChartsList(): void {
        this.options = [];
        const datas = this.chartsData;
        datas.forEach((data) => {
            let diskData;
            const {usedCapacityGB, totalCapacityGB} = data;
            if (typeof usedCapacityGB === 'number' && typeof totalCapacityGB === 'number' && totalCapacityGB) {
                diskData = Math.ceil((usedCapacityGB / totalCapacityGB) * 100);
            }
            let colors = '';
            if (diskData && diskData < this.hardDiskThresholdPercent) {
                colors = '#6C92FA';
            } else {
                colors = '#F45C5E';
            }
            diskData = (diskData || diskData === 0) ? diskData : '--';
            const seriesData = [{
                value: diskData,
                itemStyle: {
                    normal: {
                        color: colors
                    }
                },
                label: {
                    normal: {
                        show: true,
                        position: 'center',
                        width: 120,
                        rich: {
                            div: {
                                fontSize: 32,
                                lineHeight: 32,
                                color: '#282B33',
                                fontWeight: 500
                            },
                            span: {
                                fontSize: 18,
                                lineHeight: 32,
                                color: '#6C7280',
                                padding: [-7, 0, 0]
                            }
                        },
                        formatter: () => {
                            return `{div|\n${diskData}}{span| %\n}`;
                        }
                    }

                }
            },
            {
                value: 100 - diskData,
                itemStyle: {
                    normal: {
                        color: 'transparent'
                    }
                }
            }
            ];
            const option = {
                tooltip: {
                    show: false
                },
                series: [{
                    type: 'pie',
                    radius: ['86.5%', '90%'],
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
                        },
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
                                color: '#E3E9F5'
                            }
                        }
                    }]
                },
                {
                    type: 'pie',
                    radius: ['79%', '90%'],
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
                    data: seriesData
                },
                {
                    name: '',
                    type: 'pie',
                    radius: ['98.5%', '100%'],
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
                                color: '#E6EBF5'
                            }
                        }
                    }]
                }
                ]
            };
            this.options.push(option);
        });
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.chartsData.currentValue) {
            this.diskChartsList();
        }
    }

}
