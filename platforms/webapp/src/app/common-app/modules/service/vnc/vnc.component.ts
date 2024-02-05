import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { VncService } from './services';
import { UntypedFormControl, Validators, UntypedFormBuilder } from '@angular/forms';
import { TiValidators, TiValidationConfig, TiModalService, TiModalRef } from '@cloud/tiny3';
import { AlertMessageService, LoadingService, UserInfoService, ErrortipService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { getMessageId, formatEntry, getBrowserType } from 'src/app/common-app/utils';

@Component({
    selector: 'app-vnc',
    templateUrl: './vnc.component.html',
    styleUrls: ['../service.component.scss', './vnc.component.scss']
})
export class VncComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private vncService: VncService,
        private tiModal: TiModalService,
        private alert: AlertMessageService,
        private fb: UntypedFormBuilder,
        private router: Router,
        private user: UserInfoService,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService
    ) { }
    public browser: string = getBrowserType().browser;

    // 初始化属性
    public showAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('SERVICE_VNC_SECURITY_TIP')
    };

    public showSSLAlert = {
        type: 'warn',
        openAlert: false,
        label: this.translate.instant('SERVICE_VNC_SSL_SECURITY_TIP')
    };

    // 权限判断
    public isConfigureUser = this.user.hasPrivileges(['ConfigureUsers']);
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isOemKvmUser = this.user.hasPrivileges(['OemKvm']);
    public isSystemLock = this.user.systemLockDownEnabled;
    public loginRuleDisabled: boolean = false;
    public loginWithoutPassword: boolean = this.user.loginWithoutPassword;

    public errorMeg: string = '';
    public loginChange: boolean = false;

    public vncPasswordData = {
        value: 0
    };

    public vncConfigObj = {
        loginRuleList: []
    };

    public vncmaxSession = {
        value: 0
    };

    public vncActiveSession = {
        value: 0
    };
    // 键盘布局
    public keyboardLayout = {
        placeholder: this.translate.instant('SERVICE_VNC_PLACE_HOLDER'),
        value: '',
        list: [{
            id: 'en',
            key: this.translate.instant('HOME_EN_KEYBOARD'),
        }, {
            id: 'jp',
            key: this.translate.instant('SERVICE_VNC_KEY_LABEL_J'),
        }, {
            id: 'de',
            key: this.translate.instant('SERVICE_VNC_KEY_LABEL_D'),
        }]
    };

    public loginRuleCheckedArray = [];
    public vncForm = this.fb.group({
        enableSwitchState: [{ value: null, disabled: false }, []],
        portVncControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
        ],
        timeoutKVMControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(0, 480)]
        ],
        keyboardselected: [{ value: null, disabled: false }, []],
        vncPasswordControl: [{ value: '', disabled: false }, []],
        vncPwdConfirmControl: [{ value: '', disabled: false }, [MultVaild.equalTo('vncPasswordControl')]],
        sslSwitchState: [{ value: null, disabled: false }, []],
    });

    // 端口校验
    public validationPort: TiValidationConfig = {
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
    // vnc密码
    public vncPwdValidate: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            rangeSize: ''
        }
    };
    // 确认密码校验
    public vncPwdConfirmValidate: TiValidationConfig = {
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            equalTo: this.translate.instant('COMMON_PASSWORDS_DIFFERENT')
        }
    };
    // 保存状态
    public vncDisabled: boolean = true;
    public loginRuleData: object = {};
    public vncData: object = {};
    public vncProtocolData: object = {};
    // 初始化属性
    public vncParams: object = {};
    public queryParams1 = {};
    public queryParams2 = {};
    public vncLoginRuleList = [];

    public getPwdCode(vncData) {
        if (vncData) {
            const checkRuleStrict = vncData.PasswordComplexityCheckEnabled;
            if (checkRuleStrict) {
                // 开启密码检查功能
                this.vncForm.get('vncPasswordControl').setValidators(
                    [TiValidators.rangeSize(8, 8), MultVaild.pattern(), MultVaild.special()]
                );
                this.vncPwdValidate = {
                    passwordConfig: {
                        validator: {
                            rule: 'validatorsNew',
                            params: {},
                            message: {
                                rangeSize: this.translate.instant('COMMON_VNC_MUST_CONTAIN'),
                                pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                                special: this.translate.instant('VALIDATOR_PWD_VALIDATOR3'),
                            }
                        }
                    },
                    errorMessage: {
                        rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                        pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                        special: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                    }
                };
            } else {
                // 关闭密码检查功能
                this.vncForm.setControl('vncPasswordControl', new UntypedFormControl('',
                    [Validators.pattern(/^[a-zA-Z0-9~`!\?,.:;\-_''\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]{0,8}$/)])
                );

                this.vncPwdValidate = {
                    tip: formatEntry(this.translate.instant('COMMON_TIPS_NOCHECK'), [1, 8]),
                    tipPosition: 'right',
                    errorMessage: {
                        rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
                    }
                };
            }
        }
        this.vncForm.get('vncPasswordControl').updateValueAndValidity();
        this.vncForm.get('vncPwdConfirmControl').updateValueAndValidity();
    }

    // 初始化数据查询
    public initData = (vncData) => {
        // vnc使能
        this.vncParams['enabbleState'] = vncData.Enabled;
        this.showAlert['openAlert'] = this.vncParams['enabbleState'] ? true : false;
        // 端口
        this.vncParams['vncPort'] = vncData.Port;
        // 超时时长
        this.vncParams['sessionTimeout'] = vncData.SessionTimeoutMinutes;
        // SSL加密
        this.vncParams['Encryption'] = vncData.SSLEncryptionEnabled;
        this.showSSLAlert['openAlert'] = this.vncParams['enabbleState'] === true && this.vncParams['Encryption'] === false ? true : false;

        this.vncForm.patchValue({
            enableSwitchState: this.vncParams['enabbleState'],
            portVncControl: this.vncParams['vncPort'],
            timeoutKVMControl: this.vncParams['sessionTimeout'],
            vncPasswordControl: '',
            vncPwdConfirmControl: '',
            sslSwitchState: this.vncParams['Encryption'],
        });

        // 键盘布局
        this.vncParams['keyboardLayout'] = vncData.KeyboardLayout;
        const keyboardList = this.keyboardLayout['list'];
        for (let i = 0; i < keyboardList.length; i++) {
            if (this.keyboardLayout['list'][i].id === this.vncParams['keyboardLayout']) {
                this.vncForm.patchValue({
                    keyboardselected: this.keyboardLayout['list'][i],
                });
            }
        }
        // 密码有效期(天)
        const vncpasswordDays = Number(vncData.PasswordValidityDays);
        if (vncpasswordDays === 65535) {
            this.vncPasswordData['value'] = this.translate.instant('SERVICE_VNC_INDEFINITE');
        } else if (vncpasswordDays === 0) {
            this.vncPasswordData['value'] = this.translate.instant('SERVICE_LONG_TIME');
        } else {
            this.vncPasswordData['value'] = vncpasswordDays;
        }
        // 最大会话
        this.vncmaxSession['value'] = vncData.MaximumNumberOfSessions;
        // 活跃会话
        this.vncActiveSession['value'] = vncData.NumberOfActiveSessions;
        // 登录规则
        this.vncParams['vncLoginRuleList0'] = false;
        this.vncParams['vncLoginRuleList1'] = false;
        this.vncParams['vncLoginRuleList2'] = false;
        if (vncData.LoginRule.length > 0) {
            vncData.LoginRule.forEach((item) => {
                if (item === 'Rule1') {
                    this.vncParams['vncLoginRuleList0'] = true;
                }
                if (item === 'Rule2') {
                    this.vncParams['vncLoginRuleList1'] = true;
                }
                if (item === 'Rule3') {
                    this.vncParams['vncLoginRuleList2'] = true;
                }
            });
        }
        this.ruleInit(vncData);
        this.getPwdCode(vncData);
    }
    // 登录规则初始化
    public ruleInit(vncData): void {
        const loginRules = vncData.LoginRuleInfo;
        const temp = [];
        if (loginRules) {
            loginRules.forEach((item, i) => {
                temp.push(this.combination(i, item));
            });
            this.vncConfigObj['loginRuleList'] = temp;
            this.loginRuleCheckedArray = [];
            this.vncLoginRuleList = [];
            if (this.vncParams['vncLoginRuleList0']) {
                this.loginRuleCheckedArray.push(this.vncConfigObj['loginRuleList'][0]);
                this.vncLoginRuleList.push(this.vncConfigObj['loginRuleList'][0]);
            }
            if (this.vncParams['vncLoginRuleList1']) {
                this.loginRuleCheckedArray.push(this.vncConfigObj['loginRuleList'][1]);
                this.vncLoginRuleList.push(this.vncConfigObj['loginRuleList'][1]);
            }
            if (this.vncParams['vncLoginRuleList2']) {
                this.loginRuleCheckedArray.push(this.vncConfigObj['loginRuleList'][2]);
                this.vncLoginRuleList.push(this.vncConfigObj['loginRuleList'][2]);
            }
        }
    }
    // 登录规则数据组装
    public combination(i, item): object {
        return {
            disabled: !item.Enabled,
            id: item.ID,
            descIPMac: true,
            title: item.ID.replace('Rule', this.translate.instant('SERVICE_SNMP_RULE')),
            descStartTime: item.StartTime === null ? '- - -' : item.StartTime,
            descEndTime: item.EndTime === null ? '- - -' : item.EndTime,
            descIP: item.IP === null ? '- - -' : item.IP,
            descMAC: item.Mac === null ? '- - -' : item.Mac
        };
    }

    public vncGetInit = () => {
        this.vncService.getVnc().subscribe((response) => {
            this.vncData = response['body'];
            this.initData(this.vncData);
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
            this.vncAuthority();
        }
    }
    // 权限
    public vncAuthority() {
        // 具有安全配置权限可以设置VNC使能、端口
        if (this.isOemSecurityMgmtUser && !this.isSystemLock) {
            this.vncForm.root.get('enableSwitchState').enable();
            this.vncForm.root.get('portVncControl').enable();
        } else {
            this.vncForm.root.get('enableSwitchState').disable();
            this.vncForm.root.get('portVncControl').disable();
        }
        // 具有远程能控制权限可以设置超时时长、键盘布局、VNC密码、登录规则、SSL加密
        if (this.isOemKvmUser && !this.isSystemLock) {
            this.vncForm.root.get('timeoutKVMControl').enable();
            this.vncForm.root.get('keyboardselected').enable();
            this.vncForm.root.get('vncPasswordControl').enable();
            this.vncForm.root.get('vncPwdConfirmControl').enable();
            this.vncForm.root.get('sslSwitchState').enable();
            this.loginRuleDisabled = false;
        } else {
            this.vncForm.root.get('timeoutKVMControl').disable();
            this.vncForm.root.get('keyboardselected').disable();
            this.vncForm.root.get('vncPasswordControl').disable();
            this.vncForm.root.get('vncPwdConfirmControl').disable();
            this.vncForm.root.get('sslSwitchState').disable();
            this.loginRuleDisabled = true;
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.vncAuthority();

        this.vncGetInit();
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        if (!this.vncForm.valid) {
            this.vncDisabled = true;
            return;
        }
        this.loginChange = false;
        this.queryParams1 = {};
        this.queryParams2 = {};
        if (this.vncForm.root.get('enableSwitchState').value !== this.vncParams['enabbleState']) {
            this.queryParams1['Enabled'] = this.vncForm.root.get('enableSwitchState').value;
        }
        if (parseInt(this.vncForm.root.get('portVncControl').value, 10) !== this.vncParams['vncPort']) {
            this.queryParams1['Port'] = parseInt(this.vncForm.root.get('portVncControl').value, 10);
        }
        if (parseInt(this.vncForm.root.get('timeoutKVMControl').value, 10) !== this.vncParams['sessionTimeout']) {
            this.queryParams1['SessionTimeoutMinutes'] = parseInt(this.vncForm.root.get('timeoutKVMControl').value, 10);
        }
        if (this.vncForm.root.get('keyboardselected').value && this.vncForm.root.get('keyboardselected').value.id !== this.vncParams['keyboardLayout']) {
            this.queryParams1['KeyboardLayout'] = this.vncForm.root.get('keyboardselected').value.id;
        }
        if (this.vncForm.root.get('vncPasswordControl').value !== '') {
            this.queryParams2['VNCPassword'] = this.vncForm.root.get('vncPasswordControl').value;
        }
        // 登录规则
        const loginRules = [];
        const loginRuleStart = [];
        this.loginRuleCheckedArray.forEach((item, i) => {
            if (item.id) {
                loginRules.push(item.id);
            }
        });
        this.vncLoginRuleList.forEach((item, i) => {
            if (item.id) {
                loginRuleStart.push(item.id);
            }
        });
        if (loginRules.length !== loginRuleStart.length) {
            this.queryParams1['LoginRule'] = loginRules;
            this.loginChange = true;
        } else {
            for (const key of loginRules) {
                if (!loginRuleStart.includes(key)) {
                    this.queryParams1['LoginRule'] = loginRules;
                    this.loginChange = true;
                    break;
                }
            }
        }
        if (this.vncForm.root.get('sslSwitchState').value !== this.vncParams['Encryption']) {
            this.queryParams1['SSLEncryptionEnabled'] = this.vncForm.root.get('sslSwitchState').value;
        }
        if (JSON.stringify(this.queryParams1) !== '{}' || JSON.stringify(this.queryParams2) !== '{}') {
            this.vncDisabled = false;
        } else {
            this.vncDisabled = true;
        }
    }

    // change
    public vncEnabledChange(): void {
        if (this.vncForm.root.get('enableSwitchState').value === true) {
            this.showAlert['openAlert'] = true;
        } else {
            this.showAlert['openAlert'] = false;
        }

        if (this.vncForm.root.get('enableSwitchState').value === true && this.vncForm.root.get('sslSwitchState').value === false) {
            this.showSSLAlert['openAlert'] = true;
        } else {
            this.showSSLAlert['openAlert'] = false;
        }
        this.protocolEnabledEN();
    }
    public vncPortChange(): void {
        this.protocolEnabledEN();
    }
    public vncTimeoutChange(): void {
        this.protocolEnabledEN();
    }
    public keyboardLayoutChange(): void {
        this.protocolEnabledEN();
    }
    public sslEncryptionChange(): void {
        if (this.vncForm.root.get('enableSwitchState').value === true && this.vncForm.root.get('sslSwitchState').value === false) {
            this.showSSLAlert['openAlert'] = true;
        } else {
            this.showSSLAlert['openAlert'] = false;
        }
        this.protocolEnabledEN();
    }
    public vncPasswordChange(): void {
        this.vncForm.get('vncPwdConfirmControl').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public vncConfirmPasswordChange(): void {
        this.vncForm.get('vncPasswordControl').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public clickRule(item): void {
        this.protocolEnabledEN();
    }
    // 恢复默认值
    public restFN(): void {
        this.vncForm.patchValue({
            portVncControl: 5900,
        });
        this.protocolEnabledEN();
    }

    // 点击跳转安全配置页面
    public configRule(): void {
        // 具有用户配置权限可跳转到在线用户页面
        if (this.isConfigureUser) {
            sessionStorage.setItem('securityTab', 'loginRule');
            this.router.navigate(['/navigate/user/security']);
        }
    }

    // 点击跳转活跃会话页面
    public goToUser(): void {
        if (this.vncActiveSession['value'] !== 0 && this.isConfigureUser) {
            this.router.navigate(['/navigate/user/online-user']);
        }
    }

    // 保存
    public vncSaveButton(): void {
        this.vncDisabled = true;
        this.protocolEnabledEN();
        if (this.vncForm.root.get('vncPasswordControl').value === '') {
            this.vncService.setVncService(this.queryParams1).subscribe((response) => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.vncData = response['body'];
                this.initData(this.vncData);
            }, (error) => {
                const errorId = getMessageId(error.error)[0].errorId;
                const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                this.vncData = error.error.data;
                this.initData(this.vncData);
            });
        } else {
            if (this.loginWithoutPassword) {
                this.vncService.setVncListWithoutPassword(this.queryParams1, this.queryParams2)
                    .then((resArr) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        this.vncGetInit();
                    }, (error) => {
                        const errorId = getMessageId(error.error)[0].errorId;
                    });
            } else {
                const instance = this.tiModal.open(SecondPasswordComponent, {
                    id: 'myModal',
                    context: {
                        password: '',
                        state: false,
                        active: false
                    },
                    beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                        if (!reason) {
                            modalRef.destroy(reason);
                        } else {
                            if (modalRef.content.instance.password === '' || !modalRef.content.instance.active) {
                                return;
                            }
                            modalRef.content.instance.active = false;

                            this.vncService.setVncList(this.queryParams1, this.queryParams2, modalRef.content.instance.password)
                                .then((resArr) => {
                                    instance.close();
                                    modalRef.destroy(reason);
                                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                                    this.vncGetInit();
                                }, (error) => {
                                    // 发生错误时，先清除二次认证密码
                                    instance.content.instance.password = '';
                                    const errorId = getMessageId(error.error)[0].errorId;
                                    if (errorId === 'ReauthFailed') {
                                        modalRef.content.instance.state = true;
                                    } else {
                                        instance.close();
                                    }
                                });
                        }
                    }
                });
            }
        }
    }
}
