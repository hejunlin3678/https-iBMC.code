import { IOptions } from 'src/app/common-app/models';
import { VolumeStaticModel } from '../static-data';

export class PMCEditModel {
    private name: [string, string];
    private bootPriority: [string, IOptions];
    private capacityBytes: [string, string];
    private accelerationMethod: [string, IOptions];
    private stripeSizeBytes: [string, IOptions];
    private capacityUnit: [string, IOptions];
    private defaultWritePolicy: [string, IOptions];
    private maxCapacity: number;
    private maxCapacityUnit: IOptions;
    private modifyingCapacity: boolean;

    constructor(
        name: string,
        bootPriority: string,
        capacityBytes: string,
        accelerationMethod: string,
        stripeSizeBytes: number,
        capacityUnit: IOptions,
        defaultWritePolicy: string,
        maxCapacity: number,
        maxCapacityUnit: IOptions,
        modifyingCapacity: boolean
    ) {
        this.name = ['COMMON_NAME', name];
        this.bootPriority = ['STORE_BOOT_DISK', VolumeStaticModel.getBootPriority(bootPriority)];
        this.capacityBytes = ['COMMON_CAPACITY', capacityBytes];
        this.accelerationMethod = ['STORE_ACCELERATION', VolumeStaticModel.getSpeedMethod(accelerationMethod)];
        this.stripeSizeBytes = ['STORE_STRIPE_SIZE', VolumeStaticModel.getStripeSizeBytes(stripeSizeBytes)];
        this.capacityUnit = ['', capacityUnit];
        this.defaultWritePolicy = ['STORE_WRITE_POLICY', VolumeStaticModel.getPmcWritePolicy(defaultWritePolicy)];
        this.maxCapacity = maxCapacity;
        this.maxCapacityUnit = maxCapacityUnit;
        this.modifyingCapacity = modifyingCapacity;
    }

    get getName(): string {
        return this.name[1];
    }

    // PMC新增
    get getStripeSizeBytes(): IOptions {
        return this.stripeSizeBytes[1];
    }

    get getSpeedMethod(): IOptions {
        return this.accelerationMethod[1];
    }

    get getCapacity(): string {
        return this.capacityBytes[1];
    }

    get getCapacityUnit(): IOptions {
        return this.capacityUnit[1];
    }

    get getPMCWritePolicy(): IOptions {
        return this.defaultWritePolicy[1];
    }

    get getBootPriority(): IOptions {
        return this.bootPriority[1];
    }
    get getMaxCapacity(): number {
        return this.maxCapacity;
    }
    get getMaxCapacityUnit(): IOptions {
        return this.maxCapacityUnit;
    }
    get getModifyingCapacity(): boolean {
        return this.modifyingCapacity;
    }
}
