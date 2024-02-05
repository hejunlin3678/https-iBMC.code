import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import {
  IFromSet,
  Raid,
  RaidEditModel,
  RaidNode,
  RaidType,
} from '../../../models';
import { RaidEditService } from './raid-edit.service';
import { AlertMessageService, LoadingService } from 'src/app/common-app/service';
import { TiMessageService, TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { getMessageId, errorTip, formatEntry } from 'src/app/common-app/utils';
import { successTip } from '../../../utils';
import { StorageStaticData } from '../../../models/static-data/storage-static.model';
import { UntypedFormBuilder } from '@angular/forms';
import { IOptions, IWriteCachePolicy } from 'src/app/common-app/models';

@Component({
  templateUrl: './raid-edit.component.html',
  styleUrls: ['./raid-edit.component.scss'],
})
export class RaidEditComponent implements OnInit {
  @Input() raid: Raid;
  @Input() raidNode: RaidNode;
  public raidEditModel: RaidEditModel;
  public writeCacheType: IOptions[] = StorageStaticData.writeCacheType;
  public writeCachePolicy: IOptions[] = StorageStaticData.writeCachePolicy;
  public checkRateOptions: IOptions[] = StorageStaticData.checkRateOptions;
  public formSet: IFromSet = {
    lineHeight: '22px',
    fieldVerticalAlign: 'middle',
    formType: 'type',
  };
  public modeShow: boolean;
  public isARIESrCard: boolean = false;
  public isPMCCard: boolean = false;
  public bootList: IOptions[];
  public scales: number[] = [0, 20, 40, 60, 80, 100];
  public readCachePercentDisabled: boolean = false;
  private baseNoBBUState: boolean;
  public readCachePercent: number;
  public tipInfo: string = '';
  public bootDiskList = [];
  public taskDisable: boolean = true;
  public periodScales: number[] = [24, 288, 576, 864, 1152, 1440];

  public waitTimeValidation: TiValidationConfig = {
      tip: formatEntry(this.translate.instant('IRM_INFO_VALUE_RANGE_TIP'), [0, 24]),
      tipPosition: 'top',
      type: 'blur',
      errorMessage: {
        pattern: formatEntry(this.translate.instant('IRM_INFO_VALUE_RANGE_TIP'), [0, 24])
    }
  };

  constructor(
    private translate: TranslateService,
    private loading: LoadingService,
    private raidEditService: RaidEditService,
    private tiMessage: TiMessageService,
    private alertService: AlertMessageService,
    private alert: AlertMessageService,
    private fb: UntypedFormBuilder
  ) { }

  ngOnInit() {
    if (this.raid.getRaidType === RaidType.ARIES) {
      this.isARIESrCard = true;
      const bootDevices = this.raid.getBootDevice.split('/');
      const driveArr = this.raidNode.getRaid.getDriveArr;
      this.raidNode?.children?.forEach((item) => {
        // 屏蔽非online固件状态的物理盘
        const obj = driveArr.find((ele) => {
          return (
            (ele.getDriveID === item.id && ele.getFirmware === 'Online') ||
            item.componentName === 'VolumeNode'
          );
        });
        if (obj) {
          this.bootDiskList.push({
            id: item.id,
            label: item.label,
            name: 'IOptions'
          });
        }
      });
      let checkedArray: IOptions[] = [];
      // 设置启动盘选中项
      checkedArray = this.bootDiskList?.filter(item => item.id === bootDevices[0] || item.label === bootDevices[0]);
      if (checkedArray.length === 0) {
        checkedArray.push({
          name: 'IOptions',
          id: 'None',
          label: 'None'
        });
      }
      RaidEditModel.initRaidEdit(
        this.raid.getJBODState.id as boolean,
        this.raid.getCoyBack.id as boolean,
        this.raid.getSmartErrors.id as boolean,
        this.raid.getMode,
        this.raid.getJbodStateSupported,
        null,
        null,
        checkedArray[0],
        null,
        null,
        null,
        this.raid.getEnabled,
        this.raid.getRunningStatus,
        this.raid.getAutoRepairEnabled,
        this.raid.getRate,
        this.raid.getPeriod,
        this.raid.getDelayForStart
      );
      const formGroup =  this.fb.group({
          waitTimeControl: [
              { value: this.raid.getDelayForStart, disabled: true },
              [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(0, 24)]
          ]
      });
      RaidEditModel.setPeriodAndTaskWaitTimeform(formGroup);
    } else if (this.raid.getRaidType === RaidType.BRCM) {
      RaidEditModel.initRaidEdit(
        this.raid.getJBODState.id as boolean,
        this.raid.getCoyBack.id as boolean,
        this.raid.getSmartErrors.id as boolean,
        this.raid.getMode,
        this.raid.getJbodStateSupported
      );
    } else if (this.raid.getRaidType === RaidType.PMC || this.raid.getRaidType === RaidType.HBA) {
      this.isPMCCard = true;
      this.tipInfo = this.translate.instant('STORE_TIP_WORKMODE');
      // 选择启动盘
      this.bootList = [];
      this.raidNode.children?.forEach((item) => {
        if (this.raid.getMode?.id === 'RAID') {
          if (item.componentName === 'VolumeNode' && !item?.getSSDCachecadeVolume) {
            this.bootList.push({
              name: 'IOptions',
              id: item.id,
              label: item.label,
            });
          }
        } else {
          if (!item?.getSSDCachecadeVolume) {
            this.bootList.push({
              name: 'IOptions',
              id: item.id,
              label: item.label,
            });
          }
        }
      });
      // 启动盘设备也支持None
      this.bootList.push({
        name: 'IOptions',
        id: 'None',
        label: 'None'
      });
      let bootDevice1 = null;
      let bootDevice2 = null;
      if (this.raid.getBootDevice && this.raid.getBootDevice.length > 0) {
        const bootArr = this.raid.getBootDevice.split('/');
        bootDevice1 = this.bootList.filter((item) => {
          return item.label === bootArr[0] || item.id === bootArr[0];
        })[0];
        bootDevice2 = this.bootList.filter((item) => {
          return item.label === bootArr[1] || item.id === bootArr[1];
        })[0];
      }
      // 选择硬盘cache写策略, 默认选中第一个功能项的值
      const writeCacheData: IWriteCachePolicy = this.raid.getWriteCacheData;
      const type = this.writeCacheType[0];
      let policy = null;
      if (writeCacheData) {
        policy =
          this.writeCachePolicy.filter((item) => {
            return item.id === writeCacheData[type.id as string];
          })[0] || null;
      }
      this.readCachePercent = this.raid.getReadCachePercent;

      RaidEditModel.initRaidEdit(
        null,
        null,
        null,
        this.raid.getMode,
        false,
        this.raid.getRaidType === RaidType.PMC ? this.raid.getBbuWriteState : null,
        this.raid.getRaidType === RaidType.PMC ? this.raid.getReadCachePercent : null,
        bootDevice1,
        bootDevice2,
        type,
        policy
      );
      // 记录无电池写模式的初始值
      this.baseNoBBUState = this.raid.getBbuWriteState;
    }

    this.raidEditModel = RaidEditModel.getInstance();
    if (this.isPMCCard) {
      // maxCache逻辑盘列表
      const maxCacheList =
        this.raidNode?.children?.filter(
          (item) =>
            item.componentName === 'VolumeNode' && item?.getSSDCachecadeVolume
        ) || [];
      // ReadCachePercent值为null时或存在maxCache逻辑盘时禁止设置
      if (maxCacheList.length > 0) {
        this.readCachePercentDisabled = true;
      }
    }

    this.modeShow =
      this.raid.getSupportedModes &&
      this.raid.getSupportedModes?.options?.length > 0 &&
      Boolean(this.raidEditModel.getSelectData) &&
      Boolean(this.raidEditModel.getSelectData.mode);
  }
  /**
   * 模板中实际调用的是Modal服务提供的close和dismiss方法，并非此处定义的方法；
   * 在此处定义close和dismiss方法只是为了避免生产环境打包时报错
   */
  close(): void {}
  dismiss(): void {}
  save(): void {
    const radioEditInstance = this.tiMessage.open({
      id: 'radioEditMessage',
      type: 'prompt',
      title: this.translate.instant('COMMON_CONFIRM'),
      content: this.translate.instant('COMMON_ASK_OK'),
      okButton: {
        text: this.translate.instant('COMMON_OK'),
        autofocus: false,
        click: () => {
          this.loading.state.next(true);
          this.raidEditService.updateEditData().subscribe(
            () => {
              this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS',
              });
              this.loading.state.next(false);
              this.close();
            },
            (error: any) => {
              this.errorTip(error);
              this.close();
            }
          );
          radioEditInstance.close();
        },
      },
      cancelButton: {
        text: this.translate.instant('COMMON_CANCEL'),
      },
    });
  }

  reset(): void {
    const radioResetInstance = this.tiMessage.open({
      id: 'radioResetMessage',
      type: 'prompt',
      title: this.translate.instant('COMMON_CONFIRM'),
      content: this.translate.instant('COMMON_ASK_OK'),
      okButton: {
        text: this.translate.instant('COMMON_OK'),
        autofocus: false,
        click: () => {
          this.loading.state.next(true);
          this.raidEditService.reset().subscribe(
            (success) => {
              this.alertService.eventEmit.emit(
                successTip('COMMON_SUCCESS', this.translate)
              );
              this.loading.state.next(false);
              this.close();
            },
            (error) => {
              this.errorTip(error);
              this.close();
            }
          );
          radioResetInstance.close();
        },
      },
      cancelButton: {
        text: this.translate.instant('COMMON_CANCEL'),
      },
    });
  }
  // 导入Foreign配置
  foreignConfigFn(): void {
    if (this.raid.getForeignConfigStatus) {
      return;
    }
    const foreignConfigInstance = this.tiMessage.open({
      id: 'foreignConfigMessage',
      type: 'prompt',
      title: this.translate.instant('COMMON_CONFIRM'),
      content: this.translate.instant('STORE_IMPORT_FOREIGN_CONFIG_CONFIRM'),
      okButton: {
        text: this.translate.instant('COMMON_OK'),
        autofocus: false,
        click: () => {
          this.loading.state.next(true);
          this.raidEditService.foreignConfig().subscribe(
            () => {
              this.alertService.eventEmit.emit(
                successTip('COMMON_SUCCESS', this.translate)
              );
              this.loading.state.next(false);
              this.close();
            },
            (error) => {
              this.errorTip(error);
              this.close();
            }
          );
          foreignConfigInstance.close();
        },
      },
      cancelButton: {
        text: this.translate.instant('COMMON_CANCEL'),
      },
    });
  }

  // 清除Foreign配置
  clearForeignConfigFn(): void {
    if (this.raid.getForeignConfigStatus) {
      return;
    }
    const foreignConfigInstance = this.tiMessage.open({
      id: 'foreignConfigMessage',
      type: 'prompt',
      title: this.translate.instant('COMMON_CONFIRM'),
      content: this.translate.instant('STORE_CLEAR_FOREIGN_CONFIG_CONFIRM'),
      okButton: {
        text: this.translate.instant('COMMON_OK'),
        autofocus: false,
        click: () => {
          this.loading.state.next(true);
          this.raidEditService.clearForeignConfig().subscribe(
            () => {
              this.alertService.eventEmit.emit(
                successTip('COMMON_SUCCESS', this.translate)
              );
              this.loading.state.next(false);
              this.close();
            },
            (error) => {
              this.errorTip(error);
              this.close();
            }
          );
          foreignConfigInstance.close();
        },
      },
      cancelButton: {
        text: this.translate.instant('COMMON_CANCEL'),
      },
    });
  }

  // 无电池写缓存模式由关闭到开启时，提示文字信息
  public noBbuWriteChagne(value: boolean) {
    this.raidEditModel.setNoBBUWriteState(value);
    if (this.baseNoBBUState === false && value) {
      this.alertService.eventEmit.emit({
        label: this.translate.instant('STORE_TIP_NO_BBU'),
        type: 'warn',
        className: 'no-bbu',
      });
    }
  }

  private errorTip(error: any): void {
    const msgId = error.error
      ? getMessageId(error.error)[0].errorId
      : 'COMMON_FAILED';
    this.alertService.eventEmit.emit(errorTip(msgId, this.translate));
    this.loading.state.next(false);
  }
}
