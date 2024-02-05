import { Component, OnInit, Input, ElementRef } from '@angular/core';
import { ISpeedAdjustmentMode, IValidationTip } from '../../interface';
import { IntelSpeedAdjust } from '../../model/intelSpeedAdjust';
import { FormGroup, FormBuilder, ValidationErrors } from '@angular/forms';
import { TiValidators } from '@cloud/tiny3';
import { formatEntry } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-intel-speed-adjust',
    templateUrl: './intel-speed-adjust.component.html',
    styleUrls: ['./intel-speed-adjust.component.scss']
})
export class IntelSpeedAdjustComponent implements OnInit {

    constructor(
        private fb: FormBuilder,
        private elementRef: ElementRef,
        private translate: TranslateService
    ) { }

    @Input() intelSpeedAdjust: IntelSpeedAdjust;
    @Input() param;

    public temperatureRangeList: {label: string}[];

    public saveBtn: boolean = true;
    public customForm: FormGroup;

    public energySaving: ISpeedAdjustmentMode = {
        id: 'EnergySaving',
        label: this.translate.instant('FAN_MODEL_TITLE1'),
        content: this.translate.instant('FAN_MODEL_CONTENT1'),
        isSelect: false,
        isShow: true
    };
    public lowNoise: ISpeedAdjustmentMode = {
        id: 'LowNoise',
        label: this.translate.instant('FAN_MODEL_TITLE2'),
        content: this.translate.instant('FAN_MODEL_CONTENT2'),
        isSelect: false,
        isShow: true
    };
    public highPerformance: ISpeedAdjustmentMode = {
        id: 'HighPerformance',
        label: this.translate.instant('FAN_MODEL_TITLE3'),
        content: this.translate.instant('FAN_MODEL_CONTENT3'),
        isSelect: false,
        isShow: true
    };
    public custom: ISpeedAdjustmentMode = {
        id: 'Custom',
        label: this.translate.instant('FAN_MODEL_TITLE4'),
        content: this.translate.instant('FAN_MODEL_CONTENT4'),
        isSelect: false,
        isShow: true,
        tip: this.translate.instant('FAN_HEAT_MODE_WARNING')
    };
    public liquidCooling: ISpeedAdjustmentMode = {
        id: 'LiquidCooling',
        label: this.translate.instant('FAN_LIQUID_COOLING'),
        content: this.translate.instant('FAN_LIQUID_COOLING_CONTENT'),
        isSelect: false,
        isShow: true
    };
    public cpuValidation: IValidationTip = {
        tip: '',
        tipPosition: 'right'
    };
    public hdmValidation: IValidationTip = {
        tip: '',
        tipPosition: 'right'
    };
    public aiValidation: IValidationTip = {
        tip: '',
        tipPosition: 'right'
    };
    public outletValidation: IValidationTip = {
        tip: '',
        tipPosition: 'right'
    };
    public fanSpeedValidation: IValidationTip = {
        tip: '',
        tipPosition: 'top'
    };
    public selected(mode: ISpeedAdjustmentMode) {
        this.energySaving.isSelect = false;
        this.lowNoise.isSelect = false;
        this.highPerformance.isSelect = false;
        this.custom.isSelect = false;
        this.liquidCooling.isSelect = false;
        mode.isSelect = true;
        if (mode.id === 'Custom') {
            this.customForm.enable();
        } else {
            this.customForm.disable();
        }
        this.mointorBtn(mode.id);
    }

    public mointorBtn(selectId: string) {
        const errors: ValidationErrors | null = TiValidators.check(this.customForm);
        if (errors) {
            this.saveBtn = true;
            const firstError: any = Object.keys(errors)[0];
            this.elementRef.nativeElement.querySelector(`[formControlName=${firstError}]`).focus();
            return;
        }
        const param = this.getParam(selectId);
        if (param === null) {
            this.saveBtn = true;
        } else {
            this.saveBtn = false;
        }
    }

    private getCustomModeParam() {
        const param = {};
        if (this.customForm.root.get('cpuTargetTemperatureCelsius').value !==
            this.intelSpeedAdjust.getCPUTargetTemperatureCelsius()) {
            param['CPUTargetTemperatureCelsius'] = parseInt(this.customForm.root.get('cpuTargetTemperatureCelsius').value, 10);
        }
        if (this.customForm.root.get('npuHbmTargetTemperatureCelsius').value !==
            this.intelSpeedAdjust.getHbmTargetTemperatureCelsius()) {
            param['NPUHbmTargetTemperatureCelsius'] = parseInt(this.customForm.root.get('npuHbmTargetTemperatureCelsius').value, 10);
        }
        if (this.customForm.root.get('npuAiCoreTargetTemperatureCelsius').value !==
            this.intelSpeedAdjust.getAiTargetTemperatureCelsius()) {
            param['NPUAiCoreTargetTemperatureCelsius'] = parseInt(this.customForm.root.get('npuAiCoreTargetTemperatureCelsius').value, 10);
        }
        if (this.customForm.root.get('outletTargetTemperatureCelsius').value !==
            this.intelSpeedAdjust.getOutletTargetTemperatureCelsius()) {
            param['OutletTargetTemperatureCelsius'] = parseInt(this.customForm.root.get('outletTargetTemperatureCelsius').value, 10);
        }
        const fanSpeedPercent = [];
        this.customForm.root.get('fanSpeedPercentsList').value.forEach((element) => {
            fanSpeedPercent.push( parseInt(element, 10) );
        });
        if (fanSpeedPercent.toString() !== this.intelSpeedAdjust.getFanSpeedPercents().toString()) {
            param['FanSpeedPercents'] = fanSpeedPercent;
        }
        return param;
    }

