import { IOptions, IValue } from 'src/app/common-app/models';
import { VolumeStaticModel } from '../static-data';
import { IStorageRadio, RaidType } from '../storage-interface';

export class Volume {

    private name: [string, string];
    private raidLevel: [string, string];
    private ioSize: [string, string];
    private readPolicy: [string, IOptions];
    private writePolicy: [string, IOptions];
    private cachePolicy: [string, IOptions];
    private bgiEnable: [string, IOptions];
    private initType: [string, string];
    private cacheEnable: [string, IValue];
    private consistency: [string, string];
    private status: [string, string];
    private capacityBytes: [string, string];
    private sscd: [string, IStorageRadio];
    private currentRead: [string, IOptions];
    private currentWrite: [string, IOptions];
    private currentCache: [string, IOptions];
    private accessPolicy: [string, IOptions];
    private driveCache: [string, IOptions];
    private bootEnable: [string, IStorageRadio];
    private osDriveName: [string, string];
    private fgiProgress: [string, string];
    private twoCache: boolean;
    private associatedVolumes: [string, string];
    private cacheLineSize: [string, string];
    private accelerationMethod: [string, string];
    private construction: [string, string];


    constructor(
        name: string,
        raidLevel: string,
        ioSize: string,
        readPolicy: string,
        writePolicy: string,
        cachePolicy: string,
        bgiEnable: boolean,
        initType: string,
        cacheEnable: IValue,
        consistency: string,
        status: string,
        capacityBytes: string,
        sscd: boolean,
        currentRead: string,
        currentWrite: string,
        currentCache: string,
        accessPolicy: string,
        driveCache: string,
        bootEnable: boolean,
        osDriveName: string,
        fgiProgress: string,
        twoCache: boolean,
        raidType: string,
        associatedVolumes: string,
        cacheLineSize: string,
        accelerationMethod: string,
        construction: string
    ) {
        this.name = ['COMMON_NAME', name];
        this.raidLevel = ['STORE_RAI_LEVEL', raidLevel];
        this.ioSize = ['STORE_STRIPE_SIZE', ioSize];
        this.status = ['STATUS', status];
        this.capacityBytes = ['COMMON_CAPACITY', capacityBytes];
        this.bootEnable = ['STORE_BOOT_DISK', VolumeStaticModel.getIsBootDisk(bootEnable)];
        this.osDriveName = ['SYSTEM_DRIVE', osDriveName];
        this.fgiProgress = ['SYSTEM_FGIPROGRESS', fgiProgress];
        this.twoCache = twoCache;

        this.currentWrite = ['STORE_CURRENT_WRITE_POLICY', VolumeStaticModel.getWritePolicy(currentWrite)];
        this.writePolicy = ['STORE_DEFAULT_WRITE', VolumeStaticModel.getWritePolicy(writePolicy)];

        // 只有BRCM卡才显示属性
        if (raidType === RaidType.BRCM) {
            this.cacheEnable = ['STORE_LEVEL2_CACHE', cacheEnable];
            this.sscd = ['STORE_SSCD_FEATURES', VolumeStaticModel.getSSCDFunction(sscd)];
            this.initType = ['STORE_INITITYPE', this.getInitMode(initType)];
            this.consistency = ['STORE_CONSISTENCY_CHECK', consistency];
            this.readPolicy = ['STORE_DEFAULT_READ', VolumeStaticModel.getReadPolicy(readPolicy)];
            this.cachePolicy = ['STORE_DEFAULT_IO', VolumeStaticModel.getIOPolicy(cachePolicy)];
            this.bgiEnable = ['STORE_INITIALIZATION', VolumeStaticModel.getBgiState(bgiEnable)];
            this.currentRead = ['STORE_CURRENT_READ_POLICY', VolumeStaticModel.getReadPolicy(currentRead)];
            this.currentCache = ['STORE_CURRENT_IQ_POLICY', VolumeStaticModel.getIOPolicy(currentCache)];
            this.accessPolicy = ['STORE_ACCESS_POLICY', VolumeStaticModel.getAccessPolicy(accessPolicy)];
            this.driveCache = ['STORE_CACHE_STATUS', VolumeStaticModel.getDriverCachePolicy(driveCache)];
        } else if (raidType === RaidType.ARIES) {
            this.readPolicy = ['STORE_DEFAULT_READ', VolumeStaticModel.getReadPolicy(readPolicy)];
            this.cachePolicy = ['STORE_DEFAULT_IO', VolumeStaticModel.getIOPolicy(cachePolicy)];
            this.bgiEnable = ['STORE_INITIALIZATION', VolumeStaticModel.getBgiState(bgiEnable)];
            this.currentRead = ['STORE_CURRENT_READ_POLICY', VolumeStaticModel.getReadPolicy(currentRead)];
            this.currentCache = ['STORE_CURRENT_IQ_POLICY', VolumeStaticModel.getIOPolicy(currentCache)];
            this.accessPolicy = ['STORE_ACCESS_POLICY', VolumeStaticModel.getAccessPolicy(accessPolicy)];
            this.driveCache = ['STORE_CACHE_STATUS', VolumeStaticModel.getDriverCachePolicy(driveCache)];
        } else {
            this.cacheEnable = ['STORE_LEVEL2_CACHE', cacheEnable];
            this.sscd = ['STORE_SSCD_FEATURES', VolumeStaticModel.getSSCDFunction(sscd)];
            this.currentWrite = ['STORE_CURRENT_WRITE_POLICY', twoCache ? VolumeStaticModel.getWritePolicy(currentWrite) : null];
            this.associatedVolumes = ['STORE_ASSOCIT_LOGIC', associatedVolumes];
            this.cacheLineSize = ['STORE_CACHE_LINE', cacheLineSize];
            this.accelerationMethod = ['STORE_ACCELERATION', accelerationMethod];
            this.construction = ['FDM_CONSTRUCTION_STATE', construction];
        }
    }

    private getInitMode(key: string): string {
        let str = '--';
        switch (key) {
            case 'UnInit':
                str = 'No Init';
                break;
            case 'QuickInit':
                str = 'Quick Init';
                break;
            case 'FullInit':
                str = 'Full Init';
                break;
            default:
                str = '--';
        }
        return str;
    }

    get getName(): string {
        return this.name[1];
    }

    get getBootDisk(): IStorageRadio {
        return this.bootEnable[1];
    }

    get getSSCDFunc(): IStorageRadio {
        return this.sscd[1];
    }

    get getDriveCache(): IOptions {
        return this.driveCache[1];
    }

    get getAccessPolicy(): IOptions {
        return this.accessPolicy[1];
    }

    get getBGIState(): IOptions {
        return this.bgiEnable[1];
    }

    get getIOPolicy(): IOptions {
        return this.cachePolicy[1];
    }

    get getWritePolicy(): IOptions {
        return this.writePolicy[1];
    }

    get getReadPolicy(): IOptions {
        return this.readPolicy[1];
    }

    get getCacheEnableState(): boolean {
        return Boolean(this.cacheEnable[1].value);
    }

    get getTwoCache(): boolean {
        return this.twoCache;
    }
}
