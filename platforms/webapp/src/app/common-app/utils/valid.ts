import { AbstractControl, ValidatorFn, ValidationErrors } from '@angular/forms';
import { IPV6, IP_REGEX_V4, DOMAIN_NAME, VALID_BASELINE } from './common';

export class SerAddrValidators {
    public static validateIp46AndDomin(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                pattern: {
                    tiErrorMessage: 'error'
                }
            };
            if (value === '') {
                return null;
            }
            if (/^[0-9.]+$/.test(value)) {
                const reuslt = IP_REGEX_V4.test(value);
                return !reuslt ? errorModel : null;
            } else if (value.indexOf(':') > -1) {
                const reuslt = IPV6.test(value);
                return !reuslt ? errorModel : null;
            } else {
                const reuslt = DOMAIN_NAME.test(value);
                return !reuslt ? errorModel : null;
            }
        };
    }
    public static validateIp46(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                validateIp46: {
                    tiErrorMessage: 'error'
                }
            };
            if (value === '' || value === null) {
                return null;
            }
            if (/^[0-9.]+$/.test(value)) {
                const reuslt = IP_REGEX_V4.test(value);
                return !reuslt ? errorModel : null;
            } else if (value.indexOf(':') > -1) {
                const reuslt = IPV6.test(value);
                return !reuslt ? errorModel : null;
            } else {
                return errorModel;
            }
        };
    }
    public static validateIp6(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                validateIp6: {
                    tiErrorMessage: 'error'
                }
            };
            if (value === '' || value === null) {
                return null;
            }
            if (value.indexOf(':') > -1) {
                const reuslt = IPV6.test(value);
                return !reuslt ? errorModel : null;
            } else {
                return errorModel;
            }
        };
    }
    public static validateIp4Exclude(list: string[]): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                pattern: {
                    tiErrorMessage: 'error'
                }
            };
            if (value === '') {
                return null;
            }
            if (/^[0-9.]+$/.test(value)) {
                const reuslt = list.includes(value);
                return reuslt ? errorModel : null;
            }
            return null;
        };
    }

    public static validateBaseLine(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                pattern: {
                    tiErrorMessage: 'error'
                }
            };
            if (value === '') {
                return null;
            }
            const reuslt = VALID_BASELINE.test(value);
            return !reuslt ? errorModel : null;
        };
    }
}
