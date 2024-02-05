import { IOptions } from 'src/app/common-app/models';
import { IVolumePMCEditData } from '../storage-interface';
import { StorageVaild } from '../../vaild/storage.vaild';
import { UntypedFormControl, Validators } from '@angular/forms';
import { VolumeStaticModel } from '../static-data';
import { TiValidators } from '@cloud/tiny3';


export class VolumePMCEditModel {
    private constructor() { }

    private static instance: VolumePMCEditModel;
    private formContral: UntypedFormControl = new UntypedFormControl('', StorageVaild.assic15());
    private capacityContral: UntypedFormControl = new UntypedFormControl('', [TiValidators.required, Validators.pattern(/^\d+(\.\d+)?$/)]);

    private writePolicyOption = VolumeStaticModel.pmcWritePolicy;
    private selectData: IVolumePMCEditData = {
        name: null,
        optimumIOSizeBytes: null,
        accelerationMethod: null,
        capacityBytes: null,
        capacityUnit: null,
        defaultWritePolicy: null
    };
    private originData: IVolumePMCEditData;
    private saveState: boolean;

    public static getInstance(): VolumePMCEditModel {
        if (!VolumePMCEditModel.instance) {
            VolumePMCEditModel.instance = new VolumePMCEditModel();
        }
        return VolumePMCEditModel.instance;
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

    set setOriginData(selectData: IVolumePMCEditData) {
        this.originData = selectData;
    }

    public static initVolumePMCEdit(
        name: string,
        bootPriority: IOptions,
        optimumIOSizeBytes: IOptions,
        accelerationMethod: IOptions,
        capacityBytes: string,
        capacityUnit: IOptions,
        defaultWritePolicy: IOptions
    ): VolumePMCEditModel {
        VolumePMCEditModel.getInstance().selectData = {
            name,
            bootPriority,
            optimumIOSizeBytes,
            accelerationMethod,
            capacityBytes,
            capacityUnit,
            defaultWritePolicy
        };

        VolumePMCEditModel.getInstance().originData = JSON.parse(JSON.stringify(VolumePMCEditModel.getInstance().getSelectData));
        VolumePMCEditModel.getInstance().checkSave();
        return this.instance;
    }

    public static destroy(): void {
        if (VolumePMCEditModel.instance) {
            VolumePMCEditModel.instance = null;
        }
    }

    public setName(name: string) {
        this.selectData.name = name;
        this.checkSave();
    }

    public setStripeSizeBytes(size: IOptions) {
        this.selectData.optimumIOSizeBytes = size;
        this.checkSave();
    }

    public setSpeedMethod(method: IOptions) {
        this.selectData.accelerationMethod = method;
        this.checkSave();
    }

    public setCapacity(capacity: string) {
        this.selectData.capacityBytes = capacity;
        this.checkSave();
    }

    public setCapacityUnit(unit: IOptions) {
        this.selectData.capacityUnit = unit;
        this.checkSave();
    }

    public setWritePolicy(policy: IOptions) {
        this.selectData.defaultWritePolicy = policy;
        this.checkSave();
    }

    public setBootPriority(bootPriority: IOptions) {
        this.selectData.bootPriority = bootPriority;
        this.checkSave();
    }

    public checkSave() {
        if (!this.formContral || this.formContral.errors ||
            !this.capacityContral || this.capacityContral.errors ||
            JSON.stringify(this.originData) === JSON.stringify(this.selectData)
        ) {
            this.saveState = true;
        } else {
            this.saveState = false;
        }
    }
}
