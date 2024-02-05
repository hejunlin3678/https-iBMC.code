import { IRadio, IOptions } from 'src/app/common-app/models';
import { IDriverEditData, IStorageRadio, RaidType } from '../storage-interface';
import { DriveStaticModel } from '../static-data';
import { Driver } from './driver.model';
import { Raid } from '../raid';

export class DriverEditModel {
  private constructor() {}

  set setSelectData(selectData: IDriverEditData) {
    this.selectData = selectData;
  }

  set setOriginData(originData: IDriverEditData) {
    this.originData = originData;
  }

  get getSelectData() {
    return this.selectData;
  }

  get getOriginData() {
    return this.originData;
  }

  get getIndicator() {
    return this.selectData.indicatorLED;
  }

  get getHotspareType() {
    return this.selectData.hotspareType;
  }

  get getHotspareBtn() {
    return this.hotspareType;
  }

  get getFirmwareStatusBtn() {
    return this.firmwareStatus;
  }

  get getFirmwareStatus() {
    return this.selectData.firmwareStatus;
  }

  get getLogicalDrive() {
    return this.selectData.spareforLogicalDrive;
  }

  get getLogicalDriveBtn() {
    return this.logicalDrive;
  }

  get getSaveState() {
    return this.saveState;
  }

  get getVolumeSupport() {
    return this.volumeSupport;
  }

  private static instance: DriverEditModel;

  private volumeSupport: boolean;
  private selectData: IDriverEditData;
  private originData: IDriverEditData;
  private saveState: boolean;
  private epdSupported: boolean;
  public indicator: boolean;
  public jbodState: boolean;

  private hotspareType: IRadio[];
  public raidType: string;
  private firmwareStatus: {
    options: IOptions[];
    disable: boolean;
  };
  private logicalDrive: {
    options: IOptions[];
    disable: boolean;
    show: boolean;
  };

  public static getInstance(): DriverEditModel {
    if (!DriverEditModel.instance) {
      DriverEditModel.instance = new DriverEditModel();
    }
    return DriverEditModel.instance;
  }

  public static initDriveEdit(
    indicator: boolean,
    driver: Driver,
    logicalSelect: IOptions[] | IOptions,
    logicalOptions: IOptions[],
    raid: Raid,
    jbod: boolean,
    bootDiskSelect: IStorageRadio,
    raidType?: string,
    bootPriority?: IOptions,
    patrolState?: IOptions
  ): DriverEditModel {
    const volumeSupport: boolean = !raid.getVolumeSupport;
    const logicalDriveDisable: boolean = logicalSelect &&
      (raidType === RaidType.ARIES &&  !(logicalSelect instanceof Array) && logicalSelect?.id !== 'noData') ||
      (logicalSelect instanceof Array && logicalSelect.length > 0 && logicalSelect[0]?.id !== 'noData');
    DriverEditModel.getInstance().indicator = indicator;
    DriverEditModel.getInstance().epdSupported = raid.getEpdSupported;
    DriverEditModel.getInstance().setSelectData = {
      indicatorLED: indicator
        ? DriveStaticModel.INDICATOR.indicatorOn
        : DriveStaticModel.INDICATOR.indicatorOff,
      hotspareType: driver.getHotspareType,
      firmwareStatus: driver.getFirmwareStatus,
      spareforLogicalDrive: logicalSelect,
      bootEnabled: bootDiskSelect,
      bootPriority,
      patrolState
    };
    DriverEditModel.getInstance().logicalDrive = {
      options: logicalOptions,
      show: false,
      disable: logicalDriveDisable
    };
    DriverEditModel.getInstance().raidType = raidType;
    if (raidType === RaidType.PMC || raidType === RaidType.HBA) {
      // PMC卡隐藏“全局”、追加“自动替换”
      DriverEditModel.getInstance().hotspareType = [
        DriveStaticModel.HOTSPARERADIOS.none,
        DriveStaticModel.HOTSPARERADIOS.dedicated,
        DriveStaticModel.HOTSPARERADIOS.autoReplace,
      ];
    } else {
      DriverEditModel.getInstance().hotspareType = [
        DriveStaticModel.HOTSPARERADIOS.none,
        DriveStaticModel.HOTSPARERADIOS.global,
        DriveStaticModel.HOTSPARERADIOS.dedicated,
      ];
    }
    if (raidType !== RaidType.ARIES) {
      DriverEditModel.getInstance().firmwareStatus = {
        options: DriveStaticModel.getInstance().optionsFactory(
          String(driver.getFirmwareStatus.id),
          jbod,
          raid.getMode,
          driver.getIsEPD,
          raid.getEpdSupported
        ),
        disable: DriverEditModel.getInstance().isFirmwareDisable(
          String(driver.getFirmwareStatus.id)
        ),
      };
    } else {
        DriverEditModel.getInstance().jbodState = jbod;
        DriverEditModel.getInstance().firmwareStatus = {
          options: [DriveStaticModel.FIRMWARESTATUS.unconfiguredGood, DriveStaticModel.FIRMWARESTATUS.online],
          disable: (!jbod) || driver.getFirmwareStatus.id === 'HotSpareDrive'
        };
    }
    DriverEditModel.getInstance().originData = JSON.parse(
      JSON.stringify(DriverEditModel.getInstance().getSelectData)
    );

    DriverEditModel.getInstance().setHotspareType(driver.getHotspareType);
    // hi1880卡热备状态不为“无”时，无法设置巡检开关
    if (driver.getHotspareType.disable && raidType === RaidType.ARIES) {
      DriveStaticModel.patrolSwitchStatus.forEach(item => item.disable = true);
    } else {
      DriveStaticModel.patrolSwitchStatus.forEach(item => item.disable = false);
    }

    DriverEditModel.getInstance().setvolumeSupport(volumeSupport);
    return this.instance;
  }

