import { BBUModel } from './bbu.model';
import { IChildData, RaidType, ISelect, Store } from '../storage-interface';
import { DriveCheckBox } from '../volume-create';
import { StorageStaticData } from '../static-data';
import { TranslateService } from '@ngx-translate/core';
import { IValue, IOptions, ISwitch, IWriteCachePolicy } from 'src/app/common-app/models';

export class Raid {
    private name: [string, string];
    private type: [string, string];
    private firmwareVersion: [string, string];
    private bandManag: [string, string];
    private health: [string, string];
    private raidLevels: [string, IValue];
    private mode: [string, IOptions];
    private configVersion: [string, string];
    private memorySize: [string, string];
    private deviceInter: [string, string];
    private sasAddress: [string, string];
    private stripSizeRange: [string, string];
    private cachePinned: [string, string];
    private faultMemory: [string, string];
    private driverName: [string, string];
    private driverVersion: [string, string];
    private copyback: [string, ISwitch];
    private smartErrors: [string, ISwitch];
    private jbodModel: [string, ISwitch];
    private driveArr: DriveCheckBox[];
    private bbu: BBUModel;
    private foreignConfigStatus: boolean;
    private supportedModes: ISelect;
  private epdSupported: boolean;
  private jbodStateSupported: boolean;
  private ibmaRunning: boolean;
  private tmpJbodModel: ISwitch;
  private autoRepairEnabled: [string, string];
  private completedVolumeCount: [string, number];
  private delayForStart: number;
  private enabled: boolean;
  private period: [string, number];
  private rate: [string, IOptions];
  private runningStatus: [string, string];
  private totalVolumeCount: [string, number];
  private consisEnabled: [string, string];

  private consisCheckInfo: {
      runningStatus: boolean;
      period: number;
      rate: IOptions;
      autoRepairEnabled: boolean;
      completedVolumeCount: number;
      totalVolumeCount: number;
  };

  private i18n: TranslateService;
  // PMC卡属性
  private noBBUWrite: [string, string];
  private pcieLinkWidth: [string, number | string];
  private readCache: [string, string];
  private spareActivationMode: [string, string];
  private writeCachePolivy: [string, string];
  private bootDevices: [string, string];
  private writeCacheData: IWriteCachePolicy | null;

    // RAID卡下面物理盘或逻辑盘所需要的逻辑处理数据
    private childData: IChildData;

  // 卡的类型
  private raidCardType: string;

