import { AbstractControl, ValidationErrors } from '@angular/forms';
import { VALID_PRODUCT_TAG } from 'src/app/common-app/utils';

export default class Validator {
    constructor(private i18n) { }

    public tagValid = (control: AbstractControl): ValidationErrors | null => {
        const value = control.value;
        if (value === '') {
            return null;
        }
        if (!VALID_PRODUCT_TAG.test(value)) {
            return {
                isIncorrect: {
                    tiErrorMessage: this.i18n.COMMON_FORMAT_ERROR
                }
            };
        }

        return null;
    }

    public positionValid = (control: AbstractControl): ValidationErrors | null => {
        const value = control.value;
        if (value === '') {
            return null;
        }
        const reg = /^[a-zA-Z0-9~`!\?,.:;\-_'"\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]{0,64}$/;
        if (!reg.test(value)) {
            return {
                isIncorrect: {
                    tiErrorMessage: this.i18n.COMMON_FORMAT_ERROR
                }
            };
        }
        return null;
    }
}
