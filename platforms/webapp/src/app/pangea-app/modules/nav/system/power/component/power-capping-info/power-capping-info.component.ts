import { Component, OnInit, Input, OnChanges } from '@angular/core';
import { PowerCapping } from '../../model/power-capping';
import { TiMessageService, TiModalService, TiModalRef } from '@cloud/tiny3';
import { PowerService } from '../../service/power.service';
import { PowerCappingSettingComponent } from '../power-capping-setting/power-capping-setting.component';
import { TranslateService } from '@ngx-translate/core';
import { IPowerCapLabel, Platform } from '../../interface';
import { formatEntry } from 'src/app/common-app/utils';
import { AlertMessageService, UserInfoService, SystemLockService } from 'src/app/common-app/service';

@Component({
    selector: 'app-power-capping-info',
    templateUrl: './power-capping-info.component.html',
    styleUrls: ['./power-capping-info.component.scss']
})
export class PowerCappingInfoComponent implements OnInit, OnChanges {

    @Input() powerCappingInfo: PowerCapping;
    @Input() powerUnit: string;
    @Input() isPrivileges: boolean;
    public isSystemLock: boolean = true;
    public powerTiModal;

    public units = 'W';
    public totalUnits = 'kWh';
    public isArm = Platform.ARM === this.user.platform;

    public currentPower: IPowerCapLabel = {
        label: this.translate.instant('POWER_CURRENT'),
        value: null,
        desc: ''
    };
    public peakPower: IPowerCapLabel = {
        label: this.translate.instant('POWER_SYSTEM_PEAK_POWER'),
        value: null,
        desc: ''
    };
    public currentCPUPower: IPowerCapLabel = {
        label: this.translate.instant('POWER_CPU_CURRT_POWER_CONSUMPTION'),
        value: null
    };
    public averagePower: IPowerCapLabel = {
        label: this.translate.instant('POWER_SYSTEM_AVG_POWER'),
        value: null
    };
    public currentMemoryPower: IPowerCapLabel = {
        label: this.translate.instant('POWER_MEMORY_CURRT_POWER_CONSUMPTION'),
        value: null
    };
    public totalConsumedPower: IPowerCapLabel = {
        label: this.translate.instant('POWER_SYSTEM_CUMULATIVE_POWER_CONSUMPTION'),
        value: null
    };

    private utilBtu() {
        this.currentPower.value = this.service.wattstoBTUs(this.powerCappingInfo.getPowerConsumedWatts());
        this.peakPower.value = this.service.wattstoBTUs(this.powerCappingInfo.getMaxConsumedWatts());
        this.currentCPUPower.value = this.service.wattstoBTUs(this.powerCappingInfo.getCurrentCPUPowerWatts());
        this.averagePower.value = this.service.wattstoBTUs(this.powerCappingInfo.getAverageConsumedWatts());
        this.currentMemoryPower.value = this.service.wattstoBTUs(this.powerCappingInfo.getCurrentMemoryPowerWatts());
        this.totalConsumedPower.value = this.service.wattstoBTUs(this.powerCappingInfo.getTotalConsumedPowerkWh() * 1000);
        this.initDesc();
    }

    private utilw() {
        this.currentPower.value = this.powerCappingInfo.getPowerConsumedWatts();
        this.peakPower.value = this.powerCappingInfo.getMaxConsumedWatts();
        this.currentCPUPower.value = this.powerCappingInfo.getCurrentCPUPowerWatts();
        this.averagePower.value = this.powerCappingInfo.getAverageConsumedWatts();
        this.currentMemoryPower.value = this.powerCappingInfo.getCurrentMemoryPowerWatts();
        this.totalConsumedPower.value = this.powerCappingInfo.getTotalConsumedPowerkWh();
        this.initDesc();
    }

