import { UntypedFormGroup } from '@angular/forms';
import { IOptions } from 'src/app/common-app/models';
import { IRaidEditData, Store } from '../storage-interface';

export class RaidEditModel {
  private constructor() {}
  private static instance: RaidEditModel;
  public periodAndTaskWaitTimeform: UntypedFormGroup;

  set setSelectData(selectData: IRaidEditData) {
    this.selectData = selectData;
  }

  set setOriginData(originData: IRaidEditData) {
    this.originData = originData;
  }

  get getSelectData() {
    return this.selectData;
  }

  get getOriginData() {
    return this.originData;
  }

  private saveState: boolean = true;
  private selectData: IRaidEditData;
  private originData: IRaidEditData;

  // smart错误状态
  private smartState: boolean;
  private jbodShow: boolean;
  private jbodStateSupported: boolean;

  public static getInstance(): RaidEditModel {
    if (!RaidEditModel.instance) {
      RaidEditModel.instance = new RaidEditModel();
    }
    return RaidEditModel.instance;
  }

  public static initRaidEdit(
    jBODState: boolean,
    copyBackState?: boolean,
    smarterCopyBackState?: boolean,
    workMode?: IOptions,
    jbodStateSupported?: boolean,
    noBBUWriteState?: boolean,
    readCachePercent?: number,
    bootDevice1?: IOptions,
    bootDevice2?: IOptions,
    writeCacheType?: IOptions,
    writeCachePolicy?: IOptions,
    enabled?: boolean,
    runningStatus?: boolean,
    autoRepairEnabled?: boolean,
    rate?: IOptions,
    period?: number,
    delayForStart?: number
  ): RaidEditModel {
    RaidEditModel.getInstance().setSelectData = {
      jBODState,
      copyBackState,
      smarterCopyBackState,
      mode: workMode,
      noBatteryWriteCacheEnabled: noBBUWriteState,
      readCachePercent,
      bootDevice1,
      bootDevice2,
      writeCacheType,
      writeCachePolicy,
      enabled,
      runningStatus,
      autoRepairEnabled,
      rate,
      period,
      delayForStart
    };

    RaidEditModel.getInstance().setOriginData = JSON.parse(
      JSON.stringify(RaidEditModel.getInstance().getSelectData)
    );
    RaidEditModel.getInstance().jbodStateSupported = jbodStateSupported;
    RaidEditModel.getInstance().judgJBODShow();
    setTimeout(() => {
        RaidEditModel.getInstance().checkSave(true);
    }, 0);
    return this.instance;
  }

  public static destroy(): void {
    if (RaidEditModel.instance) {
      RaidEditModel.instance = null;
    }
  }

  public static setPeriodAndTaskWaitTimeform(formGroup: UntypedFormGroup) {
    RaidEditModel.getInstance().periodAndTaskWaitTimeform = formGroup;
  }

  public setJBODState(jBODState: boolean) {
    this.selectData.jBODState = jBODState;
    this.checkSave();
  }

  public setConsisEnabled(val: boolean) {
      this.selectData.enabled = val;
      if (val && this.originData.enabled === false) {
        this.periodAndTaskWaitTimeform.root.get('waitTimeControl').enable();
      }
      // 关闭的时候初始化
      if (!val) {
          this.selectData.autoRepairEnabled = this.originData.autoRepairEnabled;
          this.selectData.rate = this.originData.rate;
          this.selectData.period = this.originData.period;
          this.selectData.delayForStart = this.originData.delayForStart;
      }
      this.checkSave();
  }

  public setRunningStatus(runningState: boolean) {
    this.selectData.runningStatus = runningState;
    this.checkSave();
  }

  public setAutoRepairEnabled(autoRepairEnabled: boolean) {
    this.selectData.autoRepairEnabled = autoRepairEnabled;
    this.checkSave();
  }

  public setRate(rate: IOptions) {
    this.selectData.rate = rate;
    this.checkSave();
  }

  public setPeriodValue(period: number) {
    this.selectData.period = period;
    this.checkSave();
  }

  public setTaskWaitTimeValue(delayForStart: number) {
    this.selectData.delayForStart = delayForStart;
    this.checkSave();
  }

  public setNoBBUWriteState(noBBUWriteState: boolean) {
    this.selectData.noBatteryWriteCacheEnabled = noBBUWriteState;
    this.checkSave();
  }

  public setReadCachePercent(readCachePercent: number) {
    if (readCachePercent >= 0 &&
      readCachePercent <= 100 &&
      readCachePercent % 5 === 0
    ) {
      this.selectData.readCachePercent = readCachePercent;
      this.checkSave();
    } else {
      this.checkSave(true);
    }
  }

  public setCopyBackState(copyBackState: boolean) {
    this.selectData.copyBackState = copyBackState;
    this.smartState = !copyBackState;
    this.checkSave();
  }

  public setSmarter(smarterCopyBackState: boolean) {
    this.selectData.smarterCopyBackState = smarterCopyBackState;
    this.checkSave();
  }

  public setWorkModel(workMode: IOptions): void {
    this.selectData.mode = workMode;
    if (this.selectData.mode.id === Store.JBOD) {
      this.setJBODState(this.originData.jBODState);
    }
    this.judgJBODShow();
    this.checkSave();
  }

  public setBootDevice(bootDevice: IOptions, bootNum: number): void {
    if (bootNum === 1) {
      this.selectData.bootDevice1 = bootDevice;
    } else {
      this.selectData.bootDevice2 = bootDevice;
    }
    this.checkSave();
  }

  public setWriteCacheType(writeCacheType: IOptions) {
    this.selectData.writeCacheType = writeCacheType;
    this.checkSave();
  }

  public setWriteCachePolicy(writeCachePolicy: IOptions) {
    this.selectData.writeCachePolicy = writeCachePolicy;
    this.checkSave();
  }

  get getSaveState(): boolean {
    return this.saveState;
  }

  get getSmartState(): boolean {
    return this.smartState;
  }

  public checkSave(uncheck: boolean = false) {
    if (uncheck) {
      this.saveState = uncheck;
      return;
    }
    // 当校验通过，并且任务等待时间编辑框可输入情况下
    if (this.periodAndTaskWaitTimeform && !this.periodAndTaskWaitTimeform.valid && !this.periodAndTaskWaitTimeform.disabled) {
      this.saveState = true;
      return;
    }

    if (this.selectData.enabled !== this.originData.enabled) {
      this.saveState = false;
      return;
    }

    if (JSON.stringify(this.originData) === JSON.stringify(this.selectData)) {
      this.saveState = true;
    } else {
      this.saveState = false;
    }
  }

  private judgJBODShow(): void {
    this.jbodShow = this.jbodStateSupported;
  }

  get getJBODShow() {
    return this.jbodShow;
  }
}
