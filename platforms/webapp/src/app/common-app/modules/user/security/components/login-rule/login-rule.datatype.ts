import { UntypedFormGroup, UntypedFormControl } from '@angular/forms';

export interface ITableRow {
    memberId: string;
    ruleName: string;
    startTime: string;
    endTime: string;
    isSave: boolean;
    isEdit: boolean;
    IP: string;
    mac: string;
    ruleEnabled: boolean;
    offLabel: string;
    onLabel: string;
    disable: boolean;
    timeForm: UntypedFormGroup;
    ipValidation: UntypedFormControl;
    macValidation: UntypedFormControl;
}