    private getParam(selectId: string) {
        let param = {
            SmartCooling: {}
        };
        if (selectId === this.intelSpeedAdjust.getFanSmartCoolingMode()) {
            if (selectId === 'Custom') {
                const customParam = this.getCustomModeParam();
                if (JSON.stringify(customParam) !== '{}') {
                    param.SmartCooling['CustomConfig'] = customParam;
                }
            }
        } else {
            param.SmartCooling['Mode'] = selectId;
            if (selectId === 'Custom') {
                const customParam = this.getCustomModeParam();
                if (JSON.stringify(customParam) !== '{}') {
                    param.SmartCooling['CustomConfig'] = customParam;
                }
            }
        }
        if (JSON.stringify(param.SmartCooling) === '{}') {
            param = null;
        }
        this.param = param;
        return param;
    }

    private getFanMode() {
        const coolingMedium = this.intelSpeedAdjust.getCoolingMedium();
        const fanSmartCoolingMode = this.intelSpeedAdjust.getFanSmartCoolingMode();
        if (coolingMedium === 'AirCooled') {
            // 风冷模式
            this.liquidCooling.isShow = false;
        } else if (coolingMedium === 'LiquidCooled') {
            // 液冷模式
            this.energySaving.isShow = false;
            this.lowNoise.isShow = false;
            this.highPerformance.isShow = false;
        }

        switch (fanSmartCoolingMode) {
            case 'EnergySaving':
                // 节能模式
                this.energySaving.isSelect = true;
                break;
            case 'LowNoise':
                // 低噪音模式
                this.lowNoise.isSelect = true;
                break;
            case 'HighPerformance':
                // 高性能模式
                this.highPerformance.isSelect = true;
                break;
            case 'LiquidCooling':
                // 液冷模式
                this.liquidCooling.isSelect = true;
                break;
            case 'Custom':
                // 用户自定义模式
                this.custom.isSelect = true;
                break;
            default:
                break;
        }
    }

    private getTempRangeList() {
        const tabelLabel = [];
        const temperatureRangeCelsius = this.intelSpeedAdjust.getTemperatureRangeCelsius() || [];
        temperatureRangeCelsius.forEach((element, index) => {
            if (index === 0) {
                tabelLabel.push({
                    label: `T < ${element}`
                });
            } else if (index === temperatureRangeCelsius.length - 1) {
                tabelLabel.push({
                    label: `${temperatureRangeCelsius[index - 1]} ≤ T < ${element}`
                });
                tabelLabel.push({
                    label: `T ≥ ${element}`
                });
            } else {
                tabelLabel.push({
                    label: `${temperatureRangeCelsius[index - 1]} ≤ T < ${element}`
                });
            }
        });
        this.temperatureRangeList = tabelLabel;
    }

    private init() {
        this.getFanMode();
        this.getTempRangeList();
        const minCPU = this.intelSpeedAdjust.getMinCPUTargetTemperatureCelsius();
        const maxCPU = this.intelSpeedAdjust.getMaxCPUTargetTemperatureCelsius();
        const minHbm = this.intelSpeedAdjust.getMinHbmTargetTemperatureCelsius();
        const maxHbm = this.intelSpeedAdjust.getMaxHbmTargetTemperatureCelsius();
        const minAi = this.intelSpeedAdjust.getMinAiTargetTemperatureCelsius();
        const maxAi = this.intelSpeedAdjust.getMaxAiTargetTemperatureCelsius();
        const minOutlet = this.intelSpeedAdjust.getMinOutletTargetTemperatureCelsius();
        const maxOutlet = this.intelSpeedAdjust.getMaxOutletTargetTemperatureCelsius();
        const minFan = this.intelSpeedAdjust.getMinFanSpeedPercents();
        const maxFan = this.intelSpeedAdjust.getMaxFanSpeedPercents();
        const fanSpeedPercents = this.intelSpeedAdjust.getFanSpeedPercents() || [];
        const tabelValue = fanSpeedPercents.map((fanSpeedPercent, index) => {
            return [fanSpeedPercent, [TiValidators.digits, TiValidators.rangeValue(minFan, maxFan)]];
        });
        this.customForm = this.fb.group({
            cpuTargetTemperatureCelsius: [this.intelSpeedAdjust.getCPUTargetTemperatureCelsius(),
                [TiValidators.digits, TiValidators.rangeValue(minCPU, maxCPU)]],
            npuHbmTargetTemperatureCelsius: [this.intelSpeedAdjust.getHbmTargetTemperatureCelsius(),
                [TiValidators.digits, TiValidators.rangeValue(minHbm, maxHbm)]],
            npuAiCoreTargetTemperatureCelsius: [this.intelSpeedAdjust.getAiTargetTemperatureCelsius(),
                [TiValidators.digits, TiValidators.rangeValue(minAi, maxAi)]],
            outletTargetTemperatureCelsius: [this.intelSpeedAdjust.getOutletTargetTemperatureCelsius(),
                [TiValidators.digits, TiValidators.rangeValue(minOutlet, maxOutlet)]],
            fanSpeedPercentsList: this.fb.array(tabelValue)
        });
        this.cpuValidation.tip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [minCPU, maxCPU]);
        this.hdmValidation.tip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [minHbm, maxHbm]);
        this.aiValidation.tip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [minAi, maxAi]);
        this.outletValidation.tip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [minOutlet, maxOutlet]);
        this.fanSpeedValidation.tip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [minFan, maxFan]);
        if (!this.custom.isSelect) {
            this.customForm.disable();
        }
    }

    ngOnInit(): void {
        this.init();
    }
    /**
     * 模板中实际调用的是Modal服务提供的close和dismiss方法，并非此处定义的方法；
     * 在此处定义close和dismiss方法只是为了避免生产环境打包时报错
     */
    close(): void {
    }
    dismiss(): void {
    }

}
