import { IOptions } from 'src/app/common-app/models';
import { IVolumeEditData, IVolumnDisabled, IStorageRadio } from '../storage-interface';
import { StorageTree } from '../storage-tree.model';
import { StorageVaild } from '../../vaild/storage.vaild';
import { UntypedFormControl } from '@angular/forms';
import { VolumeStaticModel } from '../static-data';

export class VolumeEditModel {
    private constructor() { }

    private static instance: VolumeEditModel;
    private formContral: UntypedFormControl = new UntypedFormControl('', StorageVaild.assic15());

    private writePolicyOption = VolumeStaticModel.writePolicy;
    private selectData: IVolumeEditData = {
        name: '',
        bootEnabled: null,
        sSDCachingEnabled: null,
        driveCachePolicy: null,
        accessPolicy: null,
        bGIEnabled: null,
        defaultCachePolicy: null,
        defaultWritePolicy: null,
        defaultReadPolicy: null,
    };
    private originData: IVolumeEditData;
    private volumnDisable: IVolumnDisabled = {
        twoCache: true,
        readPolicyConfig: true,
        writePolicyConfig: true,
        iOPolicyConfig: true,
        drivePolicyConfig: true,
        accessPolicyConfig: true,
        isSSCD: true,
    };

    private saveState: boolean;

    public static getInstance(): VolumeEditModel {
        if (!VolumeEditModel.instance) {
            VolumeEditModel.instance = new VolumeEditModel();
        }
        return VolumeEditModel.instance;
    }

    get getWritePolicy() {
        return this.writePolicyOption;
    }
    get getOriginData() {
        return this.originData;
    }

    get getSelectData() {
        return this.selectData;
    }

    get getSaveState(): boolean {
        return this.saveState;
    }

    get getVolumnDisable(): IVolumnDisabled {
        return this.volumnDisable;
    }

    get getFormControl() {
        return this.formContral;
    }

    set setOriginData(selectData: IVolumeEditData) {
        this.originData = selectData;
    }

    public static initVolumeEdit(
        name: string,
        bootDiskSelect: IStorageRadio,
        sscdFuncSelect: IStorageRadio,
        driveCacheSelect: IOptions,
        accessSelect: IOptions,
        bgiStateSelect: IOptions,
        ioPolicySelect: IOptions,
        writePolicySelect: IOptions,
        readPolicySelect: IOptions,
        ssdEnable: boolean,
        twoCache: boolean,
        raidIndex: number
    ): VolumeEditModel {
        VolumeEditModel.getInstance().selectData = {
            name,
            bootEnabled: bootDiskSelect,
            sSDCachingEnabled: sscdFuncSelect,
            driveCachePolicy: driveCacheSelect,
            accessPolicy: accessSelect,
            bGIEnabled: bgiStateSelect,
            defaultCachePolicy: ioPolicySelect,
            defaultWritePolicy: writePolicySelect,
            defaultReadPolicy: readPolicySelect,
        };
        const raidChildData = StorageTree.getInstance().getParentRaid(raidIndex).getRaid.getChidrenData;
        VolumeEditModel.getInstance().volumnDisable = {
            twoCache,
            readPolicyConfig: twoCache ? twoCache : !raidChildData.readPolicyConfig,
            writePolicyConfig: !raidChildData.writePolicyConfig,
            iOPolicyConfig: twoCache ? twoCache : !raidChildData.IOPolicyConfig,
            drivePolicyConfig: twoCache ? twoCache : !raidChildData.drivePolicyConfig,
            accessPolicyConfig: twoCache ? twoCache : !raidChildData.accessPolicyConfig,
            isSSCD: !!sscdFuncSelect,
        };
        VolumeEditModel.getInstance().originData = JSON.parse(JSON.stringify(VolumeEditModel.getInstance().getSelectData));
        VolumeEditModel.getInstance().checkSave();
        if (twoCache) {
            VolumeEditModel.getInstance().writePolicyOption = [
                VolumeStaticModel.writePolicy[0],
                VolumeStaticModel.writePolicy[1]
            ];
        }
        return this.instance;
    }

    public static destroy(): void {
        if (VolumeEditModel.instance) {
            VolumeEditModel.instance = null;
        }
    }

    public setName(name: string) {
        this.selectData.name = name;
        this.checkSave();
    }

    public setBootDisk(bootDisk: IStorageRadio) {
        this.selectData.bootEnabled = bootDisk;
        this.checkSave();
    }

    public setSSCDFunc(sscdFunc: IStorageRadio) {
        this.selectData.sSDCachingEnabled = sscdFunc;
        this.checkSave();
    }

    public setDriveCache(driveCache: IOptions) {
        this.selectData.driveCachePolicy = driveCache;
        this.checkSave();
    }

    public setAccess(access: IOptions) {
        this.selectData.accessPolicy = access;
        this.checkSave();
    }

    public setBgiState(bgiState: IOptions) {
        this.selectData.bGIEnabled = bgiState;
        this.checkSave();
    }

    public setIOPolicy(ioPolicy: IOptions) {
        this.selectData.defaultCachePolicy = ioPolicy;
        this.checkSave();
    }

    public setWritePolicy(writePolicy: IOptions) {
        this.selectData.defaultWritePolicy = writePolicy;
        this.checkSave();
    }

    public setReadPolicy(readPolicy: IOptions) {
        this.selectData.defaultReadPolicy = readPolicy;
        this.checkSave();
    }

    public checkSave() {
        if (!this.formContral ||
            !this.formContral.valid ||
            JSON.stringify(this.originData) === JSON.stringify(this.selectData)
        ) {
            this.saveState = true;
        } else {
            this.saveState = false;
        }
    }
}
