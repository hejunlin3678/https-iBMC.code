import { NTPFormControl } from './form.model';
import { UntypedFormGroup } from '@angular/forms';

export class NTPFormGroup extends UntypedFormGroup {
    constructor(ntpControl: any) {
        super(ntpControl);
    }

    get ntpControls(): NTPFormControl[] {
        return Object.keys(this.controls).map(k => this.controls[k] as NTPFormControl);
    }

}


