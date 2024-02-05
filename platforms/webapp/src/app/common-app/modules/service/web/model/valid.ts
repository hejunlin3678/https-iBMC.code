import { AbstractControl, ValidatorFn, ValidationErrors } from '@angular/forms';

export class CustomValidators {
    public static isEqualHTTPS(comparisonValue: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                isEqualTo: {
                    tiErrorMessage: 'error'
                }
            };
            const groupName = control.root.get('webHttpsControl');
            if (groupName) {
                if (parseInt(value, 10) === parseInt(groupName.value, 10)) {
                    return errorModel;
                } else {
                    return null;
                }
            }
        };
    }
    public static isEqualHTTP(comparisonValue: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                isEqualTo: {
                    tiErrorMessage: 'error'
                }
            };
            const groupName = control.root.get('webHttpControl');
            if (groupName) {
                if (parseInt(value, 10) === parseInt(groupName.value, 10)) {
                    return errorModel;
                } else {
                    return null;
                }
            }
        };
    }
}
