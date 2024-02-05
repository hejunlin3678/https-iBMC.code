import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder } from '@angular/forms';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { VmmService } from './services';
import { AlertMessageService, LoadingService, UserInfoService, ErrortipService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-vmm',
    templateUrl: './vmm.component.html',
    styleUrls: ['../service.component.scss', './vmm.component.scss']
})
export class VmmComponent implements OnInit {
    constructor(
        private translate: TranslateService,
        private vmmService: VmmService,
        private fb: UntypedFormBuilder,
        private alert: AlertMessageService,
        private user: UserInfoService,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService
    ) { }

    public cancelLation: boolean = false;
    public vmmEncryShow: boolean = false;
    // 权限判断
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isOemVmmUser = this.user.hasPrivileges(['OemVmm']);
    public isSystemLock = this.user.systemLockDownEnabled;

    public vmmMaxSession = {
        value: 1
    };

    public vmmActiveSession = {
        value: 0
    };

    public vmmForm = this.fb.group({
        enableSwitchState: [{ value: null, disabled: false }, []],
        portVmmControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
        ],
        encryptionSwitchState: [{ value: null, disabled: false }, []],
    });

    // 初始化属性
    public vmmData: object = {};
    public virtualData: object = {};
    public vmmParams: object = {};
    public buttonDisabled: boolean = true;
    public queryParams: object = {};

    // 端口校验
    public validation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES')
        }
    };
    public showAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('SERVICE_CLOSE_ENCRYPTION')
    };

    // 初始化数据查询
    public initData = (vmmData) => {
        // vmm使能
        this.vmmParams['enabbleState'] = vmmData.Enabled;
        // 端口
        this.vmmParams['portValue'] = vmmData.Port;
        // 通信加密
        this.vmmParams['encryState'] = vmmData.EncryptionEnabled;
        this.vmmEncryShow = vmmData.EncryptionConfigurable ? true : false;
        this.vmmForm.patchValue({
            enableSwitchState: this.vmmParams['enabbleState'],
            portVmmControl: this.vmmParams['portValue'],
            encryptionSwitchState: this.vmmParams['encryState'],
        });
        this.showAlert['openAlert'] = !vmmData?.EncryptionEnabled;
        // 最大会话
        this.vmmMaxSession.value = vmmData.MaximumNumberOfSessions;
        // 活跃会话
        this.vmmActiveSession.value = vmmData.NumberOfActiveSessions;
        if (this.vmmActiveSession.value === 1) {
            this.cancelLation = true;
        } else {
            this.cancelLation = false;
        }
        this.protocolEnabledEN();
    }

    public vmmGetInit = () => {
        this.vmmService.getVmm().subscribe((response) => {
            this.vmmData = response['body'];
            this.initData(this.vmmData);
        }, () => {
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
            this.vmnAuthority();
        }
    }
    // 权限
    public vmnAuthority() {
        // 权限控制具有安全配置权限可以设置vmm使能、端口
        if (this.isOemSecurityMgmtUser && !this.isSystemLock) {
            this.vmmForm.root.get('enableSwitchState').enable();
            this.vmmForm.root.get('portVmmControl').enable();
        } else {
            this.vmmForm.root.get('enableSwitchState').disable();
            this.vmmForm.root.get('portVmmControl').disable();
        }
        // 具有远程媒体权限可以设置通信加密、注销会话
        if (this.isOemVmmUser && !this.isSystemLock) {
            this.vmmForm.root.get('encryptionSwitchState').enable();
        } else {
            this.vmmForm.root.get('encryptionSwitchState').disable();
        }
    }
    // 页面初始化
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.vmnAuthority();
        this.vmmGetInit();
    }

    // 活跃会话注销
    public logoutFN(): void {
        this.vmmService.getVmmActiveSession().subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'SERVICE_DELETE_SESSION_SUCCESS' });
            this.vmmGetInit();
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        });
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        if (!this.vmmForm.valid) {
            this.buttonDisabled = true;
            return;
        }
        this.queryParams = {};
        if (this.vmmForm.root.get('enableSwitchState').value !== this.vmmParams['enabbleState']) {
            this.queryParams['Enabled'] = this.vmmForm.root.get('enableSwitchState').value;
        }
        if (parseInt(this.vmmForm.root.get('portVmmControl').value, 10) !== this.vmmParams['portValue']) {
            this.queryParams['Port'] = parseInt(this.vmmForm.root.get('portVmmControl').value, 10);
        }
        if (this.vmmEncryShow && this.vmmForm.root.get('encryptionSwitchState').value !== this.vmmParams['encryState']) {
            this.queryParams['EncryptionEnabled'] = this.vmmForm.root.get('encryptionSwitchState').value;
        }

        if (JSON.stringify(this.queryParams) !== '{}') {
            this.buttonDisabled = false;
        } else {
            this.buttonDisabled = true;
        }
    }

    // change事件
    public vmmEnabledChangeDisabled = () => {
        this.protocolEnabledEN();
    }

    public vmmChangeDisabled = () => {
        this.protocolEnabledEN();
    }

    // 通信加密beforeChange事件
    public vmmEncryptionChange = () => {
        // 查询kvm通信加密
        this.vmmForm.patchValue({
            encryptionSwitchState: !this.vmmForm.root.get('encryptionSwitchState').value,
        });
        this.protocolEnabledEN();
        this.vmmService.getKvm().subscribe((response) => {
            const kvmEncryptionEnabled = response['body'].EncryptionEnabled;
            if (!kvmEncryptionEnabled && !this.vmmParams['encryState']) {
                this.alert.eventEmit.emit({ type: 'warn', label: 'SERVICE_VMM_ERROR_MSG' });
                this.vmmForm.patchValue({
                    encryptionSwitchState: false,
                });
                this.protocolEnabledEN();
            }
        });
        this.showAlert['openAlert'] = !this.vmmForm.root.get('encryptionSwitchState')?.value;
    }

    // 恢复默认值
    public restFN(): void {
        this.vmmForm.patchValue({
            portVmmControl: 8208,
        });
        this.protocolEnabledEN();
    }

    // 保存
    public saveButton(): void {
        this.protocolEnabledEN();
        this.buttonDisabled = true;
        this.vmmService.setVmmService(this.queryParams).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.vmmData = response['body'];
            this.initData(this.vmmData);
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            this.vmmData = error.error.data;
            this.initData(this.vmmData);
        });
    }
}
