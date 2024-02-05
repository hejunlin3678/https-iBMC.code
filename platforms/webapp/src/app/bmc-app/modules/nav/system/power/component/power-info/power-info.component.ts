import { Component, OnInit, Input, OnChanges, ViewEncapsulation } from '@angular/core';
import { PowerInfoCard } from '../../model/power-info-card';
import { TiModalService, TiModalRef } from '@cloud/tiny3';
import { PowerInfoSettingComponent } from '../power-info-setting/power-info-setting.component';
import { Power } from '../../model/power';
import { PowerService } from '../../service/power.service';
import * as utils from 'src/app/common-app/utils';
import { SystemLockService, ErrortipService, AlertMessageService } from 'src/app/common-app/service';

@Component({
  selector: 'app-power-info',
  templateUrl: './power-info.component.html',
  styleUrls: ['./power-info.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class PowerInfoComponent implements OnInit, OnChanges {

    @Input() powerInfoList: PowerInfoCard[];
    @Input() powerSettingInfo: Power;
    @Input() isPrivileges: boolean;
    public isSystemLock: boolean = false;
    public isPowerSetting: boolean = true;
    public powerInfoModal;

    constructor(
        private tiModal: TiModalService,
        private alert: AlertMessageService,
        private service: PowerService,
        private errorTip: ErrortipService,
        private lockService: SystemLockService
    ) { }

    public powerSetting() {
        if (this.isSystemLock) {
            return;
        }
        this.powerInfoModal = this.tiModal.open(PowerInfoSettingComponent, {
            id: 'powerModalId',
            modalClass: 'powerInfoSetModal',
            context: {
                isActiveAndStandby: this.powerSettingInfo.getIsActiveAndStandby(),
                activePSU: this.powerSettingInfo.getAactivePSU(),
                deepSleep: this.powerSettingInfo.getDeepSleep(),
                normalAndRedundancy: this.powerSettingInfo.normalAndRedundancy,
                param: null
            },
            close: (modalRef: TiModalRef): void => {
                const param = modalRef.content.instance.param;
                if (param) {
                    if (param === 1) {
                        this.alert.eventEmit.emit({type: 'error', label: 'POWER_NO_STAND'});
                    } else if ( param === 2) {
                        this.alert.eventEmit.emit({type: 'error', label: 'POWER_SELECT_MAIN'});
                    } else {
                        this.service.setPowerSupply(param).subscribe((response) => {
                            const data = response.body.data || response.body;
                            const powerInfoList = this.service.getPowerInfo(data);
                            this.powerInfoList = powerInfoList;
                            const powerSetting = this.service.getPowerSettingInfo(data);
                            this.powerSettingInfo.setIsActiveAndStandby(powerSetting.isActiveAndStandby);
                            this.powerSettingInfo.setAactivePSU(powerSetting.activePSU);
                            this.powerSettingInfo.setDeepSleep(powerSetting.deepSleep);
                            this.powerSettingInfo.normalAndRedundancy = powerSetting.normalAndRedundancy;
                            if (response.body.error) {
                                const errorId: string = this.errorTip.getErrorMessage(utils.getMessageId(response.body)[0].errorId) || 'COMMON_FAILED';
                                this.alert.eventEmit.emit({type: 'error', label: errorId});
                            } else {
                                this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                            }
                        }, (error) => {
                            const powerInfoList = this.service.getPowerInfo(error.error.data);
                            this.powerInfoList = powerInfoList;
                            const powerSetting = this.service.getPowerSettingInfo(error.error.data);
                            this.powerSettingInfo.setIsActiveAndStandby(powerSetting.isActiveAndStandby);
                            this.powerSettingInfo.setAactivePSU(powerSetting.activePSU);
                            this.powerSettingInfo.setDeepSleep(powerSetting.deepSleep);
                            this.powerSettingInfo.normalAndRedundancy = powerSetting.normalAndRedundancy;
                        });
                    }
                }
            },
            dismiss: (): void => {
            }
    });
    }

    ngOnInit(): void {
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
           this.isSystemLock = res;
           if (res && this.powerInfoModal) {
               this.powerInfoModal.dismiss();
           }
       });
   }

    ngOnChanges(): void {
        const deepSleep = this.powerSettingInfo.getDeepSleep();
        if (!this.powerSettingInfo.getIsActiveAndStandby() && deepSleep === null) {
            this.isPowerSetting = false;
        }
    }

}