    constructor(
        name: string,
        type: string,
        firmwareVersion: string,
        bandManag: string,
        health: string,
        raidLevels: IValue,
        mode: string,
        configVersion: string,
        memorySize: string,
        deviceInter: string,
        sasAddress: string,
        stripSizeRange: string,
        cachePinned: string,
        faultMemory: string,
        driverName: string,
        driverVersion: string,
        copyback: ISwitch,
        smartErrors: ISwitch,
        jbodModel: ISwitch,
        volumeData: IChildData,
        driveArr: DriveCheckBox[],
        bbu: BBUModel,
        ibmaRunning: boolean,
        foreignConfigStatus: boolean,
    supportedModes: IOptions[],
    epdSupported: boolean,
    jbodStateSupported: boolean,
    noBBUWrite: string,
    pcieLinkWidth: number,
    readCache: string,
    spareActivationMode: string,
    raidType: string,
    writeCachePolivy,
    bootDevices,
    writeCacheData,
    autoRepairEnabled: boolean,
    completedVolumeCount: number,
    delayForStart: number,
    enabled: boolean,
    period: number,
    rate: string,
    runningStatus: boolean,
    totalVolumeCount: number
    ) {
        this.name = ['COMMON_NAME', name];
        this.type = ['COMMON_TYPE', type];
        this.firmwareVersion = ['STORE_FIRMWARE_VERSION', firmwareVersion];
        this.bandManag = ['STORE_OUTOFBAND', bandManag];
        this.childData = volumeData;
        this.driveArr = driveArr;
        this.bbu = bbu;
        this.foreignConfigStatus = foreignConfigStatus;
        this.ibmaRunning = ibmaRunning;
        this.tmpJbodModel = jbodModel;
        const isoobAndibma = volumeData.oobSupport || ibmaRunning;
        this.setSupportedModes = supportedModes;

        // 支持带外管理或ibma运行才显示的属性
        this.health = ['NET_HEALTH_STATUS', isoobAndibma ? health : null];
        this.raidLevels = ['STORE_RAID_LEVELS', isoobAndibma ? raidLevels : null];
    this.mode = [
      'COMMON_WORKING_MODE',
      isoobAndibma ? this.getWorkMode(mode) : null,
    ];
        this.memorySize = ['STORE_MEMORY_SIZE', isoobAndibma ? memorySize : null];
    this.deviceInter = [
      'STORE_SET_INTERFACE',
      isoobAndibma ? deviceInter : null,
    ];
        this.sasAddress = ['STORE_SAS_ADDRESS', isoobAndibma ? sasAddress : null];
    this.stripSizeRange = [
      'STORE_SIZE_RANGE',
      isoobAndibma ? stripSizeRange : null,
    ];
        this.driverName = ['NET_DRIVER_NAME', isoobAndibma ? driverName : null];
    this.driverVersion = [
      'NET_DRIVER_VERSION',
      isoobAndibma ? driverVersion : null,
    ];
    this.epdSupported = epdSupported;
    this.jbodStateSupported = jbodStateSupported;
    this.setJBODState(isoobAndibma, jbodModel);
    this.raidCardType = raidType;
    this.jbodModel = ['STORE_JBOD_MODE', isoobAndibma ? jbodModel : null];

    if (RaidType.BRCM === raidType) {
      // 支持带外管理或ibma运行，并且为博通卡才显示的属性
      this.cachePinned = [
        'STORE_CACHE_PINNED_STATUS',
        isoobAndibma ? cachePinned : null,
      ];
      this.faultMemory = [
        'STORE_FAULT_MEMORY',
        isoobAndibma ? faultMemory : null,
      ];
      this.configVersion = [
        'STORE_CONFIG_VERSIONS',
        isoobAndibma ? configVersion : null,
      ];
        this.copyback = ['STORE_COPYBACK', isoobAndibma ? copyback : null];
      this.smartErrors = [
        'STORE_SMART_ERRORS',
        isoobAndibma ? smartErrors : null,
      ];
      // 博通卡根据jbodStateSupported判断是否支持JBOD模式
      this.jbodModel = this.jbodStateSupported ?
        ['STORE_JBOD_MODE', isoobAndibma ? jbodModel : null] : [null, isoobAndibma ? jbodModel : null];
    } else if (RaidType.PMC === raidType || RaidType.HBA === raidType) {
      // PMC卡才显示的属性
      this.noBBUWrite = ['STORE_NO_BBU_WRITE', noBBUWrite];
      this.pcieLinkWidth = ['STORE_PCIE_LINKWIDTH', pcieLinkWidth];
      this.readCache = ['STORE_READ_CACHE', readCache];
      this.spareActivationMode = ['STORE_SPARE_ACTIVE', spareActivationMode];
      this.writeCachePolivy = ['STORE_DISK_WRITE_POLICY', writeCachePolivy];
      this.bootDevices = ['STORE_BOOT_DISK', bootDevices];
      this.writeCacheData = writeCacheData;
      // PMC卡不支持JBOD模式，需隐藏该字段
      this.jbodModel = [null, isoobAndibma ? jbodModel : null];
    } else if (RaidType.ARIES === raidType) {
      // Hi1880卡才显示的属性
      this.cachePinned = [
        'STORE_CACHE_PINNED_STATUS',
        isoobAndibma ? cachePinned : null
      ];
       // hi1880只有一个启动设备，需隐藏第二个启动设备None
       const ariesBootDevice = bootDevices.substring(0, bootDevices.indexOf('/'));
       this.bootDevices = ['STORE_BOOT_DISK', ariesBootDevice];
      // 增加带宽显示
      this.pcieLinkWidth = ['STORE_PCIE_LINKWIDTH', pcieLinkWidth];
      this.copyback = ['STORE_COPYBACK', isoobAndibma ? copyback : null];
      this.smartErrors = [
        'STORE_SMART_ERRORS',
        isoobAndibma ? smartErrors : null,
      ];

      // 一致性校验功能显示
      this.enabled = enabled;
      this.delayForStart = delayForStart;
      this.consisEnabled = ['STORE_CONSIS_CHECK', enabled ? 'STORE_ENABLED' : 'STORE_DISABLED'];
      if (runningStatus) {
          this.runningStatus = ['STORE_CONSIS_RUNNIG_STATUS', runningStatus ? 'FDM_ENABLED' : ''];
          this.period = ['STORE_CONSIS_PERIOD_INTEVAL', period];
          this.rate = ['STORE_CONSIS_VERIFIED_RATE', this.getRateValue(rate)];
          this.autoRepairEnabled = ['STORE_CONSIS_AUTO_REPAIR', autoRepairEnabled ? 'ON' : 'OFF'];
          this.completedVolumeCount = ['STORE_CONSIS_FINISHED_COUNTS', completedVolumeCount];
          this.totalVolumeCount = ['STORE_CONSIS_TARGET_COUNTS', totalVolumeCount];
    }
      this.consisCheckInfo = {
          runningStatus,
          period,
          rate: {id: rate, label: rate, name: 'IOptions' },
          autoRepairEnabled,
          completedVolumeCount,
          totalVolumeCount
      };
    }
  }

