import { ValidatorFn, AbstractControl, ValidationErrors } from '@angular/forms';

export class PowerVaild {
    /**
     * 自定义小数位数校验规则
     * @param num 最大保留小数位数
     * @param err 校验失败的时候提示错误信息
     */
    public static decimalPlacesNumber(num: number, err: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            let msg: ValidationErrors = null;
            if (value && value.length) {
                const index = value.indexOf('.') + 1;
                if (index >= 1 && index !== value.length && value.length > index + num) {
                    msg = {
                        pattern: {
                            tiErrorMessage: err
                        }
                    };
                }
            }
            return  msg;
        };
    }
}
