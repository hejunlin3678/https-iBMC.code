import { isOtherEmptyProperty } from 'src/app/common-app/utils';
import { GlobalDataService, PRODUCTTYPE } from 'src/app/common-app/service';

export class DetailInfo {
    private name: [string, string];
    private position: [string, string];
    private manufacturer: [string, string];
    private capacityMiB: [string, string];
    private operatingSpeedMhz: [string, string];
    private allowedSpeedsMHz: [string, string];
    private memoryDeviceType: [string, string];
    private minVoltageMillivolt: [string, string];
    private typeDetail: [string, string];
    private dataWidthBits: [string, string];
    private partNumber: [string, string];
    private PANK: [string, string];
    private bomNumber: [string, string];
    private serialNumber: [string, string];
    private dimmTemp: [string, string];

    constructor(
        name: string,
        position: string,
        manufacturer: string,
        capacityMiB: string,
        operatingSpeedMhz: string,
        allowedSpeedsMHz: string,
        memoryDeviceType: string,
        minVoltageMillivolt: string,
        typeDetail: string,
        dataWidthBits: string,
        partNumber: string,
        PANK: string,
        bomNumber: string,
        serialNumber: string,
        dimmTemp: string,
        platForm: string,
        private globalData: GlobalDataService
    ) {
        this.name = ['COMMON_NAME', name];
        this.position = ['MEMORY_POSITION', position];
        this.manufacturer = ['COMMON_MANUFACTURER', manufacturer];
        this.capacityMiB = ['COMMON_CAPACITY', capacityMiB];
        this.operatingSpeedMhz = ['MEMORY_CONFIGURED_SPEED_MTS', operatingSpeedMhz];
        this.allowedSpeedsMHz = ['MEMORY_MAX_SPEED_MTS', allowedSpeedsMHz];
        this.memoryDeviceType = ['COMMON_TYPE', memoryDeviceType];
        this.minVoltageMillivolt = ['MEMORY_MIN_VOLTAGE_MILLIVOLT', minVoltageMillivolt];
        this.typeDetail = ['MEMORY_TECHNOLOGY', typeDetail];
        this.dataWidthBits = ['MEMORY_DATAWIDTH_BITS', dataWidthBits];
        if (partNumber !== undefined) {
            this.partNumber = ['OTHER_PART_NUMBER', isOtherEmptyProperty(partNumber)];
        }
        this.PANK = ['MEMORY_PANK', PANK];
        if (platForm !== 'Arm' && bomNumber !== undefined) {
            this.bomNumber = ['MEMORY_BOM_NUMBER', isOtherEmptyProperty(bomNumber)];
        }
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        if (this.globalData.productType === PRODUCTTYPE.TIANCHI) {
            this.dimmTemp = ['DIMM_TEMP', dimmTemp];
        }
    }
}
