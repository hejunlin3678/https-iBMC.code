import { DetailInfo } from './detailInfo.model';
import { AEPInfo } from './aepInfo.model';

export class Memory {
    private name: string;
    private manufacturer: string;
    private capacityMiB: string;
    private operatingSpeedMhz: string;
    private allowedSpeedsMHz: string;
    private memoryDeviceType: string;
    private position: string;
    private detailInfo: DetailInfo;
    private aepInfo: AEPInfo;
    private platForm: string;

    constructor(
        name: string,
        manufacturer: string,
        capacityMiB: string,
        operatingSpeedMhz: string,
        allowedSpeedsMHz: string,
        memoryDeviceType: string,
        position: string,
        detailInfo: DetailInfo,
        aepInfo: AEPInfo,
        platForm: string,
    ) {
        this.name = name;
        this.manufacturer = manufacturer;
        this.capacityMiB = capacityMiB;
        if (platForm !== 'Arm') {
            this.operatingSpeedMhz = operatingSpeedMhz;
        }
        this.allowedSpeedsMHz = allowedSpeedsMHz;
        this.memoryDeviceType = memoryDeviceType;
        this.position = position;
        this.detailInfo = detailInfo;
        this.aepInfo = aepInfo;
    }

    get getDetailInfo(): DetailInfo {
        return this.detailInfo;
    }

    get getAEPInfo(): AEPInfo {
        return this.aepInfo;
    }
}
