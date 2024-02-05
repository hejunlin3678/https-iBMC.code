import { BootTarget } from './boot-target';
import { BootType } from './boot-type';

export enum PLATFORM {
    ARM = 'Arm',
    X86 = 'X86'
}

export interface IBiosTab {
    title: string;
    active: boolean;
    show: boolean;
    tips?: string;
}

export interface ICPULabel {
    label: string;
    tip?: string;
}

export interface ICPUSlider {
    disabled: boolean;
    min: number;
    tipFormatterFn: (value: string) => { };
}

export interface ICPU {
    controlMethod: ICPULabel;
    operatingFrequency: ICPULabel;
    workingHours: ICPULabel;
    frequency: ICPUSlider;
    freeTime: ICPUSlider;
}

export interface ICUPData {
    CPUPLimit: number;
    CPUTLimit: number;
    CPUPState:  boolean;
    CPUTState: boolean;
    CPUPScales: string[];
    CPUTScales: string[];
}

export interface IBootTitle {
    title: string;
}

export interface IBootMode {
    title: string;
    options: {
        label: string,
        value: string
    }[];
}

export interface IBootMedia {
    title: string;
    radio: { options: {
            label: string,
            value: string
        }[]
    };
}
export interface BootTargetExt {
    id: string;
    label: string;
    children?: {
        label: string,
        id: string
    }[];
}

export interface IBootData {
    bootModeConfigOverIpmiEnabled: boolean;
    bootSourceOverrideMode: string;
    bootSourceOverrideTarget: BootTarget;
    bootSourceOverrideTargetList:  BootTarget[];
    bootSourceOverrideEnabled: string;
    bootTypeOrder: BootType[];
    bootModeIpmiSWDisplayEnabled?: boolean;
    bootSourceOverrideTargetListExt: BootTargetExt[];
    bootSourceOverrideTargetExt: string[];
}