  get getEnabled() {
    return this.enabled;
  }

  get getRunningStatus() {
    return this.consisCheckInfo.runningStatus ?
        (this.runningStatus[1] === 'FDM_ENABLED' ? true : false) : this.consisCheckInfo.runningStatus;
  }

  get getAutoRepairEnabled() {
    return this.consisCheckInfo.runningStatus ?
        (this.autoRepairEnabled[1] === 'ON' ? true : false) : this.consisCheckInfo.autoRepairEnabled;
  }

  get getRate() {
    return this.consisCheckInfo.runningStatus ? this.rate[1] : this.consisCheckInfo.rate;
  }

  get getPeriod() {
    return this.consisCheckInfo.runningStatus ? this.period[1] : this.consisCheckInfo.period;
  }

  get getDelayForStart() {
    return this.delayForStart;
  }

    get getBBU() {
        return this.bbu;
    }

    get getName(): string {
        return this.name[1];
    }

  get getBootDevice() {
    return this.bootDevices ? this.bootDevices[1] : null;
  }

    get getVolumeSupport(): boolean {
        return this.childData.volumeSupport;
    }

    get getFirmwareVersion(): string {
        return this.firmwareVersion[1];
    }

    get getOOBSupport(): boolean {
        return this.childData.oobSupport;
    }

    get getChidrenData(): IChildData {
        return this.childData;
    }

    get getJBODState(): ISwitch {
        return this.jbodModel[1];
    }

    get getCoyBack(): ISwitch {
        return this.copyback[1];
    }

    get getSmartErrors(): ISwitch {
        return this.smartErrors[1];
    }

    get getRAIDLevels(): IValue {
        return this.raidLevels[1];
    }

    get getDriveArr(): DriveCheckBox[] {
        return this.driveArr;
    }

    get getForeignConfigStatus(): boolean {
        return this.foreignConfigStatus;
    }

    get getMode(): IOptions {
        return this.mode[1];
    }

    get getEpdSupported(): boolean {
      return this.epdSupported;
    }

    get getJbodStateSupported(): boolean {
      return this.jbodStateSupported;
    }

    get getBandManag(): boolean {
      return this.bandManag[1] === 'COMMON_YES';
    }

    set setEpdSupported(epdSupported: boolean) {
      this.epdSupported = epdSupported;
    }

    set setJbodStateSupported(jbodStateSupported: boolean) {
      this.jbodStateSupported = jbodStateSupported;
      const isoobAndibma = this.isoobAndibma();
      this.setJBODState(isoobAndibma, this.tmpJbodModel);
    }

    // 是否支持带外管理或ibma

    private isoobAndibma(): boolean {
      return this.childData.oobSupport || this.ibmaRunning;
    }
    get getBbuWriteState(): boolean {
      return this.noBBUWrite[1] === null
        ? null
        : this.noBBUWrite[1] === 'true'
        ? true
        : false;
    }
    get getReadCachePercent(): number {
      return this.readCache[1] === null
        ? null
        : Number(this.readCache[1].replace('%', ''));
    }

    get getRaidType(): string {
      return this.raidCardType;
    }

    get getWriteCacheData(): IWriteCachePolicy | null {
      return this.writeCacheData;
    }

    // 设置JBOD模式
    // @param isoobAndibma 支持带外管理或ibma
    // @param jbodModel JBOD模式
    // 使用“JbodStateSupported”字段来判断；“JbodStateSupported”当值为0时，web前端做隐藏处理，表示RAID卡不支持该功能；
    // 如果工作模式为JBOD，JBOD模式不显示，反之则显示
    // 界面不显示的属性不能拥有国际化：设置JBOD模式国际化为null，界面自动隐藏

    private setJBODState(isoobAndibma: boolean, jbodModel: ISwitch): void {
        this.jbodModel = [
            this.jbodStateSupported ? 'STORE_JBOD_MODE' : null,
            isoobAndibma ? jbodModel : null
        ];
    }
    set setSupportedModes(options: IOptions[]) {
        this.supportedModes = {
            disable: false,
      options,
        };
    }

    get getSupportedModes(): ISelect {
        return this.supportedModes;
    }

    private getWorkMode(mode: string): IOptions {
    const option =
      this.supportedModes.options?.length > 0
        ? this.supportedModes.options.filter((opt) => opt.id === mode)
        : mode
        ? [
            {
            name: 'IOptions',
            id: mode,
              label: mode,
            },
          ]
        : null;
        return option && option.length > 0 ? option[0] : null;
    }

  private getRateValue(key: string): IOptions {
    const option = StorageStaticData.checkRateOptions.filter((opt) => opt.id === key);
    return option && option.length > 0 ? option[0] : null;
  }
}
