export interface ISpeedAdjustmentMode {
    id: string;
    label: string;
    content: string;
    isSelect: boolean;
    isShow: boolean;
    tip?: string;
}
export interface IFanLabel {
    model: string;
    speed: string;
    rate: string;
    partNumber: string;
}

export interface IValidationTip {
    tip: string;
    tipPosition: string;
}
