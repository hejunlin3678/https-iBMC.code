import { Component, Input, OnChanges, SimpleChanges } from '@angular/core';
import { BiosService } from '../../services/bios.service';
import { AlertMessageService, UserInfoService } from 'src/app/common-app/service';
import { Bios } from '../../model/bios';
import { BootType } from '../../model/boot-type';
import { TranslateService } from '@ngx-translate/core';
import { IBootTitle, IBootMode, IBootMedia } from '../../model/bios-interface';

@Component({
    selector: 'app-boot-device',
    templateUrl: './boot-device.component.html',
    styleUrls: ['./boot-device.component.scss']
})
export class BootDeviceComponent implements OnChanges {

    public buttonDisable: boolean = true;
    @Input() public biosModel: Bios;
    @Input() public initializesBios: Bios;
    @Input() public isSystemLock: boolean;

    constructor(
        private user: UserInfoService,
        private service: BiosService,
        private alert: AlertMessageService,
        private translate: TranslateService
    ) { }

    public isPrivileges = this.user.hasPrivileges(['ConfigureComponents']);
    public isArm: boolean = this.user.platform === 'Arm';
    public legacyShow: boolean = false;

    public ipmiBootMode: IBootTitle = {
        title: this.translate.instant('BIOS_IPMI_BOOT_MODE')
    };

    public bootMode: IBootMode = {
        title: this.translate.instant('BIOS_BOOT_MODE'),
        options: [{
            label: this.translate.instant('BIOS_LEGACY_BIOS'),
            value: 'Legacy'
        }, {
            label: this.translate.instant('BIOS_UEFI'),
            value: 'UEFI'
        }]
    };

    public bootMedia: IBootMedia = {
        title: this.translate.instant('BIOS_BOOT_MEDIUM'),
        radio: {
            options: [{
                label: this.translate.instant('BIOS_ONE_TIME'),
                value: 'Once'
            }, {
                label: this.translate.instant('BIOS_PERMANENT'),
                value: 'Continuous'
            }]
        }
    };

    public bootSequence: IBootTitle = {
        title: this.translate.instant('BIOS_BOOT_PRIORITY')
    };

    public ipmiChange(state: boolean): void {
        this.biosModel.bootModeConfigOverIpmiEnabled = state;
        this.buttonMonitor();
    }

    public bootModeChange(): void {
        this.buttonMonitor();
    }
    public bootMediaSelectChange(data): void {
        this.biosModel.bootSourceOverrideTarget = data;
        this.buttonMonitor();
    }
    public bootMediaRadioChange(): void {
        this.buttonMonitor();
    }
    public bootSequenceChange(item: BootType, direct: number, index: number): void {
        /**
         * 上下移动原理
         * 传入参数：当前数据，移动方向，当前数据的索引
         * 当超出界限时，不做处理
         * 根据传入的方向，替换数组中的数据，重新渲染界面
         */
        if (this.isSystemLock) {
            return;
        }
        if (direct !== 1 && direct !== -1) { return; }
        if ((direct === 1 && index >= this.biosModel.bootTypeOrder.length - 1) || (direct === -1 && index <= 0)) { return; }
        const _index = this.biosModel.bootTypeOrder.indexOf(item);
        if (direct === 1) {
            this.biosModel.bootTypeOrder.splice(_index, 1);
            this.biosModel.bootTypeOrder.splice(_index + 1, 0, item);
        } else {
            this.biosModel.bootTypeOrder.splice(_index, 1);
            this.biosModel.bootTypeOrder.splice(_index - 1, 0, item);
        }
        this.buttonMonitor();
    }
    public bootMediaSelectExtChange(event: string[]) {
        this.biosModel.bootSourceOverrideTargetExt = event;
        this.buttonMonitor();
    }

