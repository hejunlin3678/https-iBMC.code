import { Component, OnInit, ElementRef } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder, ValidationErrors } from '@angular/forms';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { KvmService } from './services';
import {
    uskeyboardTable, JS_KEY_A, JS_KEY_Z, JS_KEY_0, JS_KEY_9, JS_KEY_SEMI, JS_KEY_QUOTO, JS_KEY_BR1, JS_KEY_QUOT,
    JS_KEY_FF_SEMI, JS_KEY_FF_PLUS, JS_KEY_FF_MIN, JS_KEY_0P, JS_KEY_DIVP, JS_KEY_OR_1, errorTip
} from 'src/app/common-app/utils';
import { AlertMessageService } from 'src/app/common-app/service';
import { AutoKVMValidators } from './model';
import { Router } from '@angular/router';
import { LoadingService } from 'src/app/common-app/service';
import { UserInfoService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';
import { ErrortipService } from 'src/app/common-app/service';

@Component({
    selector: 'app-kvm',
    templateUrl: './kvm.component.html',
    styleUrls: ['../service.component.scss', './kvm.component.scss']
})
export class KvmComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private kvmService: KvmService,
        private elRef: ElementRef,
        private alert: AlertMessageService,
        private router: Router,
        private user: UserInfoService,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService,
        private fb: UntypedFormBuilder,
    ) { }

    // 权限判断
    public isConfigureUser = this.user.hasPrivileges(['ConfigureUsers']);
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isOemKvmUser = this.user.hasPrivileges(['OemKvm']);
    public isSystemLock = this.user.systemLockDownEnabled;

    // 页面数据渲染
    public disabledKvm: boolean = false;
    public autoLockListShow: boolean = false;
    public customShow: boolean = false;
    public openAlert: boolean = false;
    public kvmEnable: object = {
        value: ''
    };
    public kvmEncryption: object = {
        value: ''
    };

    public localKvm: object = {
        value: ''
    };

    public persistentUSBConnectionEnabled = {
        value: ''
    };

    public encryptionShow: boolean = false;
    public autoOSLockEnabled: object = {
        value: ''
    };
    public autoLockType: object = {
        value: ''
    };

    public kvmForm = this.fb.group({
        enableSwitchState: [{ value: null, disabled: false }, []],
        portKVMControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
        ],
        timeoutKVMControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(0, 480)]
        ],
        kvmEncryptionSwitch: [{ value: false, disabled: false }, []],
        localKvmSwitch: [{ value: false, disabled: false }, []],
        usbConnectionSwitch: [{ value: false, disabled: false }, []],
        autoOSLockSwitch: [{ value: false, disabled: false }, []],
        selectedAutoOSLockType: [{ value: null, disabled: false }, []],
        lockKeyValidOptions1: [
            { value: '', disabled: false },
            [AutoKVMValidators.isNotEmpty('lockKeyValidOptions2', 'lockKeyValidOptions3', 'lockKeyValidOptions4')]
        ],
        lockKeyValidOptions2: [
            { value: '', disabled: false },
            [AutoKVMValidators.isNotEmpty('lockKeyValidOptions1', 'lockKeyValidOptions3', 'lockKeyValidOptions4')]
        ],
        lockKeyValidOptions3: [
            { value: '', disabled: false },
            [AutoKVMValidators.isNotEmpty('lockKeyValidOptions1', 'lockKeyValidOptions2', 'lockKeyValidOptions4')]
        ],
        lockKeyValidOptions4: [
            { value: '', disabled: false },
            [AutoKVMValidators.isNotEmpty('lockKeyValidOptions1', 'lockKeyValidOptions2', 'lockKeyValidOptions3')]
        ]
    });

    public kvmaxSession = {
        value: '0'
    };
    public kvmActiveSession = {
        value: 0
    };

    public autoOSLockTypeList = [
        {
            key: this.translate.instant('SERVICE_WBS_CRE_CUSTOM'),
            id: 'Custom'
        }, {
            key: this.translate.instant('SERVICE_WBS_CRE_WINDOWS'),
            id: 'Windows'
        }
    ];

    // 保存
    public kvmDisabled: boolean = true;
    public kvmData: object = {};
    public kvmVirtual: object = {};
    public systemData: object = {};

    // 初始化属性
    public KEY_LENGTH = 4;
    public CUSTOM_LOCK_TYPE = 'Custom';
    public kvmParams: object = {};
    public kvmDataAutoOSLockKey: object = {};
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
    // 超时时长
    public timeoutValidation: TiValidationConfig = {
        tip: this.translate.instant('SERVICE_VNC_TIMEOUT_TIP'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('SERVICE_VNC_TIMEOUT_BLUR'),
            number: this.translate.instant('SERVICE_VNC_TIMEOUT_BLUR'),
            integer: this.translate.instant('SERVICE_VNC_TIMEOUT_BLUR'),
            rangeValue: this.translate.instant('SERVICE_VNC_TIMEOUT_BLUR')
        }
    };
    // 自定义快捷键校验
    public keyValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            isNotEmpty: this.translate.instant('SERVICE_SHORTCUT_IS_NULL'),
        }
    };

    public setKeyInputsState = (state) => {
        if (!state) {
            for (let i = 1; i < this.KEY_LENGTH + 1; i++) {
                if (!this.elRef.nativeElement.querySelector('#lockKeyId' + i).hasClass('ti-validate-error')) {
                    this.elRef.nativeElement.querySelector('#lockKeyId' + i).addClass('ti-validate-error');
                }
            }
        } else {
            for (let k = 1; k < this.KEY_LENGTH + 1; k++) {
                this.elRef.nativeElement.querySelector('#lockKeyId' + k).removeClass('ti-validate-error');
            }
        }
    }

    // 初始化数据查询
    public initData = (kvmData) => {
        // kvm使能
        this.kvmParams['kvmEnabble'] = kvmData.Enabled;
        // 端口
        this.kvmParams['kvmPortValue'] = kvmData.Port;
        // 超时时长
        this.kvmParams['kvmTimeoutDuration'] = kvmData.SessionTimeoutMinutes;
        // 通信加密
        this.kvmParams['communicationEncryption'] = kvmData.EncryptionEnabled;
        this.kvmForm.patchValue({
            kvmEncryptionSwitch: this.kvmParams['communicationEncryption'],
        });
        if (kvmData.EncryptionConfigurable === true) {
            this.encryptionShow = true;
        } else {
            this.encryptionShow = false;
        }
        // 虚拟键鼠持续连接
        this.kvmParams['connectionUSB'] = kvmData.PersistentUSBConnectionEnabled;
        // 系统自动锁定
        this.kvmParams['autoOSLock'] = kvmData.AutoOSLockEnabled;
        // 系统自动锁定方式
        this.kvmParams['autoOSLockType'] = kvmData.AutoOSLockType;

        this.kvmForm.patchValue({
            enableSwitchState: this.kvmParams['kvmEnabble'],
            portKVMControl: this.kvmParams['kvmPortValue'],
            timeoutKVMControl: this.kvmParams['kvmTimeoutDuration'],
            usbConnectionSwitch: this.kvmParams['connectionUSB'],
            autoOSLockSwitch: this.kvmParams['autoOSLock'],
            selectedAutoOSLockType: this.kvmParams['autoOSLockType'],
        });
        // 系统锁定配置快捷键
        if (kvmData.AutoOSLockKey && kvmData.AutoOSLockKey !== '') {
            this.kvmDataAutoOSLockKey = kvmData.AutoOSLockKey;
            for (let i = 1; i <= kvmData.AutoOSLockKey.length; i++) {
                this.kvmForm.patchValue({
                    ['lockKeyValidOptions' + i]: kvmData.AutoOSLockKey[i - 1],
                });
            }
        } else {
            for (let k = 1; k <= this.KEY_LENGTH; k++) {
                this.kvmForm.patchValue({
                    ['lockKeyValidOptions' + k]: '',
                });
            }
            this.kvmDataAutoOSLockKey = ['', '', '', ''];
        }
        this.attachListener();
        // 最大会话
        this.kvmParams['maxSession'] = kvmData.MaximumNumberOfSessions;
        this.kvmaxSession['value'] = this.kvmParams['maxSession'];
        // 活跃会话
        this.kvmParams['activeSession'] = kvmData.NumberOfActiveSessions;
        this.kvmActiveSession['value'] = this.kvmParams['activeSession'];
        // 本地kvm
        this.kvmParams['kvmLocal'] = kvmData.LocalKvmEnabled;
        this.kvmForm.patchValue({
            localKvmSwitch: this.kvmParams['kvmLocal'],
        });
        this.protocolEnabledEN();
    }

    public kvmGetInit = () => {
        this.kvmService.getKvm().subscribe((response) => {
            this.kvmData = response['body'];
            this.initData(this.kvmData);
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
            this.kvmAuthority();
        }
    }
    // 权限
    public kvmAuthority() {
        // 权限控制具有安全配置权限可以设置KVM使能、端口
        if (this.isOemSecurityMgmtUser && !this.isSystemLock) {
            this.kvmForm.root.get('enableSwitchState').enable();
            this.kvmForm.root.get('portKVMControl').enable();
        } else {
            this.kvmForm.root.get('enableSwitchState').disable();
            this.kvmForm.root.get('portKVMControl').disable();
        }
        // 具有远程能控制权限可以设置超时时长、通信加密、本地KVM、虚拟键鼠持续连接、系统锁定配置
        if (this.isOemKvmUser && !this.isSystemLock) {
            this.kvmForm.root.get('timeoutKVMControl').enable();
            this.kvmForm.root.get('kvmEncryptionSwitch').enable();
            this.kvmForm.root.get('localKvmSwitch').enable();
            this.kvmForm.root.get('usbConnectionSwitch').enable();
            this.kvmForm.root.get('autoOSLockSwitch').enable();
            this.kvmForm.root.get('selectedAutoOSLockType').enable();
        } else {
            this.kvmForm.root.get('timeoutKVMControl').disable();
            this.kvmForm.root.get('kvmEncryptionSwitch').disable();
            this.kvmForm.root.get('localKvmSwitch').disable();
            this.kvmForm.root.get('usbConnectionSwitch').disable();
            this.kvmForm.root.get('autoOSLockSwitch').disable();
            this.kvmForm.root.get('selectedAutoOSLockType').disable();
        }
        // 自定义快捷键权限
        if (this.autoLockListShow && this.customShow && this.isOemKvmUser && !this.isSystemLock) {
            this.kvmForm.root.get('lockKeyValidOptions1').enable();
            this.kvmForm.root.get('lockKeyValidOptions2').enable();
            this.kvmForm.root.get('lockKeyValidOptions3').enable();
            this.kvmForm.root.get('lockKeyValidOptions4').enable();
        } else {
            this.kvmForm.root.get('lockKeyValidOptions1').disable();
            this.kvmForm.root.get('lockKeyValidOptions2').disable();
            this.kvmForm.root.get('lockKeyValidOptions3').disable();
            this.kvmForm.root.get('lockKeyValidOptions4').disable();
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.kvmAuthority();
        this.kvmGetInit();
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        TiValidators.check(this.kvmForm);
        if (this.kvmForm.root.get('autoOSLockSwitch').value === true) {
            this.autoLockListShow = true;
        }
        if (this.kvmForm.root.get('autoOSLockSwitch').value === true
            && this.kvmForm.root.get('selectedAutoOSLockType').value === this.CUSTOM_LOCK_TYPE) {
            this.customShow = true;
        } else {
            this.customShow = false;
        }
        if (this.autoLockListShow && this.customShow && this.isOemKvmUser && !this.isSystemLock) {
            this.kvmForm.root.get('lockKeyValidOptions1').enable();
            this.kvmForm.root.get('lockKeyValidOptions2').enable();
            this.kvmForm.root.get('lockKeyValidOptions3').enable();
            this.kvmForm.root.get('lockKeyValidOptions4').enable();
        } else {
            this.kvmForm.root.get('lockKeyValidOptions1').disable();
            this.kvmForm.root.get('lockKeyValidOptions2').disable();
            this.kvmForm.root.get('lockKeyValidOptions3').disable();
            this.kvmForm.root.get('lockKeyValidOptions4').disable();
        }
        if (!this.kvmForm.valid) {
            this.kvmDisabled = true;
            return;
        }
        this.queryParams = {};
        if (this.kvmForm.root.get('enableSwitchState').value !== this.kvmParams['kvmEnabble']) {
            this.queryParams['Enabled'] = this.kvmForm.root.get('enableSwitchState').value;
        }
        if (parseInt(this.kvmForm.root.get('portKVMControl').value, 10) !== this.kvmParams['kvmPortValue']) {
            this.queryParams['Port'] = parseInt(this.kvmForm.root.get('portKVMControl').value, 10);
        }
        if (parseInt(this.kvmForm.root.get('timeoutKVMControl').value, 10) !== this.kvmParams['kvmTimeoutDuration']) {
            this.queryParams['SessionTimeoutMinutes'] = parseInt(this.kvmForm.root.get('timeoutKVMControl').value, 10);
        }
        if (this.encryptionShow && this.kvmForm.root.get('kvmEncryptionSwitch').value !== this.kvmParams['communicationEncryption']) {
            this.queryParams['EncryptionEnabled'] = this.kvmForm.root.get('kvmEncryptionSwitch').value;
        }
        if (this.kvmParams['kvmLocal'] !== undefined && this.kvmForm.root.get('localKvmSwitch').value !== this.kvmParams['kvmLocal']) {
            this.queryParams['LocalKvmEnabled'] = this.kvmForm.root.get('localKvmSwitch').value;
        }
        if (this.kvmForm.root.get('usbConnectionSwitch').value !== this.kvmParams['connectionUSB']) {
            this.queryParams['PersistentUSBConnectionEnabled'] = this.kvmForm.root.get('usbConnectionSwitch').value;
        }
        if (this.kvmForm.root.get('autoOSLockSwitch').value !== this.kvmParams['autoOSLock']) {
            this.queryParams['AutoOSLockEnabled'] = this.kvmForm.root.get('autoOSLockSwitch').value;
        }
        if (this.kvmForm.root.get('autoOSLockSwitch').value === true) {
            if (this.kvmForm.root.get('selectedAutoOSLockType').value !== this.kvmParams['autoOSLockType']) {
                this.queryParams['AutoOSLockType'] = this.kvmForm.root.get('selectedAutoOSLockType').value;
            }
            const keyArray = [
                this.kvmForm.root.get('lockKeyValidOptions1').value,
                this.kvmForm.root.get('lockKeyValidOptions2').value,
                this.kvmForm.root.get('lockKeyValidOptions3').value,
                this.kvmForm.root.get('lockKeyValidOptions4').value
            ];
            if (
                this.kvmForm.root.get('selectedAutoOSLockType').value === this.CUSTOM_LOCK_TYPE
                && this.kvmDataAutoOSLockKey.toString() !== keyArray.toString()
            ) {
                this.queryParams['AutoOSLockKey'] = keyArray;
            }
        } else {
            this.autoLockListShow = false;
        }
        if (JSON.stringify(this.queryParams) !== '{}') {
            this.kvmDisabled = false;
        } else {
            this.kvmDisabled = true;
        }
    }

    // change事件
    public kvmEnabledChange = () => {
        this.protocolEnabledEN();
    }
    public kvmPortChange = () => {
        this.protocolEnabledEN();
    }
    public kvmTimeoutChange = () => {
        this.protocolEnabledEN();
    }

    // 通信加密
    public kvmEncryptionChange = () => {
        // 查询vmm通信加密
        this.kvmForm.patchValue({
            kvmEncryptionSwitch: !this.kvmForm.root.get('kvmEncryptionSwitch').value,
        });
        this.protocolEnabledEN();
        this.kvmService.getVmm().subscribe((res) => {
            const vmmEncryptionEnabled = res['body'].EncryptionEnabled;
            if (vmmEncryptionEnabled && this.kvmParams['communicationEncryption']) {
                this.alert.eventEmit.emit({ type: 'warn', label: 'SERVICE_KVM_ENCRYPTION_TIP' });
                this.kvmForm.patchValue({
                    kvmEncryptionSwitch: true,
                });
                this.protocolEnabledEN();
            }
        });
    }
    public kvmlocalChange = () => {
        this.protocolEnabledEN();
    }
    public kvmConnectionChange = () => {
        this.protocolEnabledEN();
    }
    public autoOSLockChange = () => {
        this.attachListener();
        this.protocolEnabledEN();
    }
    public sessionAutoOSTypeChange = () => {
        this.protocolEnabledEN();
        this.attachListener();
    }
    // 恢复默认值
    public restFN(): void {
        this.kvmForm.patchValue({
            portKVMControl: 2198,
        });
        this.protocolEnabledEN();
    }

    // 点击跳转活跃会话页面
    public goToUser(): void {
        if (this.isConfigureUser && this.kvmActiveSession['value'] !== 0) {
            this.router.navigate(['/navigate/user/online-user']);
        }
    }

    public changeDisabled(): void {
        this.kvmDisabled = true;
    }

    // 保存
    public kvmSaveButton(): void {
        this.kvmDisabled = true;
        this.protocolEnabledEN();
        this.kvmService.setKvmService(this.queryParams).subscribe((response) => {
            if (response['body'].error) {
                const msgId = getMessageId(response['body'])[0].errorId || 'COMMON_FAILED';
                this.alert.eventEmit.emit(errorTip(msgId, this.translate));
            } else {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            }
            this.kvmData = response['body'].error ? response['body'].data : response['body'];
            this.initData(this.kvmData);
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({type: 'error', label: errorMessage});
            this.kvmData = error.error.data;
            this.initData(this.kvmData);
        });
    }

    public autoOSLockKeyFN(): void {
        // 触发自定义快捷键校验
        this.kvmForm.get('lockKeyValidOptions1').updateValueAndValidity();
        this.kvmForm.get('lockKeyValidOptions2').updateValueAndValidity();
        this.kvmForm.get('lockKeyValidOptions3').updateValueAndValidity();
        this.kvmForm.get('lockKeyValidOptions4').updateValueAndValidity();
        this.protocolEnabledEN();
     }
    // 清空快捷键
    public clearKeyFN(): void {
        this.clearKeyInput();
        // 触发自定义快捷键校验
        this.kvmForm.get('lockKeyValidOptions1').updateValueAndValidity();
        this.kvmForm.get('lockKeyValidOptions2').updateValueAndValidity();
        this.kvmForm.get('lockKeyValidOptions3').updateValueAndValidity();
        this.kvmForm.get('lockKeyValidOptions4').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 自定义快捷键
    public clearKeyInput(): void {
        for (let i = 1; i < this.KEY_LENGTH + 1; i++) {
            if (this.elRef.nativeElement.querySelector('#lockKeyId' + i)) {
                this.elRef.nativeElement.querySelector('#lockKeyId' + i).value = '';
            }
            this.kvmForm.patchValue({
                ['lockKeyValidOptions' + i]: '',
            });
            this.elRef.nativeElement.querySelector('#lockKeyId' + i).setAttribute('key', '');
            this.elRef.nativeElement.querySelector('#lockKeyId' + i).setAttribute('keyCode', '');
        }
    }
    // 绑定keydown事件
    public attachListener(): void {
        setTimeout(() => {
            for (let i = 1; i < this.KEY_LENGTH + 1; i++) {
                if (!document.getElementById('lockKeyId' + i)) {
                    break;
                }
                document.getElementById('lockKeyId' + i).removeEventListener('keydown', (e) => { });
                document.getElementById('lockKeyId' + i).removeEventListener('keyup', (e) => { });
                document.getElementById('lockKeyId' + i).addEventListener('keydown', (e) => {
                    this.setDefineKey(e);
                    const keysCode = e.key;
                    if (keysCode !== 'F12') {
                        if (e && e.preventDefault) {
                            e.preventDefault();
                        } else {
                            // tslint:disable-next-line: deprecation
                            window.event.returnValue = false;
                        }
                    }
                });
                document.getElementById('lockKeyId' + i).addEventListener('keyup', (e) => {
                    const keysCode = e.key;
                    if (keysCode !== 'F12') {
                        if (e && e.preventDefault) {
                            e.preventDefault();
                        } else {
                            // tslint:disable-next-line: deprecation
                            window.event.returnValue = false;
                        }
                    }
                });
            }
        }, 100);
    }
    public setDefineKey(e): void {
        const target = e.srcElement || e.originalTarget;
        const eltId = target.id;
        let keysym;
        const keysLocation = e.location || e.keyLocation;
        // 意大利键盘模式下，按下右边Alt键，组合键输入框内应显示为"AltGraph"
        if (typeof e.key !== 'undefined' && e.key !== 'MozPrintableKey' && e.key !== 'Alt' && keysLocation !== 2) {
            keysym = e.key;
        } else if ((e.key !== 'MozPrintableKey' && e.key === 'Alt' && keysLocation === 2) || e.key === 'AltGraph') {
            keysym = 'AltGraph';
        } else {
            keysym = this.getKeysym(e);
        }
        if (eltId === 'lockKeyId1' || eltId === 'lockKeyId2' || eltId === 'lockKeyId3' || eltId === 'lockKeyId4') {
            this.elRef.nativeElement.querySelector(`#${eltId}`).value = keysym;
            this.kvmForm.patchValue({
                ['lockKeyValidOptions' + eltId[eltId.length - 1]]: keysym,
            });
            this.elRef.nativeElement.querySelector(`#${eltId}`).setAttribute('key', keysym);
            this.elRef.nativeElement.querySelector(`#${eltId}`).setAttribute('keyCode', e.keyCode || e.which || e.charCode);
        }
    }
    public getKeysym(e): string | number {
        const keysCode = e.keyCode || e.which || e.charCode;
        for (const key of uskeyboardTable) {
            if (key[0] === keysCode) {
                if (
                    (keysCode >= JS_KEY_A && keysCode <= JS_KEY_Z)
                    || (keysCode >= JS_KEY_0 && keysCode <= JS_KEY_9)
                    || (keysCode >= JS_KEY_SEMI && keysCode <= JS_KEY_QUOTO)
                    || (keysCode >= JS_KEY_BR1 && keysCode <= JS_KEY_QUOT)
                    || keysCode === JS_KEY_FF_SEMI
                    || keysCode === JS_KEY_FF_PLUS
                    || keysCode === JS_KEY_FF_MIN
                ) {
                    return e.shiftKey ? key[4] : key[3];
                } else if ((keysCode >= JS_KEY_0P && keysCode <= JS_KEY_DIVP) || keysCode === JS_KEY_OR_1) {
                    return key[3];
                } else {
                    return key[2];
                }
            }
        }
        return 'Unknow';
    }
}
