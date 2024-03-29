import { Component, OnInit, Input, ViewEncapsulation } from '@angular/core';
import { UntypedFormControl } from '@angular/forms';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { PowerService } from '../../service/power.service';
import { TranslateService } from '@ngx-translate/core';
import { IPowerSwitch, IPowerCapValue } from '../../interface';
import { formatEntry } from 'src/app/common-app/utils';
import { GlobalDataService, PRODUCTTYPE } from 'src/app/common-app/service';

@Component({
    selector: 'app-power-capping-setting',
    templateUrl: './power-capping-setting.component.html',
    styleUrls: ['./power-capping-setting.component.scss'],
    encapsulation: ViewEncapsulation.None
})
export class PowerCappingSettingComponent implements OnInit {
    @Input() powerLimitState: boolean;
    @Input() limitInWatts: number;
    @Input() limitException: string;
    @Input() minPowerLimitInWatts: number;
    @Input() maxPowerLimitInWatts: number;
    @Input() powerUnit: string;
    @Input() param;
    @Input() isLimitState: boolean;
    @Input() capMode: boolean;
    @Input() isArm: boolean;

    constructor(
        private service: PowerService,
        private translate: TranslateService,
        private globalData: GlobalDataService
    ) { }

    public saveBtn = true;

    public capState: IPowerSwitch = {
        label: this.translate.instant('POWER_CAPPING_STATUS'),
        switchState: null,
        change: (() => {
            if (!this.capState.switchState) {
                this.limitInWatts = null;
            }
            this.monitorBtn();
        })
    };
    public capValueTip: TiValidationConfig =  {
        tip: null,
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            required: this.translate.instant('VALID_POWER_CAPPING_VALUE_NOT_RANGE'),
            digits: this.translate.instant('VALID_INTEGER_INFO'),
            number: this.translate.instant('VALID_NUMBER_INFO'),
            rangeValue: this.translate.instant('VALID_POWER_CAPPING_VALUE_NOT_RANGE')
        }
    };
    public capValue: IPowerCapValue = {
        label: this.translate.instant('POWER_CAPPING_VALUE'),
        control: null,
        value: null,
        errorShow: false,
        change: (() => {
            if (this.capValue.value === '') {
                this.capValue.value = null;
            }
            if (this.capValue.control.valid) {
                this.capValue.errorShow = this.capValue.value <
                    (this.powerUnit === 'W' ? this.minPowerLimitInWatts : this.service.wattstoBTUs(this.minPowerLimitInWatts));
            } else {
                this.capValue.errorShow = false;
            }
           this.monitorBtn();
        })
    };

    public capfailure: IPowerSwitch = {
        label: this.translate.instant('POWER_CAPPING_FAILURE_POLICY'),
        tip: ((this.globalData.productType === PRODUCTTYPE.TIANCHI) ? this.translate.instant('POWER_CAPPING_INVALID_TIP_TIANCHI') :
            this.translate.instant('POWER_CAPPING_INVALID_TIP')),
        switchState: null,
        value: '',
        show: false,
        options: [
            {
                label: this.translate.instant('POWER_NO_OPERATION'),
                value: 'NoAction'
            },
            {
                label: this.translate.instant('POWER_OFF'),
                value: 'HardPowerOff'
            },
            {
                label: this.translate.instant('POWER_RESTART'),
                value: 'Reset'
            }
        ],
        change: (() => {
            this.monitorBtn();
        })
    };

    public capModes: {label: string, show: boolean, value: string} = {
        label: this.translate.instant('POWER_CAPPING_MODE'),
        show: false,
        value: ''
    };

    private monitorBtn() {
        const valid = this.capValue.control.valid;
        if (this.capState.switchState && !valid) {
            this.saveBtn = true;
            return;
        }
        const param = this.getParam();
        if (param) {
          this.saveBtn = false;
        } else if (this.limitException !== this.capfailure.value) {
            this.saveBtn = false;
        } else {
            this.saveBtn = true;
        }
    }

    private getParam() {
        this.param = null;
        const powerLimit = {};
        if (this.capState.switchState !== this.powerLimitState) {
            if (this.capState.switchState) {
                powerLimit['LimitState'] = 'On';
            } else {
                powerLimit['LimitState'] = 'Off';
            }
        }
        if (this.capState.switchState) {
            if (this.powerUnit === 'W') {
                if (this.capValue.value && this.limitInWatts !== parseInt(String(this.capValue.value), 10)) {
                    powerLimit['LimitInWatts'] = parseInt(String(this.capValue.value), 10);
                }
            } else {
                if (this.capValue.value && this.service.wattstoBTUs(this.limitInWatts) !== Number(this.capValue.value)) {
                    powerLimit['LimitInWatts'] = Math.ceil(Number(this.capValue.value) * 0.293);
                }
            }
        }
        if (this.limitException !== this.capfailure.value) {
            powerLimit['LimitException'] = this.capfailure.value;
        }
        if (JSON.stringify(powerLimit) !== '{}') {
            this.param = {
                PowerLimit: powerLimit
            };
        }
        return this.param;
    }

    close(): void {
    }

    dismiss(): void {
    }

    ngOnInit(): void {
        // 如果不是arm，而是x86或者其他 ，那么不要重启选项
        if (!this.isArm) {
            this.capfailure.options.pop();
        }
        this.capState.switchState = this.powerLimitState;
        this.capfailure.value = this.limitException;
        this.capfailure.show = this.isLimitState;
        if (this.powerUnit === 'W') {
            this.capValue.value = this.limitInWatts;
            this.capValue.label = `${this.translate.instant('POWER_CAPPING_VALUE')} (W)`;
            this.capValue.control = new UntypedFormControl(
                this.limitInWatts,
                [TiValidators.required, TiValidators.digits, TiValidators.rangeValue( 1, this.maxPowerLimitInWatts)]);
            this.capValueTip.tip = formatEntry(this.translate.instant('VALID_RECOMMENDED_VALUE_RANGE_TIP1'),
                [this.minPowerLimitInWatts, this.maxPowerLimitInWatts]);
        } else {
            this.capValue.value = this.service.wattstoBTUs(this.limitInWatts);
            this.capValue.label = `${this.translate.instant('POWER_CAPPING_VALUE')} (BTU/h)`;
            this.capValue.control = new UntypedFormControl(
                this.service.wattstoBTUs(this.limitInWatts),
                [TiValidators.required, TiValidators.number,
                TiValidators.rangeValue(1, this.service.wattstoBTUs(this.maxPowerLimitInWatts))]
            );
            this.capValueTip.tip = formatEntry(this.translate.instant('VALID_RECOMMENDED_VALUE_RANGE_TIP2'),
                [this.service.wattstoBTUs(this.minPowerLimitInWatts), this.service.wattstoBTUs(this.maxPowerLimitInWatts)]);
        }
        this.capModes.show = this.capMode !== null && this.capMode !== undefined;
        this.capModes.value = this.capMode ? this.translate.instant('POWER_PROTECT_MODE') : this.translate.instant('POWER_USER_SET_MODE');
    }

}
