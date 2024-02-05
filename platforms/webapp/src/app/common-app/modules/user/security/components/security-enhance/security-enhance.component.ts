import { Subscription } from 'rxjs';
import { TranslateService } from '@ngx-translate/core';
import { Component, OnInit, Output, EventEmitter, Renderer2, ElementRef } from '@angular/core';
import {
    ISwitchOption,
    IAuthorityData,
    IInputOption,
    ISelectOption,
    IFormOption,
    IAuthorityEnum,
    ILoginFailOption,
    ILoginFailSelect
} from './security-enhance.dataype';
import Validators from './validator';
import { SecurityEnhanceService } from './service';
import { formatEntry } from 'src/app/common-app/utils';
import { TiMessageService } from '@cloud/tiny3';
import { SecurityService } from '../../service';
import { Element } from '@angular/compiler';
import {
    UserInfoService,
    LoadingService,
    AlertMessageService,
    CommonService,
    GlobalDataService,
    ErrortipService,
    PRODUCT
} from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-security-enhance',
    templateUrl: './security-enhance.component.html',
    styleUrls: ['./security-enhance.component.scss']
})
export class SecurityEnhanceComponent implements OnInit {
    @Output() changeSystemLocked: EventEmitter<boolean> = new EventEmitter<boolean>();

    public systemLockDownEnabled: boolean;

    private authoritySub: Subscription;

    private saveAuthoritySub: Subscription;

    public buttonDisable = true;

    public minimumPasswordLength: string;

    private inited = false;

    private changedForm: IFormOption[];

    private i18n = {
        VALIDATOR_PASSWORD_PERIOD_BIG: this.translate.instant('VALIDATOR_PASSWORD_PERIOD_BIG'),
        VALIDATOR_PASSWORD_PERIOD: this.translate.instant('VALIDATOR_PASSWORD_PERIOD'),
        SECURITY_PERIOD_NOLIMITED: this.translate.instant('SECURITY_PERIOD_NOLIMITED'),
        VALID_INTEGER_INFO: this.translate.instant('VALID_INTEGER_INFO'),
        VALID_VALUE_RANGE_TIP: this.translate.instant('VALID_VALUE_RANGE_TIP'),
        VALIDATOR_VALIDATOR_PASSWORD_ONLY: this.translate.instant('VALIDATOR_VALIDATOR_PASSWORD_ONLY'),
        VALIDATOR_PASSWORD_USAGE_PERIOD: this.translate.instant('VALIDATOR_PASSWORD_USAGE_PERIOD'),
        TLS_1_POINTER_2_LABEL: this.translate.instant('TLS_1_POINTER_2_LABEL'),
        TLS_1_POINTER_3_LABEL: this.translate.instant('TLS_1_POINTER_3_LABEL'),
    };

    private validator = new Validators(this.i18n);

    public isConfigureUser = this.userService.privileges.indexOf('ConfigureUsers') >= 0;

    public isOemSecurityMgmtUser = this.userService.privileges.indexOf('OemSecurityMgmt') >= 0;

    public isEmmApp: boolean = CommonData.productType === PRODUCT.EMM;

    public isIrmApp: boolean = CommonData.productType === PRODUCT.IRM;

