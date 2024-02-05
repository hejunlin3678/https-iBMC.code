import { FormControl, ValidatorFn } from '@angular/forms';

export interface IItem {
    label: string;
    title?: string;
    id?: productId | mainBoardId | systemId | DigitalWarrantyId;
    content?: string;
    hide?: boolean;
    editable?: boolean;
    editableDate?: boolean;
    format?: string;
    confirm?: (param: string) => void;
    maxSize?: number;
    controll?: FormControl;
    validationRules?: ValidatorFn[];
    validation?: any;
}

export enum productId {
    name = 'name',
    serialNum = 'serialNum',
    assetTag = 'assetTag',
    position = 'position',
    productManufacturer = 'productManufacturer',
    bomNumber = 'bomNumber'
}

export enum mainBoardId {
    ibmcVersion = 'ibmcVersion',
    boardId = 'boardId',
    biosVersion = 'biosVersion',
    boardManu = 'boardManu',
    cpldVersion = 'cpldVersion',
    boardVersion = 'boardVersion',
    mainBoot = 'mainBoot',
    serial = 'serial',
    backupBoot = 'backupBoot',
    backupSerial = 'backupSerial',
    pchVersion = 'pchVersion',
    chasisId = 'chasisId',
    particle = 'particle',
    deviceSlotID = 'deviceSlotID',
    pcbVersion = 'pcbVersion',
    partNumber = 'partNumber',
}

export enum systemId {
    localDomain = 'localDomain',
    ibmaService = 'ibmaService',
    description = 'description',
    ibmaStatus = 'ibmaStatus',
    kernalVersion = 'kernalVersion',
    ibmaDriver = 'ibmaDriver',
    domain = 'domain',
    operator = 'operator',
}

export enum DigitalWarrantyId {
    productNamePolicy = 'productNamePolicy',
    serialNumber = 'serialNumber',
    manufactureDate = 'manufactureDate',
    UUID = 'UUID',
    unitType = 'unitType',
    startPoint = 'startPoint',
    lifespan = 'lifespan',
}

export type IInfo = {
    [key in productId | systemId | mainBoardId | DigitalWarrantyId]?: { value: string; hide: boolean; } | string;
} & {
    showSysInfo?: boolean;
    showSmsEnabled?: boolean;
};

export interface IUpdateBody {
    AssetTag?: string;
    DeviceLocation?: string;
    DigitalWarranty?: {};
}