    private initDesc() {
        const limitException =  this.powerCappingInfo.getLimitException();
        let limitInWatts = this.powerCappingInfo.getLimitInWatts();
        if (this.units !== 'W') {
            limitInWatts = this.service.wattstoBTUs(limitInWatts);
        }
        // 功率封顶使能关闭状态
        if (this.powerCappingInfo.getPowerLimitState() === false) {
            this.currentPower.desc = this.translate.instant('POWER_MODE_LOFF');
        } else if (limitInWatts !== null) {
            // 功率封顶使能开启 且有功率封顶值
            this.currentPower.desc = formatEntry(this.translate.instant('POWER_TO'), [limitInWatts + this.units]);
            // 持功率封顶失效策略且自动关机
            if (limitException) {
                this.currentPower.desc += this.translate.instant('POWER_AUTO_POWER_OFF');
            }
            // 支持功率封顶失效策略且不关机
            if (limitException === false) {
                this.currentPower.desc += this.translate.instant('POWER_NOT_POWER_OFF');
            }
        }
        this.peakPower.desc = this.translate.instant('POWER_PEAK_OCCURRED') + this.powerCappingInfo.getMaxConsumedOccurred();
    }

    private setUnits() {
        if (this.powerUnit === 'W') {
            this.units = 'W';
            this.totalUnits = 'kWh';
            this.utilw();
        } else {
            this.units = 'BTU/h';
            this.totalUnits = 'BTU';
            this.utilBtu();
        }
    }

    private refresh() {
        this.service.getPower().subscribe((response) => {
            const powerCapping = this.service.getPowerCapping(response.body, this.units);
            this.powerCappingInfo = new PowerCapping(powerCapping);
            this.setUnits();
        });
    }

    public reset() {
        if (this.isSystemLock) {
            return;
        }
        this.powerTiModal = this.tiMessage.open({
            id: 'powerCapDialog',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: { },
            close: () => {
                this.service.resetPowerCapping().subscribe((response) => {
                    setTimeout(() => {
                        this.refresh();
                    }, 1000);
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                });
            },
            dismiss: () => {

            }
        });
    }

    public setting() {
        if (this.isSystemLock) {
            return;
        }
        this.powerTiModal = this.tiModal.open(PowerCappingSettingComponent, {
            id: 'powerCappingModal',
            modalClass: 'cappingModal',
            context: {
                powerLimitState: this.powerCappingInfo.getPowerLimitState(),
                limitInWatts: this.powerCappingInfo.getLimitInWatts(),
                limitException: this.powerCappingInfo.getLimitException(),
                minPowerLimitInWatts: this.powerCappingInfo.getMinPowerLimitInWatts(),
                maxPowerLimitInWatts: this.powerCappingInfo.getMaxPowerLimitInWatts(),
                isLimitState: this.powerCappingInfo.isLimitState,
                powerUnit: this.powerUnit,
                capMode: this.powerCappingInfo.powerCapMode,
                param: null
            },
            close: (modalRef: TiModalRef): void => {
                const param = modalRef.content.instance.param;
                if (param) {
                    this.service.setPower(param).subscribe((response) => {
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        const powerCapping = this.service.getPowerCapping(response.body, this.units);
                        this.powerCappingInfo = new PowerCapping(powerCapping);
                        this.setUnits();
                    }, (error) => {
                        const powerCapping = this.service.getPowerCapping(error.error.data, this.units);
                        this.powerCappingInfo = new PowerCapping(powerCapping);
                        this.setUnits();
                    });
                }
            },
            dismiss: (): void => { }
        });
    }

    constructor(
        private tiMessage: TiMessageService,
        private service: PowerService,
        private alert: AlertMessageService,
        private tiModal: TiModalService,
        private user: UserInfoService,
        private translate: TranslateService,
        private lockService: SystemLockService
    ) { }

    ngOnChanges(): void {
        if (this.powerUnit === 'W') {
            this.units = 'W';
            this.totalUnits = 'kWh';
            this.utilw();
        } else {
            this.units = 'BTU/h';
            this.totalUnits = 'BTU';
            this.utilBtu();
        }
    }

    ngOnInit(): void {
        this.initDesc();
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.powerTiModal) {
                this.powerTiModal.dismiss();
            }
        });
    }

}