    private getParam() {
        const param = {};
        // IPMI参数拼装
        if (this.biosModel.bootModeConfigOverIpmiEnabled !== this.initializesBios.bootModeConfigOverIpmiEnabled) {
            param['BootModeConfigOverIpmiEnabled'] = this.biosModel.bootModeConfigOverIpmiEnabled ? 'On' : 'Off';
        }

        // 启动模式参数拼装
        if (this.biosModel.bootSourceOverrideMode !== this.initializesBios.bootSourceOverrideMode) {
            param['BootSourceOverrideMode'] = this.biosModel.bootSourceOverrideMode;
        }
        // 优先引导介质和引导介质有效期参数拼装（当2者只要有一个发生变化时，都必须传入2个参数）
        const flag1 = this.biosModel.bootSourceOverrideTarget.id !== this.initializesBios.bootSourceOverrideTarget.id;
        const flag2 = this.biosModel.bootSourceOverrideEnabled !== this.initializesBios.bootSourceOverrideEnabled;
        const flag3 = this.biosModel.bootSourceOverrideTargetExt[this.biosModel.bootSourceOverrideTargetExt.length - 1]
            !== this.initializesBios.bootSourceOverrideTargetExt[this.initializesBios.bootSourceOverrideTargetExt.length - 1];

        if (flag1 || flag2 || flag3) {
            if (param['PreferredBootMedium']) {
                param['PreferredBootMedium'].EffectivePeriod = this.biosModel.bootSourceOverrideEnabled;
                if (flag1) {
                    param['PreferredBootMedium'].Target = this.biosModel.bootSourceOverrideTarget.id;
                }
                if (flag3) {
                    param['PreferredBootMedium'].Target = this.biosModel.bootSourceOverrideTargetExt[0];
                    param['PreferredBootMedium'].TargetExt = this.biosModel.bootSourceOverrideTargetExt.length > 1
                        ? this.biosModel.bootSourceOverrideTargetExt[this.biosModel.bootSourceOverrideTargetExt.length - 1]
                        : 'None';
                }
            } else {
                if (flag1) {
                    param['PreferredBootMedium'] = {
                        EffectivePeriod: this.biosModel.bootSourceOverrideEnabled,
                        Target: this.biosModel.bootSourceOverrideTarget.id
                    };
                }
                if (flag3) {
                    param['PreferredBootMedium'] = {
                        EffectivePeriod: this.biosModel.bootSourceOverrideEnabled,
                        Target: this.biosModel.bootSourceOverrideTargetExt[0],
                        TargetExt: this.biosModel.bootSourceOverrideTargetExt.length > 1
                            ? this.biosModel.bootSourceOverrideTargetExt[this.biosModel.bootSourceOverrideTargetExt.length - 1]
                            : 'None'
                    };
                }
                if (flag2 && !flag1 && !flag3) {
                    param['PreferredBootMedium'] = {
                        EffectivePeriod: this.biosModel.bootSourceOverrideEnabled,
                        Target: this.biosModel.bootSourceOverrideTarget.id
                            ? this.biosModel.bootSourceOverrideTarget.id
                            : this.biosModel.bootSourceOverrideTargetExt[0]
                    };
                    if (this.biosModel.bootSourceOverrideTargetExt.length > 0) {
                        param['PreferredBootMedium'].TargetExt = 'None';
                    }
                    if (this.biosModel.bootSourceOverrideTargetExt.length > 1) {
                        param['PreferredBootMedium'].TargetExt =
                            this.biosModel.bootSourceOverrideTargetExt[this.biosModel.bootSourceOverrideTargetExt.length - 1];
                    }
                }
            }
        }
        if (JSON.stringify(this.biosModel.bootTypeOrder) !== JSON.stringify(this.initializesBios.bootTypeOrder)) {
            const typeOrders = this.biosModel.bootTypeOrder;
            const bootOrders = [];
            for (const typeOrder of typeOrders) {
                bootOrders.push(typeOrder.value);
            }
            param['BootOrder'] = bootOrders;
        }
        return param;
    }
    private buttonMonitor() {
        const param = this.getParam();
        this.buttonDisable = JSON.stringify(param) === '{}';
    }
    public bootDeviceSave() {
        if (this.isSystemLock) {
            return;
        }
        this.buttonDisable = true;
        const param = this.getParam();
        this.service.setBoot(param).subscribe((response) => {
            const bootData = this.service.getBootData(response.body);
            this.service.setBootModel(this.biosModel, bootData);
            const initializesBoot = this.service.getBootData(response.body);
            this.service.setBootModel(this.initializesBios, initializesBoot);
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
        }, (error) => {
            const bootData = this.service.getBootData(error.error.data);
            this.service.setBootModel(this.biosModel, bootData);
            const initializesBoot = this.service.getBootData(error.error.data);
            this.service.setBootModel(this.initializesBios, initializesBoot);
        });
    }
    ngOnChanges(changes: SimpleChanges): void {
        if (changes && changes.biosModel) {
            this.legacyShow = changes.biosModel.currentValue.bootModeIpmiSWDisplayEnabled;
        }
    }
}
