import { FormControl } from '@angular/forms';

export enum Platform {
    ARM = 'Arm'
}

export const W_TO_BTU_COEFF = 0.293;
export interface ICheckList {
    label: string;
    name: string;
    checked: boolean;
    disabled: boolean;
}

export interface IPowerCapLabel {
    label: string;
    value: number;
    desc?: string;
}

export interface IPowerSwitch {
    label: string;
    tip?: string;
    switchState: boolean;
    show?: boolean;
    change(): void;
}
export interface IPowerCapValue {
    label: string;
    control: FormControl ;
    value: number | string;
    errorShow: boolean;
    change(): void;
}

export interface PowerOnPolicy {
    value: string;
    label: string;
    disabled: boolean;
}

export interface PowerOnDelayedEditor {
    show: boolean;
    value?: string;
    editor?(): void;
    cancel?(): void;
    confirm?(): void;
}
export interface PowerOnDelayed {
    value: string;
    label: string;
    disabled: boolean;
    desc: string;
    editor: PowerOnDelayedEditor;
}