  public static destroy(): void {
    if (DriverEditModel.instance) {
      DriverEditModel.instance = null;
    }
  }

  public setFirmwareSelect(firmwareSelect: IOptions) {
    this.selectData.firmwareStatus = firmwareSelect;
    this.checkSave();
  }

  public setBootPriority(bootPriority: IOptions) {
    this.selectData.bootPriority = bootPriority;
    this.checkSave();
  }

  public setLogicalDrive(logicalDrive: IOptions[] | IOptions) {
    this.selectData.spareforLogicalDrive = logicalDrive;
    if (this.selectData.hotspareType.id === 'AutoReplace' &&  logicalDrive instanceof Array && logicalDrive.length === 1) {
      DriverEditModel.getInstance().logicalDrive.options.filter((item) => {
        if (item.label !== logicalDrive[0].label) {
          item.disabled = true;
        }
      });
    } else {
      DriverEditModel.getInstance().logicalDrive.options.filter(
        (item) => (item.disabled = false)
      );
    }
    this.checkSave();
  }

  public setBootDisk(bootDisk: IStorageRadio) {
    this.selectData.bootEnabled = bootDisk;
    this.checkSave();
  }

  public setPatrolState(patrolState: IStorageRadio) {
    this.selectData.patrolState = patrolState;
    this.checkSave();
  }

  /**
   * 设置热备状态按钮的可编辑状态
   */
  public setHotSpareDisable(hotSpareDisable: {
    none: boolean;
    dedicated: boolean;
    global?: boolean;
    autoReplace?: boolean;
  }) {
    const hotspareType = this.hotspareType;
    hotspareType[0].disable = hotSpareDisable.none;
    if (this.raidType === RaidType.PMC || this.raidType === RaidType.HBA) {
      hotspareType[1].disable =
        this.originData.firmwareStatus.id === 'Ready'
          ? hotSpareDisable.dedicated
          : true;
      hotspareType[2].disable =
        this.originData.firmwareStatus.id === 'Ready'
          ? hotSpareDisable.autoReplace
          : true;
    } else {
      hotspareType[2].disable =
        this.originData.firmwareStatus.id === 'UnconfiguredGood'
          ? hotSpareDisable.dedicated
          : true;
      hotspareType[1].disable =
        this.originData.firmwareStatus.id === 'UnconfiguredGood'
          ? hotSpareDisable.global
          : true;
    }
    this.hotspareType = hotspareType;
  }

