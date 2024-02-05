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

export class MonitorBeautiful {
    constructor(private params: IMonitorParams) {
        const innerHeight = window.innerWidth;
        this.params.lineHeight = this.params.lineHeight || 32;
        this.params.startAngle = this.params.startAngle || 90;
        this.params.clockwise = this.params.clockwise !== false;
        this.detailPieData = {
            value: this.params.value,
            itemStyle: {
                normal: {
                    color: this.getColor(),
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
                            color: '#000',
                        },
                        p: {
                            fontSize: 15,
                            fontFamily: 'PingFang, "Hiragino Sans GB", "Microsoft YaHei", "Microsoft JhengHei", "Huawei sans", Arial',
                            color: '#000',
                            verticalAlign: 'middle',
                            padding: [6, 0, 0, 0]
                        },
                        span: {
                            fontSize: 12,
                            fontFamily: 'PingFang, "Hiragino Sans GB", "Microsoft YaHei", "Microsoft JhengHei", "Huawei sans", Arial',
                            lineHeight: 16,
                            color: '#000',
                        },
                    },
                    formatter: this.params.formatter
                }
            }
        };
    }

    public get pieOptions(): EChartsCoreOption {
        const firstPieOption = this.getPieOption({
            xRadius: this.firstPieXRadius,
            yRadius: this.firstPieYRadius,
            startAngle: this.firstPieStartAngle,
            value: this.pieValue,
            clockwise: true
        });
        const secondPieOption = this.getPieOption({
            xRadius: this.secondPieXRadius,
            yRadius: this.secondPieYRadius,
            startAngle: this.params.startAngle,
            value: this.params.value,
            clockwise: true,
            detailData: this.detailPieData
        });
        const thirdPieOption = this.getPieOption({
            xRadius: this.thirdPieXRadius,
            yRadius: this.thirdPieYRadius,
            startAngle: this.thirdPieStartAngle,
            value: this.pieValue,
            clockwise: true
        });

        return {
            series: [firstPieOption, secondPieOption, thirdPieOption]
        };
    }

    public get gaugeOptions(): EChartsCoreOption {
        const firstPieOptionWithGauge = this.getPieOption({
            xRadius: this.firstPieXRadius,
            yRadius: this.firstPieYRadius,
            startAngle: this.firstPieStartAngleWithGauge,
            value: this.pieValueWithGauge,
            clockwise: true
        });
        const secondPieOptionWithGauge = this.getPieOption({
            xRadius: this.secondPieXRadius,
            yRadius: this.secondPieYRadius,
            startAngle: this.params.startAngle,
            value: this.params.value,
            clockwise: this.params.clockwise,
            detailData: this.detailPieData
        });
        const thirdPieOptionWithGauge = this.getPieOption({
            xRadius: this.thirdPieXRadius,
            yRadius: this.thirdPieYRadius,
            startAngle: this.thirdPieStartAngleWithGauge,
            value: this.pieValueWithGauge,
            clockwise: true
        });
        return {
            series: [firstPieOptionWithGauge, secondPieOptionWithGauge, thirdPieOptionWithGauge, this.getGaugeOption()]
        };
    }

    // first:仪表盘背景环形 second:仪表盘进度条  third:仪表盘外部圆边
    private readonly firstPieXRadius = '60%';
    private readonly firstPieYRadius = '77%';
    private readonly secondPieXRadius = '60%';
    private readonly secondPieYRadius = '80%';
    private readonly thirdPieXRadius = '89%';
    private readonly thirdPieYRadius = '90%';
    private readonly firstPieStartAngle = 90;
    private readonly firstPieStartAngleWithGauge = 225;
    private readonly thirdPieStartAngle = 225;
    private readonly thirdPieStartAngleWithGauge = 225;
    private readonly pieValue = 100;
    private readonly pieValueWithGauge = 75;

    private detailPieData: object;
    // 根据数值控制颜色
    private getColor(): string {
        if (this.params.computedValue === 0) {
            return '#ddd';
        }
        if (this.params.computedValue < this.params.minorValue) {
            return '#86A6FC';
        }
        if (this.params.computedValue >= this.params.minorValue && this.params.computedValue < this.params.majorVal) {
            return '#F5A623';
        }
        return '#FF4C57';
    }

    private completeValueOption(value: number, option: object[]) {
        if (value < 100) {
            const additionalDataOption = {
                value: 100 - value,
                itemStyle: {
                    normal: {
                        color: 'transparent'
                    }
                }
            };
            return {
                data: [...option, additionalDataOption]
            };
        }

        return {
            data: option
        };
    }
    // 生成饼图
    private getPieOption(config: IPieConfigOption) {
        const defaultData = {
            value: config.value,
            itemStyle: {
                normal: {
                    color: '#ECEEEF',
                },
            },
        };
        const option = {
            name: '',
            type: 'pie',
            radius: [config.xRadius, config.yRadius],
            center: ['50%', '45%'],
            cursor: 'pointer',
            hoverAnimation: false,
            startAngle: config.startAngle,
            silent: true,
            clockwise: config.clockwise,
            label: {
                normal: {
                    show: false,
                },
                emphasis: {
                    show: false,
                },
            },
            labelLine: {
                normal: {
                    show: false,
                },
            },
            data: [config.detailData || defaultData],
        };

        const resultData = this.completeValueOption(config.value, option.data);
        return { ...option, ...resultData };
    }
    // 生成仪表盘
    private getGaugeOption() {
        return {
            name: '',
            type: 'gauge',
            min: this.params.minVal,
            max: this.params.maxVal,
            // 分割段数，默认为5
            splitNumber: 6,
            axisLine: {
                // 坐标轴线
                show: true,
                lineStyle: {
                    // 属性lineStyle控制线条样式
                    width: 0,
                },
            },
            // 坐标轴小标记
            axisTick: {
                show: false,
            },
            axisLabel: {
                show: false,
                // 坐标轴文本标签，详见axis.axisLabel
                distance: 87,
                textStyle: {
                    // 其余属性默认使用全局文本样式，详见TEXTSTYLE
                    color: '#B8BECC',
                },
                rich: {
                    span: {
                        fontSize: 7,
                        lineHeight: 7,
                        color: '#B8BECC',
                    },
                },
                formatter(val: string) {
                    return '{span|' + parseInt(val, 10) + '}';
                },
            },
            // 分隔线
            splitLine: {
                // 默认显示，属性show控制显示与否
                show: true,
                distance: -10,
                // 属性length控制线长
                length: 5,
                lineStyle: {
                    // 属性lineStyle（详见lineStyle）控制线条样式
                    color: 'transparent',
                },
            },
            pointer: {
                width: 0,
            },
            title: {
                show: false,
            },
            detail: {
                show: false,
            },
        };
    }
}
