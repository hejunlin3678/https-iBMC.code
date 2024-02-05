import { ValidatorFn, AbstractControl, ValidationErrors } from '@angular/forms';
import { getByte } from 'src/app/common-app/utils';

export class ChassisVaild {
    // 自定义校验规则 字节长度小于20
    public static bytesLength(wanderLength: number = 20): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            let msg: ValidationErrors = null;
            if (getByte(value) > wanderLength) {
                msg = {
                    bytesLength: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return  msg;
        };
    }

    // 自定义校验规则 -只能输入字母数字特殊字符
    public static noChineseCharacters (): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            let msg: ValidationErrors = null;
             // 字母数字特殊字符校验
            const NO_CHINESE_CHAEACTERS = /^[a-zA-Z0-9`~!@#$%^&*()-_=+\|\[{}\];:'",<.>/?]+$/;
            if (value !== '' && !NO_CHINESE_CHAEACTERS.test(value)) {
                msg = {
                    noChineseCharacters: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return  msg;
        };
    }
}