    // 系统锁定模式
    public openLock: ISwitchOption = {
        show: false,
        label: 'SECURITY_HOME_OPENLOCK',
        value: false,
        initValue: false,
        id: 'openLock',
        associated: IAuthorityEnum.systemLockDownEnabled,
        disabled: this.userService.userRole.indexOf('Administrator') < 0,
    };
    // 业务侧用户管理使能
    public serviceEnable: ISwitchOption = {
        label: 'SECURITY_SERVICE_ENABLE',
        value: false,
        initValue: false,
        associated: IAuthorityEnum.OSUserManagementEnabled,
        id: 'serviceEnable',
        disabled: !this.isConfigureUser
    };
    // 密码检查
    public passwordCheck: ISwitchOption = {
        label: 'SECURITY_PASSWORD_CHECK',
        value: false,
        initValue: false,
        id: 'passwordCheck',
        associated: IAuthorityEnum.passwordComplexityCheckEnabled,
        tip: this.isOemSecurityMgmtUser ? 'SECUTITY_CFG_USER_MNG_MEG' : '',
        disabled: !this.isOemSecurityMgmtUser
    };
    // SSH密码认证
    public loginSecurity: ISwitchOption = {
        label: 'SECURITY_SSH_AUTHENTICATION',
        value: false,
        initValue: false,
        id: 'loginSecurity',
        associated: IAuthorityEnum.SSHPasswordAuthenticationEnabled,
        tip: this.isConfigureUser ? 'SECUTITY_CFG_PWD_CHECK' : '',
        disabled: !this.isConfigureUser
    };
    // 防DNS重绑定
    public dnsBinding: ISwitchOption = {
        label: 'SECURITY_ANTI_REBIND',
        value: false,
        initValue: false,
        id: 'dnsBinding',
        associated: IAuthorityEnum.antiDNSRebindEnabled,
        disabled: !this.isOemSecurityMgmtUser
    };
    // TLS版本
    public tlsVersion: ISelectOption = {
        label: 'SECURITY_TLS_VERSION',
        id: 'tlsVersion',
        tip: this.isOemSecurityMgmtUser ? 'SECURITY_TLS_TIPS' : '',
        value: null,
        initValue: '',
        options: [
            {
                id: 'TLS1.2',
                label: this.i18n.TLS_1_POINTER_2_LABEL,
            },
            {
                id: 'TLS1.3',
                label: this.i18n.TLS_1_POINTER_3_LABEL,
            }
        ],
        associated: IAuthorityEnum.tlsVersion,
        disabled: !this.isOemSecurityMgmtUser,
    };

    // 密码有效期
    public passwordPeriod: IInputOption = {
        id: 'passwordPeriod',
        label: 'SECURITY_VNC_PASSWORD_DATE',
        controll: this.validator.getPeriodValid(),
        validation: {
            tip: this.isOemSecurityMgmtUser ? this.translate.instant('SECUTITY_CFG_PWD_DAY') : '',
            tipPosition: 'right',
            ...this.validator.tiCommonValidation
        },
        initValue: '',
        // 密码有效期与密码最短使用期的校验互相关联，当有一处值更改时，另一处的校验需要重新设置
        change: (value) => {
            const currentValue = this.leastPeriod.controll.value;
            this.validator.setLeastPeriodValid(currentValue, Number(value));
            this.detectChange();
        },
        blur() {
            inputBlurFn.call(this);
            this.clearErrorInfo();
        },
        clearErrorInfo: () => {
            this.clearErrorInfo();
        },
        associated: IAuthorityEnum.passwordValidityDays,
        disabled: !this.isOemSecurityMgmtUser
    };

    // 密码最小长度配置
    public passwordMinLen: IInputOption = {
        id: 'passwordMinLen',
        label: 'PASSWPRD_MIN_LEN',
        controll: this.validator.getPasswordMinLen(),
        validation: {
            tip: this.isOemSecurityMgmtUser ? this.translate.instant('VALIDATOR_PASSWORD_MIN_LEN') : '',
            tipPosition: 'right',
            ...this.validator.tiCommonValidation
        },
        initValue: '',
        change: () => {
            this.detectChange();
        },
        blur() {
            inputPasswordBlurFn.call(this);
            this.clearErrorInfo();
        },
        clearErrorInfo: () => {
            this.clearErrorInfo();
        },
        associated: IAuthorityEnum.minimumPasswordLength,
        disabled: !this.isOemSecurityMgmtUser
    };

