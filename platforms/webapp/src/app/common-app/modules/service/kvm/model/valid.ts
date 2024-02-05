import { AbstractControl, ValidatorFn, ValidationErrors } from '@angular/forms';

export class AutoKVMValidators {
    public static isNotEmpty(comparisonValue1: string, comparisonValue2: string, comparisonValue3: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                isNotEmpty: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            const keyBoard2 = control.root.get(comparisonValue1);
            const keyBoard3 = control.root.get(comparisonValue2);
            const keyBoard4 = control.root.get(comparisonValue3);
            if (keyBoard2 && keyBoard3 && keyBoard4) {
                if (!value && !keyBoard2.value && !keyBoard3.value && !keyBoard4.value) {
                    return errorModel;
                } else {
                    return null;
                }
            }
        };
    }
}
