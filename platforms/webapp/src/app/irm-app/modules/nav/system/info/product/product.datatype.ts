import { UntypedFormControl, ValidatorFn } from '@angular/forms';

export interface IItem {
    label: string;
    title?: string;
    id?: CabinetInfoId | BaseInfoId;
    content?: string;
    hide?: boolean;
    editable?: boolean;
    editableDate?: boolean;
    format?: string;
    confirm?: (param: string) => void;
    maxSize?: number;
    controll?: UntypedFormControl;
    validationRules?: ValidatorFn[];
    validation?: any;
}

export enum CabinetInfoId {
    cabinetPosition = 'cabinetPosition',
    cabinetName = 'Name',
    cabinetRack = 'Rack',
    cabinetRow = 'Row',
    ratedPower = 'RatedPowerWatts',
    loadCapacityKg = 'LoadCapacityKg',
    totalUCount = 'TotalUCount',
    cabinetUse = 'UcountUsed',
}

export enum BaseInfoId {
    productModel = 'productModel',
    productTag = 'productTag',
    serialNumberBase = 'serialNumberBase',
    partNumberBase = 'partNumberBase',
    vendor = 'vendor',
    firmwareVersion = 'firmwareVersion',
    veneerID = 'veneerID',
}

export type IInfo = {
    [key in CabinetInfoId | BaseInfoId]?:
    { value: string; hide: boolean; } | string;
};

export interface IUpdateBody {
    AssetTag?: string;
    RackInfo?: { [key: string]: any };
}
