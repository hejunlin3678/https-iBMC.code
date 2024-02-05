import { AbstractControl, ValidationErrors, UntypedFormControl } from '@angular/forms';
import { TiValidationConfig, TiValidators } from '@cloud/tiny3';

export default class AuthorityValidator {

    private readonly MAX_DAY: number = 365;

    // 密码有效期校验规则
    private periodValid: UntypedFormControl;

    // 密码最小长度配置
    private passwordMinLen: UntypedFormControl;

    private leastPeriodValid: UntypedFormControl;

    private expiresValid: UntypedFormControl;

    private inactivityValid: UntypedFormControl;

    private loginFailValid: UntypedFormControl;

    public tiCommonValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: '',
            integer: this.i18n.VALID_INTEGER_INFO,
            rangeValue: this.i18n.VALID_VALUE_RANGE_TIP
        }
    };

    public setPeriodValid(initValue: string = '0', minValue: number = 0): void {
        const moreValidator = (control: AbstractControl): ValidationErrors | null => {
            const currentValue = Number(control.value);
            if (currentValue !== 0 && minValue >= 355) {
                return {
                    isOnly: {
                        tiErrorMessage: this.i18n.VALIDATOR_PASSWORD_USAGE_PERIOD
                    }
                };
            } else if (currentValue !== 0 && minValue !== 0 && (currentValue - 10 <= minValue)) {
                return {
                    isMoreThan: {
                        tiErrorMessage: this.i18n.VALIDATOR_PASSWORD_PERIOD_BIG
                    }
                };
            } else {
                return null;
            }
        };

        const validator = [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(0, this.MAX_DAY),
            moreValidator
        ];
        if (this.periodValid) {
            this.periodValid.clearValidators();
            this.periodValid.setValidators(validator);
            this.periodValid.updateValueAndValidity();
            this.periodValid.markAsTouched();
        } else {
            this.periodValid = new UntypedFormControl(initValue, validator);
        }
    }

    public setPasswordMinLen(initValue: string = '8'): void {
        const moreValidator = (control: AbstractControl): ValidationErrors | null => {
            const currentValue = Number(control.value);
            if (currentValue < 8 || currentValue > 20) {
                return {
                    isMoreThan: {
                        tiErrorMessage: this.i18n.VALIDATOR_PASSWORD_MIN_LEN
                    }
                };
            }
        };

        this.passwordMinLen = new UntypedFormControl(initValue, [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(8, 20),
            moreValidator
        ]);
    }

    public getPeriodValid(): UntypedFormControl {
        return this.periodValid;
    }

    public getPasswordMinLen(): UntypedFormControl {
        return this.passwordMinLen;
    }

    public setLeastPeriodValid(initValue: string = '0', maxValue: number = this.MAX_DAY): void {
        const lessValidator = (control: AbstractControl): ValidationErrors | null => {
            const currentValue = Number(control.value);
            if (currentValue !== 0 && maxValue > 0 && maxValue < 10) {
                return {
                    isOnly: {
                        tiErrorMessage: this.i18n.VALIDATOR_VALIDATOR_PASSWORD_ONLY
                    }
                };
            } else if (currentValue !== 0 && maxValue !== 0 && (currentValue + 10 >= maxValue)) {
                return {
                    isLessThan: {
                        tiErrorMessage: this.i18n.VALIDATOR_PASSWORD_PERIOD
                    }
                };
            } else {
                return null;
            }
        };

        const validator = [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(0, this.MAX_DAY),
            lessValidator
        ];
        if (this.leastPeriodValid) {
            this.leastPeriodValid.clearValidators();
            this.leastPeriodValid.setValidators(validator);
            this.leastPeriodValid.updateValueAndValidity();
            this.leastPeriodValid.markAsTouched();
        } else {
            this.leastPeriodValid = new UntypedFormControl(initValue, validator);
        }
    }

    public getLeastPeriodValid(): UntypedFormControl {
        return this.leastPeriodValid;
    }

    public setExpiresValid(initValue: string = '90'): void {
        const min = 7;
        const max = 180;
        this.expiresValid = new UntypedFormControl(initValue, [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(min, max),
        ]);
    }

    public getExpiresValid(): UntypedFormControl {
        return this.expiresValid;
    }

    public getInactivityValid(): UntypedFormControl {
        return this.inactivityValid;
    }

    public setInactivityValid(initValue: string = '0'): void {
        const minValue = 30;
        const maxValue = 365;
        const validator = (control: AbstractControl): ValidationErrors | null => {
            const currentValue = Number(control.value);
            if (currentValue === 0 || (currentValue >= minValue && currentValue <= maxValue)) {
                return null;
            } else {
                return {
                    notInRange: {
                        tiErrorMessage: this.i18n.SECURITY_PERIOD_NOLIMITED
                    }
                };
            }
        };
        this.inactivityValid = new UntypedFormControl(initValue, [
            TiValidators.required,
            TiValidators.integer,
            validator
        ]);
    }

    public getLoginFailValid(): UntypedFormControl {
        return this.loginFailValid;
    }

    public setLoginFailValid(initValue: string = '1'): void {
        const min = 1;
        const max = 30;
        this.loginFailValid = new UntypedFormControl(initValue, [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(min, max),
        ]);
    }

    constructor(private i18n: any) {
        this.setPeriodValid();
        this.setPasswordMinLen();
        this.setLeastPeriodValid();
        this.setExpiresValid();
        this.setInactivityValid();
        this.setLoginFailValid();
    }

}
