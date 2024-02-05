import { AbstractControl, ValidationErrors, ValidatorFn } from '@angular/forms';

export class AssetValid {
    public static getRepeat(data, value): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            let count = 0;
            data.forEach(item => {
                if (item.key === value) {
                    count++;
                }
            });
            if (count > 1) {
                return {
                    patternRepeat: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return null;
        };
    }
}
