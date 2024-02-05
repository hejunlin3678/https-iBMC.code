import { IOptions } from 'src/app/common-app/models';
import { IVolumeSPEditData, IVolumePMCEditData } from '../storage-interface';
import { StorageVaild } from '../../vaild/storage.vaild';
import { UntypedFormControl, Validators } from '@angular/forms';
import { VolumeStaticModel } from '../static-data';
import { TiValidators } from '@cloud/tiny3';

export class VolumeSPEditModel {
  private constructor() {}

  private static instance: VolumeSPEditModel;
  private formContral: UntypedFormControl = new UntypedFormControl(
    '',
    StorageVaild.assic15()
  );
  private capacityContral: UntypedFormControl = new UntypedFormControl('', [
    TiValidators.required,
    Validators.pattern(/^\d+(\.\d+)?$/),
  ]);

  private writePolicyOption = VolumeStaticModel.writePolicy;
  private selectData: IVolumeSPEditData = {
    name: null,
    bootEnabled: null,
    defaultWritePolicy: null,
    defaultReadPolicy: null,
    driveCachePolicy: null,
  };

  private originData: IVolumeSPEditData;
  private saveState: boolean;
  // hi1880卡模式为HBA时，逻辑盘设置项“默认读策略”和“默认读策略”置灰
  public hbaMode: boolean = false;

  public static getInstance(): VolumeSPEditModel {
    if (!VolumeSPEditModel.instance) {
      VolumeSPEditModel.instance = new VolumeSPEditModel();
    }
    return VolumeSPEditModel.instance;
  }

  get getWritePolicy(): any {
    return this.writePolicyOption;
  }

  get getOriginData(): any {
    return this.originData;
  }

  get getSelectData(): IVolumeSPEditData {
    return this.selectData;
  }

  get getSaveState(): boolean {
    return this.saveState;
  }

  get getFormControl(): UntypedFormControl {
    return this.formContral;
  }

  get getCapacityFormControl(): UntypedFormControl {
    return this.capacityContral;
  }

  set setOriginData(selectData: IVolumeSPEditData) {
    this.originData = selectData;
  }

  public static initVolumeSPEdit(
    name: string,
    bootEnabled: IOptions,
    defaultWritePolicy: IOptions,
    defaultReadPolicy: IOptions,
    hbaMode: boolean,
    driveCachePolicy: IOptions
  ): VolumeSPEditModel {
    VolumeSPEditModel.getInstance().selectData = {
      name,
      bootEnabled,
      defaultWritePolicy,
      defaultReadPolicy,
      driveCachePolicy,
    };
    VolumeSPEditModel.getInstance().hbaMode = hbaMode;
    VolumeSPEditModel.getInstance().originData = JSON.parse(
      JSON.stringify(VolumeSPEditModel.getInstance().getSelectData)
    );
    VolumeSPEditModel.getInstance().checkSave();
    return this.instance;
  }

  public static destroy(): void {
    if (VolumeSPEditModel.instance) {
      VolumeSPEditModel.instance = null;
    }
  }

  public setName(name: string): void {
    this.selectData.name = name;
    this.checkSave();
  }

  public setWritePolicy(policy: IOptions): void {
    this.selectData.defaultWritePolicy = policy;
    this.checkSave();
  }

  public setReadPolicy(policy: IOptions): void {
    this.selectData.defaultReadPolicy = policy;
    this.checkSave();
  }

  public setBootDisk(bootEnabled: IOptions): void {
    this.selectData.bootEnabled = bootEnabled;
    this.checkSave();
  }

  public setDriveCache(driveCache: IOptions): void {
    this.selectData.driveCachePolicy = driveCache;
    this.checkSave();
  }

  public checkSave(): void {
    if (JSON.stringify(this.originData) === JSON.stringify(this.selectData)) {
      this.saveState = true;
    } else {
      this.saveState = false;
    }
  }
}
