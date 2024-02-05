import { Component, OnInit, Input, OnDestroy } from '@angular/core';
import { PowerService } from '../../service/power.service';
import { PowerControl } from '../../model/power-control';
import { TiMessageService, TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { PowerControlValid } from '../../service/PowerControlValid';
import { TranslateService } from '@ngx-translate/core';
import { PowerOnPolicy, PowerOnDelayed, Platform } from '../../interface';
import { AlertMessageService, UserInfoService, LoadingService, SystemLockService } from 'src/app/common-app/service';
import { formatEntry } from 'src/app/common-app/utils';

@Component({
    selector: 'app-power-control',
    templateUrl: './power-control.component.html',
    styleUrls: ['./power-control.component.scss']
})
export class PowerControlComponent implements OnInit, OnDestroy {

    constructor(
        private service: PowerService,
        private loading: LoadingService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private user: UserInfoService,
        private translate: TranslateService,
        private lockService: SystemLockService
    ) { }

    @Input() isPrivileges: boolean;
    @Input() powerContorlInfo: PowerControl;
    public isSystemLock: boolean = false;
    public isArm: boolean = this.user.platform === Platform.ARM;
    public leakageSupport: boolean = false;
    public powerControlModal;

    public interval;

    public forcedOffTip: string = this.translate.instant('POWER_FORCED_OFF_TIP');
    public forcedRestartTip: string = this.translate.instant('POWER_FORCED_RESTART_TIP');
    public forcedCycleTip: string = this.translate.instant('POWER_FORCED_CYCLE_TIP');
    public nmiTip: string = this.translate.instant('POWER_NMI_TIP');
    public panelBtnTip: string = this.translate.instant('POWER_PANEL_BTN_TIP');

    public powerOff = {
        show: false,
        value: null,
        validationRules: [TiValidators.required, TiValidators.integer],
        iconTip: this.translate.instant('COMMON_EDIT'),
        cancel: () => {
            this.powerOff.value = this.powerContorlInfo.getSafePowerOffTimoutSeconds().toString();
        },
        confirm: () => {
            const param = {
                PowerOffAfterTimeout: {
                    TimoutSeconds: parseInt(this.powerOff.value, 10)
                }
            };
            this.service.setPowerControl(param).subscribe((response) => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                const powerContorlInfo = this.service.getPowerControlInfo(response.body);
                this.powerContorlInfo.setSafePowerOffTimoutSeconds(powerContorlInfo.safePowerOffTimoutSeconds);
                this.init();
            }, (error) => {
                const powerContorlInfo = this.service.getPowerControlInfo(error.error.data);
                this.powerContorlInfo.setSafePowerOffTimoutSeconds(powerContorlInfo.safePowerOffTimoutSeconds);
                this.init();
            });
        }
    };


    public panelPower = {
        switchState: null,
        change: () => {
            if (!this.isPrivileges) {
                return;
            }
            const param = {
                PanelPowerButtonEnabled: this.panelPower.switchState ? 'Off' : 'On'
            };
            this.powerControlModal = this.tiMessage.open({
                id: 'panelBtnDialog',
                type: 'prompt',
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.translate.instant('COMMON_ASK_OK'),
                okButton: {
                    autofocus: false,
                    primary: true
                },
                cancelButton: {},
                close: () => {
                    this.service.setPowerControl(param).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        const powerContorlInfo = this.service.getPowerControlInfo(response.body);
                        this.powerContorlInfo.setPanelPowerButtonEnabled(powerContorlInfo.panelPowerButtonEnabled);
                        this.init();
                    }, (error) => {
                        const powerContorlInfo = this.service.getPowerControlInfo(error.error.data);
                        this.powerContorlInfo.setPanelPowerButtonEnabled(powerContorlInfo.panelPowerButtonEnabled);
                        this.init();
                    });
                },
                dismiss: () => {

                },
            });

        }
    };
    public wakeOnLan = {
        switchState: null,
        show: false,
        change: () => {
            if (!this.isPrivileges) {
                return;
            }
            const param = {
                WakeOnLANStatus: !this.wakeOnLan.switchState
            };
            this.powerControlModal = this.tiMessage.open({
                id: 'wekeOnLanDialog',
                type: 'prompt',
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.translate.instant('COMMON_ASK_OK'),
                okButton: {
                    autofocus: false,
                    primary: true
                },
                cancelButton: {},
                close: () => {
                    this.service.setPowerControl(param).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        const powerContorlInfo = this.service.getPowerControlInfo(response.body);
                        this.powerContorlInfo.setWakeOnLANStatus(powerContorlInfo.wakeOnLANStatus);
                        this.init();
                    }, (error) => {
                        const powerContorlInfo = this.service.getPowerControlInfo(error.error.data);
                        this.powerContorlInfo.setWakeOnLANStatus(powerContorlInfo.wakeOnLANStatus);
                        this.init();
                    });
                },
                dismiss: () => {

                },
            });

        }
    };
    // 漏液自动下电
    public leakage: any = {
        switchState: null,
        change: () => {
            if (!this.isPrivileges || this.isSystemLock) {
                return;
            }
            const param = {
                LeakStrategy: this.leakage.switchState ? 'ManualPowerOff' : 'AutoPowerOff'
            };
            const message = this.leakage.switchState ? 'COMMON_ASK_OK' : 'POWER_LEAKAGE_DIALOG_TIP';
            this.powerControlModal = this.tiMessage.open({
                id: 'leakageDialog',
                type: 'prompt',
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.translate.instant(message),
                okButton: {
                    autofocus: false,
                    primary: true
                },
                cancelButton: {},
                close: () => {
                    this.service.setPowerControl(param).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        const powerContorlInfo = this.service.getPowerControlInfo(response.body);
                        this.powerContorlInfo.setLeakStrategy(powerContorlInfo.leakStrategy);
                        this.init();
                    }, (error) => {
                        const powerContorlInfo = this.service.getPowerControlInfo(error.error.data);
                        this.powerContorlInfo.setLeakStrategy(powerContorlInfo.leakStrategy);
                        this.init();
                    });
                },
                dismiss: () => {

                },
            });

        }
    };


    public powerOnPolicy: PowerOnPolicy[] = [{
        value: 'TurnOn',
        label: this.translate.instant('POWER_KEEP_ON'),
        disabled: false
    }, {
        value: 'StayOff',
        label: this.translate.instant('POWER_KEEP_DOWN'),
        disabled: false
    }, {
        value: 'RestorePreviousState',
        label: this.translate.instant('POWER_KEEP_CONSISTT'),
        disabled: false
    }];
    public powerOnPolicySelected: string = '';
    public powerOnDelayedIconTip: string = this.translate.instant('COMMON_EDIT');
    public powerOnDelayed: PowerOnDelayed[] = [
        {
            value: 'DefaultDelay',
            label: this.translate.instant('POWER_DEFAULT_DELAY'),
            disabled: false,
            desc: this.translate.instant('POWER_DEFAULT_DELAY_INFO1'),
            editor: {
                show: false
            }
        },
        {
            value: 'HalfDelay',
            label: this.translate.instant('POWER_HALF_DELAY'),
            disabled: false,
            desc: this.translate.instant('POWER_HALF_DELAY_INFO'),
            editor: {
                show: true,
                value: '',
                editor: () => {
                    this.saveBtn = true;
                },
                cancel: () => {
                    this.monitorBtn();
                },
                confirm: () => {
                    this.monitorBtn();
                }
            }
        },
        {
            value: 'FixedDelay',
            label: this.translate.instant('POWER_FIXED_DELAY'),
            disabled: false,
            desc: this.translate.instant('POWER_FIXED_DELAY_INFO'),
            editor: {
                show: true,
                value: '',
                editor: () => {
                    this.saveBtn = true;
                },
                cancel: () => {
                    this.monitorBtn();
                },
                confirm: () => {
                    this.monitorBtn();
                }

            }
        },
        {
            value: 'RandomDelay',
            label: this.translate.instant('POWER_RANDOM_DELAY'),
            disabled: false,
            desc: this.translate.instant('POWER_RANDOM_DELAY_INFO'),
            editor: {
                show: true,
                value: '',
                editor: () => {
                    this.saveBtn = true;
                },
                cancel: () => {
                    this.monitorBtn();
                },
                confirm: () => {
                    this.monitorBtn();
                }
            }
        }
    ];
    public powerOffValidation: TiValidationConfig = {
        tip: '',
        tipPosition: 'top',
        errorMessage: {
            required: '',
            integer: this.translate.instant('VALID_INTEGER_INFO'),
            rangeValue: ''
        }
    };
    public powerOnDelayedValidation: TiValidationConfig = {
        tip: this.translate.instant('VALID_DELAY_POLICY_TIP'),
        tipPosition: 'top',
        errorMessage: {
            number: this.translate.instant('VALID_NUMBER_INFO'),
            required: this.translate.instant('VALID_NUMBER_INFO'),
            rangeValue: this.translate.instant('VALID_DELAY_POLICY_TIP'),
            decimalPlaces: this.translate.instant('VALID_OUT_DECIMAL_PPOINT')
        }
    };
    public powerOnDelayedValidationRules = [
        TiValidators.required,
        TiValidators.number,
        PowerControlValid.decimalPlaces(),
        TiValidators.rangeValue(0, 120)
    ];
    public powerOnDelayedSelected: string = '';
    public saveBtn: boolean = true;

    public powerOnPolicyChange() {
        this.monitorBtn();
    }

    public powerOnDelayedChange() {
        this.monitorBtn();
    }

    private monitorBtn() {
        const param = this.getParam();
        this.saveBtn = JSON.stringify(param) === '{}';
    }

    private getParam() {
        let param = null;
        param = {};
        if (this.powerOnPolicySelected !== this.powerContorlInfo.getPowerOnStrategy()) {
            param['PowerOnStrategy'] = this.powerOnPolicySelected;
        }

        if (this.powerOnDelayedSelected !== this.powerContorlInfo.getPowerRestoreDelayMode()) {
            if (!param['PowerRestoreDelay']) {
                param['PowerRestoreDelay'] = {};
            }
            param.PowerRestoreDelay['Mode'] = this.powerOnDelayedSelected;
        }

        const powerRestoreDelaySeconds = this.powerContorlInfo.getPowerRestoreDelaySeconds().toString();
        switch (this.powerOnDelayedSelected) {
            case 'HalfDelay':
                if (this.powerOnDelayed[1].editor.value !== powerRestoreDelaySeconds) {
                    if (!param['PowerRestoreDelay']) {
                        param['PowerRestoreDelay'] = {};
                    }
                    param.PowerRestoreDelay['Seconds'] = parseFloat(this.powerOnDelayed[1].editor.value);
                }
                break;
            case 'FixedDelay':
                if (this.powerOnDelayed[2].editor.value !== powerRestoreDelaySeconds) {
                    if (!param['PowerRestoreDelay']) {
                        param['PowerRestoreDelay'] = {};
                    }
                    param['PowerRestoreDelay'].Seconds = parseFloat(this.powerOnDelayed[2].editor.value);
                }
                break;
            case 'RandomDelay':
                if (this.powerOnDelayed[3].editor.value !== powerRestoreDelaySeconds) {
                    if (!param['PowerRestoreDelay']) {
                        param['PowerRestoreDelay'] = {};
                    }
                    param['PowerRestoreDelay'].Seconds = parseFloat(this.powerOnDelayed[3].editor.value);
                }
                break;
            default:
                break;
        }
        return param;
    }

    public powerContorlSave() {
        this.loading.state.next(true);
        this.saveBtn = true;
        const param = this.getParam();
        this.service.setPowerControl(param).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            const powerControlInfo = this.service.getPowerControlInfo(response.body);
            this.powerContorlInfo.setPowerOnStrategy(powerControlInfo.powerOnStrategy);
            this.powerContorlInfo.setPowerRestoreDelayMode(powerControlInfo.powerRestoreDelayMode);
            this.powerContorlInfo.setPowerRestoreDelaySeconds(powerControlInfo.powerRestoreDelaySeconds);
        }, (error) => {
            const powerControlInfo = this.service.getPowerControlInfo(error.error.data);
            this.powerContorlInfo.setPowerOnStrategy(powerControlInfo.powerOnStrategy);
            this.powerContorlInfo.setPowerRestoreDelayMode(powerControlInfo.powerRestoreDelayMode);
            this.powerContorlInfo.setPowerRestoreDelaySeconds(powerControlInfo.powerRestoreDelaySeconds);
        }, () => {
            this.init();
            this.loading.state.next(false);
        });
    }

    public virtualKeyClick(data) {
        if (!this.isPrivileges) {
            return;
        }
        let tips = '';
        const param = {
            OperateType: ''
        };
        switch (data) {
            case 'on':
                tips = this.translate.instant('COMMON_ASK_OK');
                param.OperateType = 'On';
                break;
            case 'off':
                tips = this.translate.instant('COMMON_ASK_OK');
                param.OperateType = 'GracefulShutdown';
                break;
            case 'forcedOff':
                tips = this.translate.instant('POWER_STRONG_DOWN_POWER_TIP');
                param.OperateType = 'ForceOff';
                break;
            case 'forcedRestart':
                tips = this.translate.instant('POWER_REST_START_TIP');
                param.OperateType = 'ForceRestart';
                break;
            case 'forcedCycle':
                tips = this.translate.instant('POWER_OFF_POWER_ON_TIP');
                param.OperateType = 'ForcePowerCycle';
                break;
            case 'nmi':
                tips = this.translate.instant('POWER_NMI_LABEL_TIP');
                param.OperateType = 'Nmi';
                break;
            default:
                break;
        }
        this.powerControlModal = this.tiMessage.open({
            id: 'powerOnOffDialog',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: tips,
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: {},
            close: () => {
                this.service.setPowerOnOff(param).subscribe((response) => {
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    clearInterval(this.interval);
                    this.initInterval();
                });
            },
            dismiss: () => {

            },
        });

    }

    public powerOffChange(data) {
        if (!this.isPrivileges) {
            return;
        }
        this.loading.state.next(true);
        const params = {
            PowerOffAfterTimeout: {
                Enabled: data ? 'On' : 'Off'
            }
        };
        this.service.setPowerControl(params).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.loading.state.next(false);
            const chassisType = this.powerContorlInfo.getChassisType();
            const powerContorlInfo = this.service.getPowerControlInfo(response.body);
            this.powerContorlInfo = new PowerControl(powerContorlInfo);
            this.powerContorlInfo.setChassisType(chassisType);
            this.powerOff.show = this.powerContorlInfo.getSafePowerOffTimeoutEnabled();
            this.init();
        }, (error) => {
            this.loading.state.next(false);
            const chassisType = this.powerContorlInfo.getChassisType();
            const powerContorlInfo = this.service.getPowerControlInfo(error.error.data);
            this.powerContorlInfo = new PowerControl(powerContorlInfo);
            this.powerContorlInfo.setChassisType(chassisType);
            this.powerOff.show = this.powerContorlInfo.getSafePowerOffTimeoutEnabled();
            this.init();
        });
    }

    initInterval() {
        this.interval = setInterval(() => {
            this.service.getPowerControl().subscribe((response) => {
                const powerContorlInfo = this.service.getPowerControlInfo(response.body);
                this.powerContorlInfo = new PowerControl(powerContorlInfo);
                const chassisType = this.service.getChassisTypeInfo(response.body);
                this.powerContorlInfo.setChassisType(chassisType);
                this.init();
            });
        }, 15000);

    }
    init() {
        this.leakageSupport = this.powerContorlInfo.leakDetectionSupport;
        this.leakage.switchState = this.powerContorlInfo.leakStrategy === 'AutoPowerOff';
        this.powerOff.show = this.powerContorlInfo.getSafePowerOffTimeoutEnabled();
        this.powerOff.value = this.powerContorlInfo.getSafePowerOffTimoutSeconds().toString();
        this.panelPower.switchState = this.powerContorlInfo.getPanelPowerButtonEnabled();
        this.powerOnPolicySelected = this.powerContorlInfo.getPowerOnStrategy();
        this.powerOnDelayedSelected = this.powerContorlInfo.getPowerRestoreDelayMode();
        this.powerOnDelayed[1].editor.value = this.powerContorlInfo.getPowerRestoreDelaySeconds().toString();
        this.powerOnDelayed[2].editor.value = this.powerContorlInfo.getPowerRestoreDelaySeconds().toString();
        this.powerOnDelayed[3].editor.value = this.powerContorlInfo.getPowerRestoreDelaySeconds().toString();
        const min = this.powerContorlInfo.getSafePowerOffMinTimeoutSeconds();
        const max = this.powerContorlInfo.getSafePowerOffMaxTimeoutSeconds();
        this.powerOff.validationRules = [
            TiValidators.required,
            TiValidators.integer,
            TiValidators.rangeValue(min, max)];
        const defaultTip = formatEntry(this.translate.instant('VALID_VALUE_POWER_OFF_RANGE_TIP'),
            [min, max, this.powerContorlInfo.powerOffDefaultSeconds]);
        const errorTip = formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [min, max]);
        this.powerOffValidation.tip = defaultTip;
        this.powerOffValidation.errorMessage.required = errorTip;
        this.powerOffValidation.errorMessage.rangeValue = errorTip;
        this.wakeOnLan.show = this.powerContorlInfo.getWakeOnLANStatus() !== null &&
            this.powerContorlInfo.getWakeOnLANStatus() !== undefined;
        this.wakeOnLan.switchState = this.powerContorlInfo.getWakeOnLANStatus();
        if (this.powerContorlInfo.getChassisType() === 'Blade') {
            this.powerOnDelayed[0].desc = this.translate.instant('POWER_DEFAULT_DELAY_INFO2');
        } else {
            this.powerOnDelayed[0].desc = this.translate.instant('POWER_DEFAULT_DELAY_INFO1');
        }
        this.powerOnPolicy[0].disabled = !this.isPrivileges;
        this.powerOnPolicy[1].disabled = !this.isPrivileges;
        this.powerOnPolicy[2].disabled = !this.isPrivileges;
        this.powerOnDelayed[0].disabled = !this.isPrivileges;
        this.powerOnDelayed[1].disabled = !this.isPrivileges;
        this.powerOnDelayed[2].disabled = !this.isPrivileges;
        this.powerOnDelayed[3].disabled = !this.isPrivileges;
    }

    ngOnInit(): void {
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.powerControlModal) {
                this.powerControlModal.dismiss();
            }
        });
        this.init();
    }

    ngOnDestroy(): void {
        clearInterval(this.interval);
    }

}