  /**
   * 更改热备状态
   */
  public setHotspareType(hotspareSelect: IRadio) {
    // 更改热备状态当前选中项
    const selectData = this.selectData;
    selectData.hotspareType = hotspareSelect;
    // 根据热备状态选中项更改固件状态下拉选项
    if (this.selectData.hotspareType?.id !== 'None') {
      selectData.firmwareStatus = DriveStaticModel.FIRMWARESTATUS.hotSpareDrive;
    } else {
      selectData.firmwareStatus = this.originData.firmwareStatus;
    }
    this.selectData = selectData;

    // 根据固件状态选项判定固件状态控件可编辑状态
    this.firmwareStatus = {
      options: this.firmwareStatus.options,
      disable: this.raidType === RaidType.ARIES ?
                (!this.jbodState || this.getSelectData.firmwareStatus.id === 'HotSpareDrive') :
                this.isFirmwareDisable(String(this.selectData.firmwareStatus.id))
    };

    // 根据热备状态选项判定热备状态按钮可编辑状态
    this.setHotSpareDisable({
      none: false,
      global: false,
      dedicated: false,
      autoReplace: false,
    });

    // 根据热备状态选项判定逻辑盘控件的显隐
    const logicalDrive = this.logicalDrive;
    if (
      hotspareSelect?.id === 'Dedicated' ||
      hotspareSelect?.id === 'AutoReplace'
    ) {
      logicalDrive.show = true;
      if (this.hotspareType?.every((item) => !item.disable)) {
        this.selectData.spareforLogicalDrive = [];
        DriverEditModel.getInstance().logicalDrive.options.filter(
          (item) => (item.disabled = false)
        );
      }
    } else {
      logicalDrive.show = false;
      this.selectData.spareforLogicalDrive = [];
    }
    this.logicalDrive = logicalDrive;

    this.checkSave();
  }

  public setIndicator(indicator: boolean) {
    this.selectData.indicatorLED = indicator
      ? DriveStaticModel.INDICATOR.indicatorOn
      : DriveStaticModel.INDICATOR.indicatorOff;
    this.checkSave();
  }

  /**
   * 设置是否为IT卡属性
   */
  public setvolumeSupport(volumeSupport: boolean) {
    this.volumeSupport = volumeSupport;
    this.volumeSupportFunc(this.volumeSupport);
  }

  /**
   * 控制器为IT卡时（false），控制器、逻辑盘均不可编辑，只可编辑物理盘的“定位状态”
   * @param volumeSupport 是否为IT卡
   */
  private volumeSupportFunc(volumeSupport: boolean) {
    if (volumeSupport) {
      this.firmwareStatus.disable = true;
      this.logicalDrive.disable = true;
      this.setHotSpareDisable({ none: true, global: true, dedicated: true });
    }
  }

  /**
   * 判定固件状态组件是否禁用
   * 判定依据（以下条件必须同时满足）：
   *  1、固件状态为可编辑的选项
   *  2、控制器非IT卡
   * @param key 固件状态选项
   */
  private isFirmwareDisable(key: string): boolean {
    // 允许编辑固件状态控件的选项,pmc卡为“Ready”状态也可编辑
    const firmwareTrue = [
      'UnconfiguredGood',
      'UnconfiguredBad',
      'JBOD',
      'Online',
      'Offline',
      'Ready',
    ];
    if (firmwareTrue.indexOf(key) > -1 && !this.volumeSupport) {
      return false;
    }
    return true;
  }

  public checkSave() {
    // 当前值和初始值一致时，保存按钮置灰
    if (JSON.stringify(this.originData) === JSON.stringify(this.selectData)) {
      this.saveState = true;
      return;
    }
    // 可关联逻辑盘为数组时（pmc卡和博通卡）
    if (this.selectData.spareforLogicalDrive instanceof Array) {
      // 热备状态为专用或者自动替换时，管理逻辑盘没选或者选择了没有数据，保存按钮置灰
      if (['Dedicated', 'AutoReplace'].includes(this.selectData.hotspareType.id.toString()) &&
          (this.selectData.spareforLogicalDrive.length === 0 || this.selectData.spareforLogicalDrive[0]?.id === 'noData')) {
        this.saveState = true;
        return;
      }
    } else {
      // 可关联逻辑盘只能选一个时（hi1880），没有选择逻辑盘或者选择了没有数据，保存按钮置灰
      if (!this.selectData.spareforLogicalDrive || this.selectData.spareforLogicalDrive['id'] === 'noData') {
        this.saveState = true;
        return;
      }
    }
    this.saveState = false;
  }
}
