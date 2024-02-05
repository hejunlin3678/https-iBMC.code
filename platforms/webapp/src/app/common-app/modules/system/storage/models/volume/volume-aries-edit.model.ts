import { IOptions } from 'src/app/common-app/models';
import { IVolumeAriesEditData, IVolumePMCEditData } from '../storage-interface';
import { StorageVaild } from '../../vaild/storage.vaild';
import { UntypedFormControl, Validators } from '@angular/forms';
import { VolumeStaticModel } from '../static-data';
import { TiValidators } from '@cloud/tiny3';


export class VolumeAriesEditModel {
    private constructor() { }

    private static instance: VolumeAriesEditModel;
    private formContral: UntypedFormControl = new UntypedFormControl('', StorageVaild.assic15());
    private capacityContral: UntypedFormControl = new UntypedFormControl('', [TiValidators.required, Validators.pattern(/^\d+(\.\d+)?$/)]);

    private writePolicyOption = VolumeStaticModel.writePolicy;
    private selectData: IVolumeAriesEditData = {
        name: null,
        bootEnabled: null,
        defaultWritePolicy: null,
        defaultReadPolicy: null
    };
    private originData: IVolumeAriesEditData;
    private saveState: boolean;
    // hi1880卡模式为HBA时，逻辑盘设置项“默认读策略”和“默认读策略”置灰
    public hbaMode: boolean = false;

    public static getInstance(): VolumeAriesEditModel {
        if (!VolumeAriesEditModel.instance) {
            VolumeAriesEditModel.instance = new VolumeAriesEditModel();
        }
        return VolumeAriesEditModel.instance;
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

    get getFormControl() {
        return this.formContral;
    }

    get getCapacityFormControl() {
        return this.capacityContral;
    }

    set setOriginData(selectData: IVolumeAriesEditData) {
        this.originData = selectData;
    }

    public static initVolumeAriesEdit(
        name: string,
        bootEnabled: IOptions,
        defaultWritePolicy: IOptions,
        defaultReadPolicy: IOptions,
        hbaMode: boolean
    ): VolumeAriesEditModel {
        VolumeAriesEditModel.getInstance().selectData = {
            name,
            bootEnabled,
            defaultWritePolicy,
            defaultReadPolicy
        };
        VolumeAriesEditModel.getInstance().hbaMode = hbaMode;
        VolumeAriesEditModel.getInstance().originData = JSON.parse(JSON.stringify(VolumeAriesEditModel.getInstance().getSelectData));
        VolumeAriesEditModel.getInstance().checkSave();
        return this.instance;
    }

    public static destroy(): void {
        if (VolumeAriesEditModel.instance) {
            VolumeAriesEditModel.instance = null;
        }
    }

    public setName(name: string) {
        this.selectData.name = name;
        this.checkSave();
    }

    public setWritePolicy(policy: IOptions) {
        this.selectData.defaultWritePolicy = policy;
        this.checkSave();
    }

    public setReadPolicy(policy: IOptions) {
        this.selectData.defaultReadPolicy = policy;
        this.checkSave();
    }

    public setBootDisk(bootEnabled: IOptions) {
        this.selectData.bootEnabled = bootEnabled;
        this.checkSave();
    }

    public checkSave() {
        if (JSON.stringify(this.originData) === JSON.stringify(this.selectData)) {
            this.saveState = true;
        } else {
            this.saveState = false;
        }
    }
}
