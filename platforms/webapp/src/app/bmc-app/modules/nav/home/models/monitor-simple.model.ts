import { EChartsCoreOption } from 'echarts/types/dist/echarts';
import { IMonitorParams } from '../home.datatype';

interface IPieConfigOption {
    xRadius: string;
    yRadius: string;
    startAngle: number;
    value: number;
    clockwise: boolean;
    detailData?: object;
}
export class MonitorSimple {
    public detailPieData: object;
    constructor(private params: IMonitorParams) {
        const innerHeight = window.innerWidth;
        this.params.lineHeight = this.params.lineHeight || 12;
        this.params.startAngle = this.params.startAngle || 90;
        this.params.clockwise = this.params.clockwise !== false;
        this.detailPieData = {
            value: this.params.value,
            itemStyle: {
                normal: {
                    color: this.getColor(params),
                },
            },
            label: {
                normal: {
                    show: true,
                    position: 'center',
                    rich: {
                        div: {
                            fontSize: innerHeight > 1280 ? 25 : 18,
                            fontFamily: 'PingFang, "Hiragino Sans GB", "Microsoft YaHei", "Microsoft JhengHei", "Huawei sans", Arial',
                            lineHeight: this.params.lineHeight,
                            color: 'red',
                        },
                        p: {
                            fontSize: 15,
                            fontFamily: 'PingFang, "Hiragino Sans GB", "Microsoft YaHei", "Microsoft JhengHei", "Huawei sans", Arial',
                            color: '#B8BECC',
                            verticalAlign: 'middle',
                            padding: [6, 0, 0, 0]
                        },
                        span: {
                            fontSize: 12,
                            fontFamily: 'PingFang, "Hiragino Sans GB", "Microsoft YaHei", "Microsoft JhengHei", "Huawei sans", Arial',
                            lineHeight: 16,
                            color: '#B8BECC',
                        },
                    },
                    formatter: this.params.formatter
                }
            }
        };
    }
    public get pieOptions(): EChartsCoreOption {
        return {
            series: [this.getPieOption(this.params)],
        };
    }

    public get gaugeOptions(): EChartsCoreOption {
        return {
            series: [this.getGaugeOption(this.params)],
        };
    }
    public color: string;

    // 根据数值控制颜色
    private getColor(value): string {
        if (value.computedValue === 0) {
            return '#ddd';
        }
        if (value.computedValue < value.minorValue) {
            return '#86A6FC';
        }
        if (value.computedValue >= value.minorValue && value.computedValue < value.majorVal) {
            return '#F5A623';
        }
        return '#FF4C57';
    }
    // 根据数值控制向左偏移量
    private getOffsetCenter(value): number {
        if (value < 100) {
            return 0;
        } else if (value >= 100 && value < 1000) {
            return -8;
        } else {
            return -12;
        }
    }
    // 生成饼图
    private getPieOption(data: IMonitorParams) {
        return {
            name: '',
            type: 'gauge',
            center: ['40%', '55%'],
            startAngle: 40,
            endAngle: -40,
            min: 100,
            max: 0,
            splitNumber: 1,
            axisLine: {
                lineStyle: {
                    width: 15,
                    color: [
                        [1 - (data.computedValue / 100), '#C2C4CC'], [1, this.getColor(data)],
                    ]
                }
            },
            pointer: {
                show: false
            },
            axisLabel: {
                distance: -20,
                color: '#C2C4CC'
            },
            axisTick: {
                show: false,
                length: 12,
                lineStyle: {
                    color: 'auto',
                    width: 2
                }
            },
            splitLine: {
                length: 0,
                lineStyle: {
                    color: 'auto',
                    width: 5
                }
            },
            detail: {
                fontSize: 36,
                offsetCenter: [0, '0%'],
                color: 'auto'
            },
            data: [{
                length: 12,
                value: data.computedValue,
                // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                textStyle: {
                    color: '#3ebf27',
                    fontSize: 18
                }
            }]


        };
    }
    // 生成仪表盘
    private getGaugeOption(data: IMonitorParams) {
        const percentValue = (100 - data.computedValue) / 200;
        let colorSets;
        if (!data.isSimpleTemp) {
            colorSets = [[1 - (data.computedValue / data.maxVal), '#C2C4CC'], [1, this.getColor(data)]];
        } else if (data.computedValue >= 0) {
            colorSets = [[percentValue, '#C2C4CC'], [0.5, this.getColor(data)], [1, '#C2C4CC']];
        } else if (data.computedValue < 0) {
            colorSets = [[0.5, '#C2C4CC'], [percentValue, this.getColor(data)], [1, '#C2C4CC']];
        }
        return {
            name: '',
            type: 'gauge',
            center: ['45%', '55%'],
            startAngle: 40,
            endAngle: -40,
            min: data.maxVal,
            max: data.minVal,
            splitNumber: 1,
            axisLine: {
                lineStyle: {
                    width: 15,
                    color: colorSets
                }
            },
            pointer: {
                show: false
            },
            axisLabel: {
                distance: -20,
                color: '#C2C4CC'
            },
            axisTick: {
                show: false,
                length: 12,
                lineStyle: {
                    color: 'auto',
                    width: 2
                }
            },
            splitLine: {
                length: 0,
                lineStyle: {
                    color: 'auto',
                    width: 5
                }
            },
            // 在值为三、四位数时减小字号并向左偏移
            detail: {
                fontSize: data.computedValue >= 100 ? 32 : 36,
                offsetCenter:  [this.getOffsetCenter(data.computedValue), '0%'],
                color: 'auto'
            },
            data: [{
                length: 12,
                value: data.computedValue,
                // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                textStyle: {
                    color: '#3ebf27',
                    fontSize: 18
                }
            }]


        };
    }
}
