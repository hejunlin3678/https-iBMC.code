import { ValidatorFn, AbstractControl, ValidationErrors } from '@angular/forms';

export class StorageVaild {
    // 自定义校验规则
    public static assic15(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value ? control.value.split('') : [];
            let msg: ValidationErrors = null;
            value.forEach(code => {
                if (code.charCodeAt() < 32 || code.charCodeAt() > 126) {
                    msg = {
                        pattern: {
                            tiErrorMessage: 'error'
                        }
                    };
                }
            });
            return  msg;
        };
    }

    // 自定义校验规则
    public static spanCount(min, max): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            let msg: ValidationErrors = null;
             // 正整数校验
            const REGULAR_INT = /^[0-9]+$/ ;
            if (control.value !== null &&
                (!REGULAR_INT.test(control.value) ||
                Number(control.value) < min ||
                Number(control.value) > max)
            ) {
                msg = {
                    pattern: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return  msg;
        };
    }

    // 自定义校验规则(值为n的倍数)
    public static multiples(n): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            let msg: ValidationErrors = null;
             // 正整数校验
            const REGULAR_INT = /^[0-9]+$/ ;
            if (control.value !== null && (!REGULAR_INT.test(control.value) || Number(control.value) % n !== 0)
            ) {
                msg = {
                    pattern: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return  msg;
        };
    }
}
