import { AbstractControl, ValidatorFn, ValidationErrors } from '@angular/forms';

export class PowerControlValid {
    public static decimalPlaces(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                decimalPlaces: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            const reg = /^((\d+)|(\d+\.\d))$/;
            if (reg.test(value)) {
                return null;
            } else {
                return errorModel;
            }

        };
    }
}
