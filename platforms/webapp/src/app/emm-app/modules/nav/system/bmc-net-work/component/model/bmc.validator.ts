import { AbstractControl, UntypedFormGroup, ValidationErrors, ValidatorFn } from '@angular/forms';
import { checkV6Segment, IPv6Supplement } from './data.func';

export class BmcValidator {
    constructor() { }

    public static validateIp46(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                invalidAddress: {
                    tiErrorMessage: ''
                }
            };
            if (value === '' || value === null || value === undefined) {
                return null;
            }

            const completeIpV6 = IPv6Supplement(value);
            const firstValue = completeIpV6.split(':')[0];
            if (checkV6Segment(value, '0::0', 128) || checkV6Segment(value, '0::1', 128)) {
                return errorModel;
            } else {
                const notEqualList = ['FF', 'FE8', 'FE9', 'FEA', 'FEB'];
                const state = notEqualList.some(item => {
                    return firstValue.toUpperCase().indexOf(item) === 0;
                });

                if (state) {
                    return errorModel;
                }
            }
            return null;
        };
    }

    public static ipv6Required(
        param: { ipName?: string; gateName?: string; prefixName?: string; formGroup: UntypedFormGroup }): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                ipv6Required: {
                    tiErrorMessage: ''
                }
            };
            // 如果前缀长度位为0或无效值，则不校验IP地址或网关
            const prefixControlValue = param.formGroup.get(param.prefixName).value;
            if (prefixControlValue === 0 || prefixControlValue === '' || prefixControlValue === null) {
                return null;
            }

            if (value === '' || value === null) {
                return errorModel;
            }

            return null;
        };
    }
}
