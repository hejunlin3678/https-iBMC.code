import { Component, EventEmitter, Input, OnChanges, Output, SimpleChanges } from '@angular/core';
import { UntypedFormBuilder, UntypedFormGroup } from '@angular/forms';
import { TiValidators } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service';
import { formatEntry } from 'src/app/common-app/utils';
import { Chassis } from '../../model/chassis';

@Component({
    selector: 'app-chassis-power-capping',
    templateUrl: './chassis-power-capping.component.html',
    styleUrls: ['./chassis-power-capping.component.scss']
})
export class ChassisPowerCappingComponent implements OnChanges {

    @Input() chassis: Chassis;
    @Output() private outer = new EventEmitter();

    public validCapValue;
    public validCapThreshold;
    public isPrivileges = this.user.hasPrivileges(['OemPowerControl']);
    public chassisForm: UntypedFormGroup = this.fb.group({
        powerCapEnable: [{ value: true, disabled: !this.isPrivileges }, []],
        capValueForm: [
            { value: null, disabled: !this.isPrivileges },
            [
                TiValidators.required,
                TiValidators.digits
            ]
        ],
        capThresholdForm: [
            { value: null, disabled: !this.isPrivileges },
            [
                TiValidators.required,
                TiValidators.digits,
                TiValidators.rangeValue(0, 70),
            ]
        ],
        capModes: [{ value: null, disabled: !this.isPrivileges }, []],
    });
    constructor(
        private fb: UntypedFormBuilder,
        private user: UserInfoService,
        private translate: TranslateService
    ) { }

    public powerCapEnable = {
        label: 'POWER_CAPPING_ENABLE',
        switchState: false,
        onNgModelChange: (value: boolean) => {
            this.getParam();
        }
    };

    public actualPower = {
        label: 'POWER_CAPPING_REALTIME',
        value: null
    };

    public capStatus = {
        label: 'POWER_CAPPING_CHESSIS_STATUES',
        value: null
    };

    public capValue = {
        label: 'POWER_CAPPING_CHESSIS_VALUE',
        value: null,
        change: () => {
            this.getParam();
        }

    };

    public capThreshold = {
        label: 'POWER_CAPPING_THRESHOLD',
        value: null,
        change: () => {
            this.getParam();
        }
    };

    public capMode = {
        label: 'POWER_CAPPING_MODE',
        list: [{
            id: 'Equal',
            key: 'POWER_CAPPING_EQUALLY'
        }, {
            id: 'Proportion',
            key: 'POWER_CAPPING_PROPORTIPONS'
        }, {
            id: 'Manual',
            key: 'POWER_CAPPING_MANUAL'
        }],
        value: '',
        change: () => {
            this.getParam();
        }
    };

    private getParam() {
        const param = {};
        if (this.chassisForm.valid) {
            if (this.chassisForm.controls['powerCapEnable'].value !== this.powerCapEnable.switchState) {
                param['PowerLimitEnabled'] = this.chassisForm.controls['powerCapEnable'].value;
            }
            if (Number(this.chassisForm.controls['capValueForm'].value) !== this.capValue.value) {
                param['LimitInWatts'] = Number(this.chassisForm.controls['capValueForm'].value);
            }
            if (Number(this.chassisForm.controls['capThresholdForm'].value) !== this.capThreshold.value) {
                param['PowerLimitThreshold'] = Number(this.chassisForm.controls['capThresholdForm'].value);
            }
            if (this.chassisForm.controls['capModes'].value !== this.capMode.value) {
                param['PowerLimitMode'] = this.chassisForm.controls['capModes'].value;
            }
        }
        this.outer.emit(JSON.stringify(param) !== '{}' ? param : null);
    }

    initValid() {
        this.chassisForm.controls['capValueForm'].setValidators([
            TiValidators.required,
            TiValidators.digits,
            TiValidators.rangeValue(this.chassis.minCapValue, this.chassis.maxCapValue),
        ]);
        this.validCapValue = {
            tip: formatEntry(this.translate.instant('VALID_RANGE_MINIMUM_TIP'),
                [this.chassis.minCapValue, this.chassis.maxCapValue, this.chassis.recommendMinCapValue]),
            errorMessage: {
                required:  formatEntry(this.translate.instant('VALID_RANGE_MINIMUM_TIP'),
                            [this.chassis.minCapValue, this.chassis.maxCapValue, this.chassis.recommendMinCapValue]),
                digits:  formatEntry(this.translate.instant('VALID_RANGE_MINIMUM_TIP'),
                            [this.chassis.minCapValue, this.chassis.maxCapValue, this.chassis.recommendMinCapValue]),
                rangeValue: formatEntry(this.translate.instant('VALID_RANGE_MINIMUM_TIP'),
                            [this.chassis.minCapValue, this.chassis.maxCapValue, this.chassis.recommendMinCapValue]),
            }
        };

        this.validCapThreshold = {
            tip: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 70]),
            errorMessage: {
                required: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 70]),
                digits: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 70]),
                rangeValue: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 70]),
            }
        };
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.chassis && changes.chassis.currentValue) {
            this.initValid();
            this.powerCapEnable.switchState = this.chassis.enable;
            this.actualPower.value = this.chassis.actualPower;
            this.capStatus.value = this.chassis.capStatus;
            this.capValue.value = this.chassis.capValue;
            this.capThreshold.value = this.chassis.capThreshold;
            this.capMode.value = this.chassis.capMode;
            this.chassisForm.patchValue({
                powerCapEnable: this.chassis.enable,
                capValueForm: this.chassis.capValue,
                capThresholdForm: this.chassis.capThreshold,
                capModes: this.chassis.capMode
            });
        }
    }
}