    // 密码最短使用期
    public leastPeriod: IInputOption = {
        id: 'leastPeriod',
        label: 'SECURITY_MINIMUM_PERIOD',
        controll: this.validator.getLeastPeriodValid(),
        validation: {
            tip: this.isOemSecurityMgmtUser ? this.translate.instant('SECUTITY_CFG_MIN_PWD_DAY') : '',
            tipPosition: 'right',
            ...this.validator.tiCommonValidation
        },
        initValue: '',
        blur() {
            inputBlurFn.call(this);
            this.clearErrorInfo();
        },
        clearErrorInfo: () => {
            this.clearErrorInfo();
        },
        associated: IAuthorityEnum.minimumPasswordAgeDays,
        // 密码有效期与密码最短使用期的校验互相关联，当有一处值更改时，另一处的校验需要重新设置
        change: (value) => {
            const currentValue = this.passwordPeriod.controll.value;
            this.validator.setPeriodValid(currentValue, Number(value));
            this.detectChange();
        },
        disabled: !this.isOemSecurityMgmtUser
    };
    // 不活动期限
    public inactivity: IInputOption = {
        id: 'inactivity',
        label: 'SECURITY_INACTIVITYPERIOD',
        blur() {
            inputBlurFn.call(this);
        },
        controll: this.validator.getInactivityValid(),
        validation: {
            tip: this.isOemSecurityMgmtUser ? this.translate.instant('SECURITY_PERIOD_NOLIMITED') : '',
            tipPosition: 'right',
            ...this.validator.tiCommonValidation
        },
        initValue: '',
        associated: IAuthorityEnum.accountInactiveTimelimit,
        disabled: !this.isOemSecurityMgmtUser
    };

    // 紧急登录用户
    public emergencyUser: ISelectOption = {
        label: 'SECURITY_EMERGENCY_USER',
        id: 'emergencyUser',
        options: [],
        value: null,
        initValue: '',
        tip: this.isOemSecurityMgmtUser ? 'SECURITY_EMERGENCY_USER_TIP' : '',
        show: this.isConfigureUser,
        associated: IAuthorityEnum.emergencyLoginUser,
        change: (value) => {
            this.securityServ.userSelectId = value.id;
            this.detectChange();
        },
        disabled: !this.isOemSecurityMgmtUser
    };

    // 禁用历史密码
    public banPassword: ISelectOption = {
        label: 'SECURITY_DISABLE_HISTORY_PASSWORD',
        id: 'banPassword',
        tip: this.isOemSecurityMgmtUser ? 'SECURITY_HISTORY_DISABLED_TIP' : '',
        value: null,
        initValue: '',
        options: this.initBanPwdOpt(),
        associated: IAuthorityEnum.previousPasswordsDisallowedCount,
        disabled: !this.isOemSecurityMgmtUser,
    };

    // 登录失败锁定
    public loginFailSelect: ILoginFailSelect = {
        label: 'SECURITY_LOGIN_FAILURE_LOCK',
        id: 'loginFailSelect',
        options: this.initLoginFailOpt(),
        value: null,
        initValue: '',
        associated: IAuthorityEnum.accountLockoutThreshold,
        disabled: !this.isOemSecurityMgmtUser,
        changeFn: (option) => {
            this.loginFailInput.show = !option.isIgnoreTime;
            this.detectChange();
        }
    };

