import { AbstractControl, ValidationErrors } from '@angular/forms';

export default class LoginRuleValidator {
    constructor(private i18n) {}

    private getErrorMsg = (num: number): ValidationErrors | null => {
         // 时间格式校验错误提示信息
         const errorMssage = {
            1: 'VALIDATOR_TIME_FORMAT_INCORRECT',
            2: 'VALIDATOR_START_ENDING',
            3: 'VALIDATOR_RIGHT_TIME1',
            4: 'VALIDATOR_RIGHT_TIME2',
            5: 'VALIDATOR_WRONG_TIME1',
            6: 'VALIDATOR_WRONG_TIME2'
        };
        return {
            isIncorrect: {
                tiErrorMessage: this.i18n[errorMssage[num]]
            }
        };
    }

    public selfTimeValid = (control: AbstractControl): ValidationErrors | null => {
        const value = control.value;
        if (value === '' || value === null) {
            return null;
        }
        // 支持的时间格式
        const reg = /(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}$)|(^\d{4}-\d{2}-\d{2}$)|(^\d{2}:\d{2})/;
        if (!reg.test(value)) {
            return this.getErrorMsg(1);
        }

        let transformedValue = value;
        if (value.indexOf('-') === -1) {
            transformedValue = '1970-01-01 ' + value;
        }
        transformedValue = transformedValue.replace(' ', 'T');
        if (transformedValue.indexOf('24:') > -1 || new Date(transformedValue).toString() === 'Invalid Date') {
            return this.getErrorMsg(1);
        }

        const MIN_YEAR = 1970;
        const MAX_YEAR = 2050;
        const transformedValueYear = new Date(transformedValue).getFullYear();
        if (transformedValueYear < MIN_YEAR || transformedValueYear > MAX_YEAR) {
            return this.getErrorMsg(2);
        }

        return null;
    }

    public ipValid = (control: AbstractControl): ValidationErrors | null => {
        const value = control.value;
        if (value === '' || value == null) {
            return null;
        }
        const reg = /^((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]\d)|[1-9])(\.((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]\d)|\d)){3}\/(([12][0-9]?)|(3[0-2])|([3-9]))$/;
        if (!reg.test(value)) {
            const reg1 = /^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$/;
            if (!reg1.test(value)) {
                return {
                    formatError: {
                        tiErrorMessage: this.i18n.VALIDATOR_INCORRECT
                    }
                };
            }
        }
        return null;
    }

    public interTimeValid = (isStart: boolean, interTime: string) => {
        return (control: AbstractControl): ValidationErrors | null => {
            const time = control.value;
            if (interTime === null || time === null) {
                return null;
            }
            // 验证开始、结束时间提示
            if (!isStart) {
                if (interTime.length !== time.length) {
                    return this.getErrorMsg(4);
                }
            } else {
                if (interTime && (!time || (interTime.length !== time.length))) {
                    return this.getErrorMsg(3);
                }
            }
            let transformdInterTime = interTime;
            let transformdTime = time;
            if (interTime.indexOf('-') === -1) {
                const extraDateValue = '1970-01-01T';
                transformdInterTime = extraDateValue + transformdInterTime;
                transformdTime = extraDateValue + transformdTime;
            }

            if (isStart && new Date(transformdInterTime) < new Date(transformdTime)) {
                return this.getErrorMsg(5);
            }

            if (!isStart && new Date(transformdInterTime) >= new Date(transformdTime)) {
                return this.getErrorMsg(6);
            }

            return null;
        };
    }

    public macValid = (control: AbstractControl): ValidationErrors | null => {
        const value = control.value;
        if (value === '' || value == null) {
            return null;
        }
        const reg6 = /^([A-Fa-f0-9]{2}[:]){5}[A-Fa-f0-9]{2}$/;
        const reg3 = /^([A-Fa-f0-9]{2}[:]){2}[A-Fa-f0-9]{2}$/;
        if (!(reg6.test(value) || reg3.test(value))) {
            return {
                macError: {
                    tiErrorMessage: this.i18n.VALIDATOR_MAC_ERROR
                }
            };
        } else {
            return null;
        }
    }
}
