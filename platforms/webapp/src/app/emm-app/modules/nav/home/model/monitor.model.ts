import { EChartsCoreOption } from 'echarts';
import { IMonitorParams } from '../home.datatype';

interface IPieConfigOption {
    xRadius: string;
    yRadius: string;
    startAngle: number;
    value: number;
    clockwise: boolean;
    detailData?: object;
}

export class Monitor {
    constructor(private params: IMonitorParams) {
        const innerHeight = window.innerWidth;
        this.params.lineHeight = this.params.lineHeight || 32;
        this.params.startAngle = this.params.startAngle || 90;
        this.params.clockwise = this.params.clockwise || true;
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
                        fontSize: 24,
                        lineHeight: this.params.lineHeight,
                        color: '#282B33',
                    },
                    p: {
                        fontSize: 15,
                        color: '#B8BECC',
                        verticalAlign: 'bottom',
                        padding: [6, 0, 0, 0]
                    },
                    span: {
                        fontSize: 12,
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

    private readonly firstPieXRadius = '77%';
    private readonly firstPieYRadius = '80%';
    private readonly secondPieXRadius = '69%';
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

    private getPieOption(config: IPieConfigOption) {
        const defaultData = {
            value: config.value,
            itemStyle: {
                normal: {
                    color: '#545968',
                },
            },
        };
        const option = {
            name: '',
            type: 'pie',
            radius: [config.xRadius, config.yRadius],
            center: ['50%', '50%'],
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
        return {...option, ...resultData};
    }
}
