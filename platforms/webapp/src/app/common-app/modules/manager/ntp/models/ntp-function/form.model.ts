import { UntypedFormControl } from '@angular/forms';
import { TiValidationConfig } from '@cloud/tiny3';

export class NTPFormControl extends UntypedFormControl {
    id: string;
    label: string;
    value: string;
    isValid: string;

    modelProperty: string;
    validation: TiValidationConfig;

    constructor(label: string, property: string, value: string, validator: any, validation: TiValidationConfig) {
        super(value, validator);
        this.label = label;
        this.modelProperty = property;
        validation.type = 'change';
        this.validation = validation;
    }

    getValidation(): TiValidationConfig {
        return this.validation;
    }

    get getLabel(): string {
        return this.label;
    }

}