    public loginFailInput: IInputOption = {
        id: 'loginFailInput',
        show: true,
        initValue: '',
        controll: this.validator.getLoginFailValid(),
        validation: {
            tip: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [1, 30]),
            tipPosition: 'right',
            ...this.validator.tiCommonValidation
        },
        associated: IAuthorityEnum.accountLockoutDuration,
        disabled: !this.isOemSecurityMgmtUser,
    };

    // 证书过期提前告警时间
    public expireWarning: IInputOption = {
        label: 'SECURITY_EXPIRE_WARNING',
        id: 'expireWarning',
        controll: this.validator.getExpiresValid(),
        validation: {
            tip: this.isOemSecurityMgmtUser ? this.translate.instant('SECURITY_EXPIRE_WARNING_TIP') : '',
            tipPosition: 'right',
            ...this.validator.tiCommonValidation
        },
        recommendValue: '90',
        blur() {
            inputBlurFn.call(this);
        },
        initValue: '90',
        associated: IAuthorityEnum.certificateOverdueWarningTime,
        disabled: !this.isOemSecurityMgmtUser,
    };

    private formComponents = [this.openLock, this.serviceEnable, this.passwordCheck, this.loginSecurity, this.tlsVersion,
    this.passwordMinLen, this.passwordPeriod, this.leastPeriod, this.inactivity, this.emergencyUser, this.banPassword, this.loginFailSelect,
    this.loginFailInput, this.expireWarning, this.dnsBinding];

    // 有校验的组件
    private hasControlComps = this.formComponents.filter((comp): comp is IInputOption => (comp as IInputOption).controll !== undefined);

    public showMessage() {
        const tipsInfo = [];
        // 只有当密码复杂度由开启状态变为关闭状态时，才提示“禁用密码复杂度检查会降低系统安全性。”
        const pwdItem = this.formComponents.filter(item => item.id === 'passwordCheck')[0];
        if (pwdItem && !pwdItem.value && pwdItem.initValue) {
            tipsInfo.push(this.translate.instant('SECURITY_CLOSE_PWD_CHECK'));
        }
        if (parseInt(this.passwordPeriod.controll.value, 10) !== 0) {
            tipsInfo.push(this.translate.instant('SECURITY_URGENT_USER_TIP'));
        }
        if (parseInt(this.inactivity.controll.value, 10) !== 0) {
            tipsInfo.push(this.translate.instant('SECURITY_URGENT_USER_TIP2'));
        }
        if (this.tlsVersion.options[0].label) {
            tipsInfo.push(this.translate.instant('COMMON_ASK_OK'));
        }
        let content = '<div>';
        if (tipsInfo.length > 0) {
            tipsInfo.forEach((value, index) => {
                const count = '<p>' + (tipsInfo.length > 1 ? index + 1 + ') ' : '');
                content += count + value + '</p>';
            });
            content += '</div>';
        } else {
            content = this.translate.instant('COMMON_ASK_OK');
        }

        const messageModal = this.tiMessage.open({
            id: 'oneKeyModel',
            type: 'prompt',
            okButton: {
                show: true,
                click: () => {
                    this.save();
                    messageModal.close();
                },
                autofocus: false,
                text: this.translate.instant('COMMON_OK')
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_CANCEL')
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content
        });
    }

    private save() {
        let hasTls = false;
        this.changedForm = this.getShowedComps(this.formComponents).filter((form) => {
            if (form.controll) {
                return !this.isEqual(form.controll.value, form.initValue);
            }
            if (!this.isEqual(form.value, form.initValue)) {
                if (form.associated === IAuthorityEnum.tlsVersion) {
                    hasTls = true;
                    return false;
                }
                return true;
            }
            return false;
        });
        if (hasTls) {
            this.changedForm = this.changedForm.concat(this.tlsVersion);
        }

        this.saveAuthoritySub = this.securityEnhanceService.saveParams(this.changedForm).subscribe((data) => {
            // 如报200，但是有错误对象的时候
            if (data.error) {
                const errorId = getMessageId({ error: data.error })[0]?.errorId;
                const errorMsg = this.errorTip.getErrorMessage(errorId || 'COMMON_FAILED');
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: errorMsg
                });
            } else {
                this.alert.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS',
                });
            }

            this.saveAuthoritySub.unsubscribe();
            this.initValue(data);

            // 更新系统锁定状态
            this.userService.systemLockDownEnabled = data.systemLockDownEnabled;
            this.globalData.systemLockState.next(data.systemLockDownEnabled);
            this.commonService.saveUserToStorage();
        }, () => {
            this.saveAuthoritySub.unsubscribe();
            this.getData();
        });

    }

    private initBanPwdOpt() {
        const options = [];
        for (let i = 0; i < 6; i++) {
            options.push({
                id: i,
                label: `${i}`
            });
        }
        return options;
    }

    private initLoginFailOpt(): ILoginFailOption[] {
        const options = [];
        for (let j = 0; j < 7; j++) {
            let isIgnoreTime = false;
            let labelValue = String(j);
            if (j === 0) {
                labelValue = this.translate.instant('SECURITY_UNLIMITED');
                isIgnoreTime = true;
            }
            options.push({
                id: j,
                label: labelValue,
                isIgnoreTime,
            });
        }
        return options;
    }

    constructor(
        private commonService: CommonService,
        private userService: UserInfoService,
        private globalData: GlobalDataService,
        private translate: TranslateService,
        private securityEnhanceService: SecurityEnhanceService,
        private loadingService: LoadingService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private securityServ: SecurityService,
        private rd2: Renderer2,
        private el: ElementRef,
        private errorTip: ErrortipService
    ) { }

    ngOnInit() {
        this.getData();
    }

    private getData() {
        this.loadingService.state.next(true);
        this.authoritySub = this.securityEnhanceService.getData().subscribe((response) => {
            this.loadingService.state.next(false);
            this.initValue(response);
            this.inited = true;
        }, () => {
            this.loadingService.state.next(false);
        });
    }

    private setSystemLockDownEnabled(value: boolean) {
        this.systemLockDownEnabled = value;
        this.changeSystemLocked.emit(this.systemLockDownEnabled);
        this.changeDisableStatus();
    }

    // type为text的input框，disable状态需要通过formControll来更改
    private changeDisableStatus() {
        const textForms = [this.passwordPeriod, this.passwordMinLen,
        this.passwordMinLen, this.leastPeriod, this.inactivity, this.loginFailInput, this.expireWarning];
        for (const form of textForms) {
            if (this.systemLockDownEnabled || form.disabled) {
                form.controll.disable();
            } else {
                form.controll.enable();
            }
        }
    }

    // 根据接口返回值进行初始赋值
    private initValue(authorityData: IAuthorityData): void {
        this.openLock.value = authorityData.systemLockDownEnabled;
        this.setSystemLockDownEnabled(Boolean(this.openLock.value));
        this.openLock.show = typeof (this.openLock.value) === 'boolean';
        this.emergencyUser.options = authorityData.user;
        this.passwordCheck.value = authorityData.passwordComplexityCheckEnabled;
        this.serviceEnable.value = authorityData.OSUserManagementEnabled;
        this.loginSecurity.value = authorityData.sshPasswordAuthenticationEnabled;
        this.dnsBinding.value = authorityData.antiDNSRebindEnabled;
        this.passwordPeriod.controll.setValue(String(authorityData.passwordValidityDays));
        this.leastPeriod.controll.setValue(String(authorityData.minimumPasswordAgeDays));
        this.passwordMinLen.controll.setValue(String(authorityData.minimumPasswordLength));
        this.minimumPasswordLength = String(authorityData.minimumPasswordLength);
        this.inactivity.controll.setValue(String(authorityData.accountInactiveTimelimit));
        this.emergencyUser.value = this.emergencyUser.options.find((item) => {
            return item.id === authorityData.emergencyLoginUser;
        });
        // 紧急登录用户选择项，在登录规则页面使用
        this.securityServ.userSelectId = authorityData.emergencyLoginUser;
        this.banPassword.value = this.banPassword.options.find((item) => {
            return item.id === authorityData.previousPasswordsDisallowedCount;
        });
        this.loginFailSelect.value = this.loginFailSelect.options.find((item) => {
            return item.id === authorityData.accountLockoutThreshold;
        });
        this.loginFailInput.controll.setValue(String(authorityData.accountLockoutDuration));
        this.loginFailInput.show = !this.loginFailSelect.value.isIgnoreTime;
        this.expireWarning.controll.setValue(String(authorityData.certificateOverdueWarningTime));
        this.tlsVersion.value = this.tlsVersion.options.find((item) => {
            return item.id === authorityData.tlsVersion[0];
        });
        if (!this.passwordCheck.value || this.systemLockDownEnabled || !this.isOemSecurityMgmtUser) {
            this.passwordMinLen.controll.disable();
        } else {
            this.passwordMinLen.controll.enable();
        }
        this.setInitValue();
    }

    // 关联校验的组件，当相关联的组件输入正确时，需清除关联组件的错误提示
    private clearErrorInfo() {
        this.hasControlComps.forEach((comp) => {
            if (!comp.controll.errors) {
                const formEle = this.el.nativeElement.querySelector(`#${comp.id}`);
                if (!formEle) {
                    return;
                }
                const containErrorEle: Element = formEle.parentNode;
                const childEle = [...containErrorEle.children];
                const errorEle = childEle.find((ele: any) => ele.className.includes('ti3-unifyvalid-error-container'));
                if (errorEle) {
                    this.rd2.setStyle(errorEle, 'display', 'none');
                }
            }
        });
    }

    // 记录后台返回值，以后用来做判断是否进行了更改
    private setInitValue(): void {
        for (const form of this.formComponents) {
            if (form.controll !== undefined) {
                form.initValue = form.controll.value;
            } else if (typeof form.value === 'object') {
                form.initValue = Object.assign({}, form.value);
            } else {
                form.initValue = form.value;
            }
        }
    }

    private getShowedComps(comps: IFormOption[]): IFormOption[] {
        return comps.filter(comp => comp.show !== false);
    }

    private isEqual(a: any, b: any): boolean {
        if (typeof a !== typeof b) {
            return false;
        }
        if (typeof a === 'object') {
            const aKeys = Object.keys(a);
            const bKeys = Object.keys(b);
            if (aKeys.length !== bKeys.length) {
                return false;
            }

            for (const key of aKeys) {
                if (!this.isEqual(a[key], b[key])) {
                    return false;
                }
            }

            return true;
        }

        return a === b;
    }

    public serviceEnableChange() {
        const switchState = !this.serviceEnable.value;
        if (!switchState) {
            this.tiMessage.open({
                id: 'serviceEnableDialog',
                type: 'prompt',
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.translate.instant(
                    'SECURITY_SERVICE_ENABLE_CLOSE'
                ),
                close: () => {
                    this.serviceEnable.value = switchState;
                    this.detectChange();
                },
                dismiss: () => {}
            });
        } else {
            this.serviceEnable.value = switchState;
            this.detectChange();
        }
    }

    public detectChange(data?: string) {
        if (!this.inited) {
            return;
        }
        if (data === 'passwordCheck') {
            if (!this.passwordCheck.value) {
                this.passwordMinLen.controll.disable();
                this.passwordMinLen.controll.setValue(this.minimumPasswordLength);
            } else {
                this.passwordMinLen.controll.enable();
            }
        }
        // 因为select选项的更改有滞后，所以添加延时
        setTimeout(() => {
            const showedHasControlComps = this.getShowedComps(this.hasControlComps);
            if (showedHasControlComps.some((comp) => comp.controll.errors)) {
                this.buttonDisable = true;
                return;
            }
            const showedFormComps = this.getShowedComps(this.formComponents);
            for (const form of showedFormComps) {
                if (form.controll) {
                    if (!this.isEqual(form.controll.value, form.initValue)) {
                        this.buttonDisable = false;
                        return;
                    }
                } else if (!this.isEqual(form.value, form.initValue)) {
                    this.buttonDisable = false;
                    return;
                }
            }
            this.buttonDisable = true;
        }, 0);
    }

    ngOnDestroy(): void {
        this.authoritySub.unsubscribe();
    }

}

function inputBlurFn() {
    if (this.controll.value === '') {
        this.controll.setValue(this.recommendValue || '0');
    }
}

function inputPasswordBlurFn() {
    if (this.controll.value === '') {
        this.controll.setValue(this.recommendValue || '8');
    }
}
