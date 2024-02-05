import { Component, OnInit } from '@angular/core';
import { UntypedFormBuilder } from '@angular/forms';
import { TiValidators, TiValidationConfig, TiModalRef, TiMessageService } from '@cloud/tiny3';
import { PartService } from './service';
import { CommonService } from 'src/app/common-app/service/common.service';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { LoadingService } from 'src/app/common-app/service/loading.service';

@Component({
    selector: 'app-hard-partity',
    templateUrl: './hard-partity.component.html',
    styleUrls: ['./hard-partity.component.scss']
})
export class HardPartityComponent implements OnInit {
    public vgaUsbDidShow: boolean = false;
    constructor(
        private fb: UntypedFormBuilder,
        private service: PartService,
        private commonService: CommonService,
        private translate: TranslateService,
        private user: UserInfoService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private loading: LoadingService
    ) { }
    public i18n = {
        any255: this.translate.instant('HARD_ANY_CHARACTER255'),
        any20: this.translate.instant('HARD_ANY_CHARACTER20'),
        modelTip: this.translate.instant('HARD_TIP'),
        yes: this.translate.instant('COMMON_OK'),
        no: this.translate.instant('COMMON_CANCEL'),
        modelSuggestion: this.translate.instant('HARD_MODEL_CHANGE_TIP'),
        suggestion: this.translate.instant('COMMON_INFORMATION'),
        confirm: this.translate.instant('COMMON_CONFIRM')
    };
    public partForm = this.fb.group({
        systemType: [null, []],
        userName: [null, [TiValidators.required, TiValidators.rangeSize(1, 255)]],
        pwd: [null, [TiValidators.required, TiValidators.rangeSize(1, 20)]],
        model: [null, []]
    });
    public userNameValidation: TiValidationConfig = {
        tipPosition: 'right',
        errorMessage: {
            rangeSize: this.i18n['any255']
        }
    };
    public pwdValidation: TiValidationConfig = {
        tipPosition: 'right',
        errorMessage: {
            rangeSize: this.i18n['any20']
        }
    };
    public system = [{
        id: 'SingleSystem',
        key: 'HARD_PARTING_SINGLE_SYSTEM',
        disable: false
    }, {
        id: 'DualSystem',
        key: 'HARD_PARTING_DOUBLE_SYSTEM',
        disable: false
    }];
    public modelStatus = [
        {
            key: 'COMMON_ENABLE',
            id: true,
            disable: false,
            uniq: 'able0'
        },
        {
            key: 'COMMON_DISABLE',
            id: false,
            disable: false,
            uniq: 'able1'
        }
    ];
    public hasPrivilege: boolean;
    public isSystemLock = this.user.systemLockDownEnabled;
    public btnSaveStatus: boolean = true;
    public defaultData;
    ngOnInit(): void {
        this.init(this.systemLock);
    }
    public init(callback) {
        this.loading.state.next(true);
        this.service.getSettings().subscribe((res: IPartity) => {
            this.vgaUsbDidShow = res.vgaUSBDVDEnabled;
            this.defaultData = res.fusionPartition;
            this.partForm.patchValue({
                systemType: res.fusionPartition,
                model: res.vgaUSBDVDEnabled
            });
            this.hasPrivilege = this.user.hasPrivileges(['ConfigureComponents']);
            if (!this.hasPrivilege) {
                this.partForm.root.get('userName').disable();
                this.partForm.root.get('pwd').disable();
                this.system[0].disable = true;
                this.system[1].disable = true;
                this.modelStatus[0].disable = true;
                this.modelStatus[1].disable = true;
            } else {
                this.partForm.root.get('userName').enable();
                this.partForm.root.get('pwd').enable();
                this.system[0].disable = false;
                this.system[1].disable = false;
                this.modelStatus[0].disable = false;
                this.modelStatus[1].disable = false;
            }
            if (res.fusionPartition == null && res.vgaUSBDVDEnabled !== null) {
                this.partForm.root.get('userName').disable();
                this.partForm.root.get('pwd').disable();
                this.system[0].disable = true;
                this.system[1].disable = true;
            }
            callback();
            this.loading.state.next(false);
        }, () => {
            callback();
            this.loading.state.next(false);
        });
    }
    public systemTypeChange() {
        if (this.partForm.get('systemType').value === this.defaultData) {
            this.btnSaveStatus = true;
        } else {
            this.btnSaveStatus = false;
        }
    }
    public userNameChange() {
        if (this.partForm.get('userName').value === ' ') {
            this.partForm.patchValue({
                userName: this.partForm.get('userName').value.slice(1, this.partForm.get('userName').value.length)
            });
        }
    }
    public systemLock = () => {
        if (this.isSystemLock) {
            this.partForm.root.get('userName').disable();
            this.partForm.root.get('pwd').disable();
            this.system[0].disable = true;
            this.system[1].disable = true;
            this.modelStatus[0].disable = true;
            this.modelStatus[1].disable = true;
        }
    }
    public pwdChange() {
        if (this.partForm.get('pwd').value === ' ') {
            this.partForm.patchValue({
                pwd: this.partForm.get('pwd').value.slice(1, this.partForm.get('pwd').value.length)
            });
        }
    }
    // 模式变换
    public modelChange() {
        const model = this.partForm.get('model').value;
        const params = {
            'VGAUSBDVDEnabled': !model
        };
        const self = this;
        const instance = this.tiMessage.open({
            okButton: {
                show: true,
                text: this.i18n.yes,
                autofocus: false,
                click: () => {
                    this.service.setSetting(params).subscribe((data) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    }, (error) => {
                        self.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                        self.partForm.patchValue({
                            model: !self.partForm.get('model').value
                        });

                    });
                    instance.close();
                }
            },
            cancelButton: {
                show: true,
                text: this.i18n.no,
                click: () => {
                    this.partForm.patchValue({
                        model: !this.partForm.get('model').value
                    });
                    instance.dismiss();
                }
            },
            title: this.i18n.suggestion,
            content: this.i18n.modelSuggestion,
        });
    }

    public save() {
        const isEmpty = this.partForm.get('userName').value && this.partForm.get('pwd').value;
        if (!isEmpty) {
            return;
        }
        const params = {
            'FusionPartition': this.partForm.get('systemType').value,
            'RemoteNodeUserName': this.partForm.get('userName').value,
            'RemoteNodePassword': this.partForm.get('pwd').value
        };
        const instance = this.tiMessage.open({
            content: this.i18n.modelTip,
            okButton: {
                text: this.i18n.yes,
                autofocus: false,
                click: () => {
                    this.loading.state.next(true);
                    this.service.setSetting(params).subscribe((data) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        this.init(this.systemLock);
                        this.loading.state.next(false);
                    }, (error) => {
                        this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                        this.loading.state.next(false);
                    });
                    instance.close();
                }
            },
            cancelButton: {
                text: this.i18n.no,
                click: () => {
                    instance.dismiss();
                }
            },
            title: this.i18n.confirm,
            close(messageRef: TiModalRef): void {
            },
            dismiss(messageRef: TiModalRef): void {
            }
        });
    }
}
export interface IPartity {
    fusionPartition: string;
    vgaUSBDVDEnabled: boolean;
}
