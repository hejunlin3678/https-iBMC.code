import { Component, OnInit, ChangeDetectorRef, ViewChild, TemplateRef, OnDestroy } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { LanguageService } from 'src/app/common-app/service/language.service';
import { LoginService } from './login.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { Router } from '@angular/router';
import { CommonService } from 'src/app/common-app/service/common.service';
import { ErrortipService } from 'src/app/common-app/service/errortip.service';

import { TiTipRef, TiTipService, TiMessageService, TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { UntypedFormGroup, UntypedFormBuilder } from '@angular/forms';
import * as commonValidtor from 'src/app/common-app/utils/multValid';

// 通用方法引用
import * as utils from 'src/app/common-app/utils';
import { HelpRelationService } from 'src/app/common-app/service/helprelation.service';
import { LoginType } from './model/loginType.model';
import { HttpResponse, HttpHeaders, HttpClient } from '@angular/common/http';
import { GlobalData, InputLimit, PasswordLength} from 'src/app/common-app/models';
import { OperatorType, ModifyState } from 'src/app/irm-app/models/irm.dataType';

@Component({
    selector: 'app-login',
    templateUrl: './login.component.html',
    styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit, OnDestroy {

    constructor(
        private translate: TranslateService,
        private langService: LanguageService,
        private loginService: LoginService,
        private router: Router,
        private userInfo: UserInfoService,
        private common: CommonService,
        private errorTipService: ErrortipService,
        private tiTip: TiTipService,
        private tiMessage: TiMessageService,
        private globalData: GlobalDataService,
        private ref: ChangeDetectorRef,
        private helpService: HelpRelationService,
        private fb: UntypedFormBuilder,
        private errorTip: ErrortipService,
        private httpClient: HttpClient
    ) {
        // 只要是跳到了登录页，我们就认为是会话已经被注销了，有效的Tabs个数归0
        localStorage.setItem('tabs', JSON.stringify(0));

        this.globalData.sessionTimeOut = false;
        // 查询是否开启SSO登录
        this.checkSSOLogin();
        if (localStorage.getItem('resourceId') && localStorage.getItem('resourceToken')) {
            this.userInfo.token = localStorage.getItem('resourceToken');
            this.loginService.loginOut(localStorage.getItem('resourceId')).subscribe((res) => {
                localStorage.removeItem('resourceId');
                localStorage.removeItem('resourceToken');
                this.userInfo.token = null;
            });
        }
    }
    @ViewChild('noticeTpl', { static: true }) noticeTpl: TemplateRef<any>;
    public webStyle = GlobalData.getInstance().getWebStyle;
    public hideNoWarming: boolean = true;
    // 登录方式
    public baseLoginType = new LoginType();
    public loginType: number = this.baseLoginType.undetermined;
    // 双因素登录时的重试次数
    private retryCount: number = 0;
    // 无密码登录时登录状态的文字提示
    public noPwdLoginState: string = '';
    // 无密码登录的当前状态
    public noPwdLoginError: boolean = false;
    public imgUrl: string;

    public loginFailed: boolean = false;
    public isLogining: boolean = false;
    public showSsoLogin: boolean = false;
    public errorMessage: string = '';
    public securityNotice: string = '';
    public lockedTime: number = 300;
    public copyright: string = this.globalData.copyright;
    public productName: string = this.globalData.productName;
    public fqdn: string = this.globalData.fqdn;
    public docSupport: boolean = false;
    private pwdCheck: boolean | number = -1;
    public isNoticeShow: boolean = false;
    private noticeInstant: TiTipRef = null;
    private domainInstant: TiTipRef = null;
    // 修改密码的最小长度
    public minPwdLength: number = PasswordLength.MIN;

    // 密码修改表单
    public modifyForm: UntypedFormGroup;
    private i18n: any = {
        LOGIN_DOMAIN_IBMC: this.translate.instant('LOGIN_DOMAIN_IBMC'),
        LOGIN_DOMAIN_AUTO: this.translate.instant('LOGIN_DOMAIN_AUTO'),
        LOGIN_RISK_TIP: this.translate.instant('LOGIN_RISK_TIP'),
        LOGIN_BROWSER_OK: this.translate.instant('LOGIN_BROWSER_OK'),
        LOGIN_BROWSER_TEXT: this.translate.instant('LOGIN_BROWSER_TEXT'),
        LOGIN_BROWSER_CHROM: this.translate.instant('LOGIN_BROWSER_CHROM'),
        LOGIN_BROWSER_MOZ: this.translate.instant('LOGIN_BROWSER_MOZ'),
        LOGIN_BROWSER_IE: this.translate.instant('LOGIN_BROWSER_IE'),
        LOGIN_BROWSER_EDG: this.translate.instant('LOGIN_BROWSER_EDG'),
        LOGIN_BROWSER_SAFA: this.translate.instant('LOGIN_BROWSER_SAFA')
    };
    // 密码修改结果，-1为默认，1：成功，0失败,2:正在发送请求修改
    public modifyState: number = ModifyState.Default;
    public savePwdFailedMessage: string = '';

    // 域名列表
    public domainList = [
        {
            value: 'LocaliRM',
            content: 'LOGIN_DOMAIN_IBMC',
            show: true,
            type: 'Local'
        },
        {
            value: 'AutomaticMatching',
            content: 'LOGIN_DOMAIN_AUTO',
            show: false,
            type: 'Auto',
        }
    ];
    // 输入账号密码
    public userParams = {
        account: '',
        pwd: '',
        domain: this.domainList[0]
    };
    // 二维码展示
    public showCode: boolean = false;
    // 域名展示
    public showDomain: boolean = false;

    // 联机帮助
    public languageToken = this.langService.activeLanguage.tiny3Set.toLowerCase();
    public route = this.router.url.split('/').pop();
    public fileName = this.helpService.helpRelation(this.route);

    public languageOptions: any[] = [];
    public activeLanguage: any = null;

    // 是否需要更新密码
    public needModifyPwd: boolean = false;
    // 不在提示保存时状态
    public notRemindSaving: boolean = false;
    // 修改密码
    public updatePwd: boolean = false;

    // 用户名最大长度
    public userNameLength: number = InputLimit.len16;
    // 密码最大长度
    public pwdLength: number = InputLimit.len20;

    // 是否是初始密码
    public isInitPwd: boolean = false;

    public userPasswordValidation: TiValidationConfig;
    public confirmPasswordValidation: TiValidationConfig;
    ngOnDestroy(): void {
        this.hideTip();
    }

    ngOnInit(): void {

        this.userInfo.loginWithoutPassword = false;
        // 监控语言切换变化
        this.translate.onLangChange.subscribe((res) => {
            for (const key in this.i18n) {
                if (Object.prototype.hasOwnProperty.call(this.i18n, key)) {
                    this.i18n[key] = res.translations[key];
                }
            }
            this.ref.detectChanges();
        });

        // 进入登录页后，判断浏览器版本和类型，不在支持范围内的，弹出提示框
        if (GlobalData.getInstance().getIsLowBrowserVer) {
            this.tiMessage.open({
                type: 'warn',
                title: this.i18n.LOGIN_RISK_TIP,
                modalClass: 'version',
                cancelButton: {
                    show: false
                },
                okButton: {
                    text: this.i18n.LOGIN_BROWSER_OK
                },
                content: `<div class="title">${this.i18n.LOGIN_BROWSER_TEXT}</div>
					<p>${this.i18n.LOGIN_BROWSER_CHROM}</p>
					<p>${this.i18n.LOGIN_BROWSER_MOZ}</p>
					<p>${this.i18n.LOGIN_BROWSER_IE}</p>
					<p>${this.i18n.LOGIN_BROWSER_EDG}</p>
					<p>${this.i18n.LOGIN_BROWSER_SAFA}</p>
				`
            });
        }

        this.languageOptions = this.langService.langOptions;
        this.activeLanguage = this.langService.activeLanguage;

        // 如果已经存在了 userInfo,则直接跳转到首页
        if (localStorage.getItem('loginUserInfo')) {
            this.router.navigate(['/navigate']);
        }

        // 设置域名信息
        this.reloadLoginData();

        this.RestartService();
    }

    // 保存是否不通过密码登录状态
    public saveLoginWithoutPassword() {
        this.userInfo.loginWithoutPassword = true;
        this.common.saveUserToStorage();
    }

    // 二维码显示和隐藏
    public toggleCode() {
        this.showCode = !this.showCode;
        if (this.languageToken === 'zh-cn') {
            this.imgUrl = 'code1.png';
        } else {
            this.imgUrl = 'code2.png';
        }
    }

    // 安全公告显示
    public showTip($event): void {
        $event.stopPropagation();
        const elem = $event.target;
        this.noticeInstant = this.tiTip.create(elem, {
            position: 'bottom-left',
            maxWidth: `${elem.offsetWidth}px`
        });
        this.noticeInstant.show(this.noticeTpl);
    }

    // 安全公告隐藏
    public hideTip(): void {
        if (this.noticeInstant) {
            this.noticeInstant.destroy();
        }
    }

    public toHelp() {
        // 为兼容IRM，TCE产品，联机帮助路径暂时做转换，适配以前的路径，等IRM，TCE适配之后此处需删除
        if (this.languageToken === 'fr-fr') {
            this.languageToken = 'fre-fr';
        } else if (this.languageToken === 'ja-ja') {
            this.languageToken = 'jap-ja';
        }
        window.open(`${location.origin}/help/${this.languageToken}/${this.fileName}`);
    }

    // 清除账户和密码, 用于输入框叉号按钮点击事件
    public clearContent(content: 'user' | 'pwd') {
        if (content === 'user') {
            this.userParams.account = '';
        } else {
            this.userParams.pwd = '';
        }
    }

    // 清空账户密码信息
    private clearLoginInfo() {
        this.userParams.account = '';
        this.userParams.pwd = '';
    }

    // 固件升级  刷新
    public RestartService(): void {
        const firmwareToLogin = localStorage.getItem('firmwareToLogin');
        if (firmwareToLogin && firmwareToLogin === 'true') {
            this.setTimerV1();
        }
    }

    public setTimerV1(): void {
        this.isLogining = true;
        this.loginFailed = true;
        this.errorMessage = this.translate.instant('IBMC_LOGIN_PLEASE_AGAIN');
        // 定时轮询，检测重启状态
        setTimeout(() => {
            let timer = null;
            timer = setInterval(() => {
                this.loginService.getV1Info().subscribe((res) => {
                    if (!res.body) {
                        return;
                    }
                    if (timer !== null) {
                        localStorage.setItem('firmwareToLogin', JSON.stringify(false));
                        clearInterval(timer);
                        window.location.reload();
                    }
                }, (error) => {
                    this.errorMessage = this.translate.instant('IBMC_LOGIN_PLEASE_AGAIN');
                });
            }, 5000);
        }, 3000);
    }

    // 当需要重新数据时
    public reloadLoginData(): void {
        this.loginService.getV1Info().subscribe((res: HttpResponse<any>) => {
            // LDAPDomain，KerberosDomain，在没有使能时，字段不存在，在开启使能而没有配置时，LDAPDomain为空数组，KerberosDomain为空字符串
            const data = res.body;
            if (data.QRCodeState) {
                this.docSupport = data.QRCodeState === 'Show';
            }
            const domainArr = [];
            const krbDomain: string = data.KerberosDomain;
            const ldapDomain: string[] = data.LDAPDomain;
            if (data.SecurityBanner !== undefined) {
                this.isNoticeShow = true;
                const notice = utils.htmlEncode(data.SecurityBanner);
                this.securityNotice = notice.replace(/\r|\n/g, '<br>').replace(/\s/g, '&nbsp;');
            }

            this.lockedTime = data.AccountLockoutDuration;
            // 只要开启了KRB使能，就显示SSO登陆
            this.showSsoLogin = (krbDomain !== undefined);

            // 只要KRB使能和LDAP使能开启一个，则显示自动匹配
            this.domainList[1].show = false;
            if (krbDomain !== undefined || ldapDomain !== undefined) {
                this.domainList[1].show = true;
            }

            if (krbDomain !== undefined && krbDomain !== '') {
                domainArr.push({
                    value: krbDomain,
                    type: 'Kerberos',
                    content: krbDomain,
                    show: true
                });
            }

            if (ldapDomain !== undefined) {
                const newArr = ldapDomain.map((item: string) => {
                    return {
                        value: item,
                        type: 'Ldap',
                        content: item,
                        show: true
                    };
                });
                domainArr.push(...newArr);
            }

            if (domainArr.length > 0) {
                domainArr.forEach(item => {
                    this.domainList.push(item);
                });
            }
        });
    }

    // 选择域名下拉菜单
    public showDomainList(ev): void {
        this.showDomain = !this.showDomain;
        ev.stopPropagation();
    }

    public showDomainTip(ev: MouseEvent, tipMsg: string) {
        const msg = this.translate.instant(tipMsg);
        this.domainInstant = this.tiTip.create(ev.target as Element, {
            maxWidth: '400px',
            position: 'left'
        });
        this.domainInstant.show(msg);
    }

    public hideDomainTip() {
        this.destoryDoaminTip();
    }

    private destoryDoaminTip() {
        if (this.domainInstant) {
            this.domainInstant.destroy();
        }
    }

    public selectDomain(ev, item) {
        this.showDomain = false;
        this.userParams.domain = item;
        if (item.value === 'LocaliRM') {
            this.userNameLength = InputLimit.len16;
            this.pwdLength = InputLimit.len20;
        } else {
            this.userNameLength = InputLimit.len255;
            this.pwdLength = InputLimit.len255;
        }
        ev.stopPropagation();
    }

    public hideDomain() {
        this.showDomain = false;
    }

    // 清楚登录失败时的报错信息
    public clearError() {
        if (this.loginFailed) {
            this.loginFailed = false;
        }
    }

    // 清除密码修改失败时的提示
    public clearModifyError() {
        this.modifyState = ModifyState.Default;
        this.savePwdFailedMessage = '';
    }

    // 清除密码修改的密码
    public clearModifyPwd(name) {
        this.modifyForm.get(name).patchValue('');
    }

    // 按下回车键后，自动登录
    public autoLogin(ev) {
        if (ev.keyCode === 13) {
            this.login();
        }
    }

    // 密码需要修改时，选择操作
    public operator(type) {
        // 1: 立即修改  2：暂不修改  3：不在提示。
        switch (type) {
            case OperatorType.ModifyNow:
                if (this.pwdCheck === -1) {
                    break;
                }
                this.updatePwd = true;
                this.needModifyPwd = false;
                this.modifyState = ModifyState.Default;
                this.savePwdFailedMessage = '';
                this.initModifyInfo();
                this.setPwdValidator(this.pwdCheck);
                break;
            case OperatorType.NotModify:
                this.goHome();
                break;
            case OperatorType.NotPrompt:
                this.neverRemind();
                break;
            default:
        }
    }

    // 密码修改选择 不再提示时
    public neverRemind() {
        this.notRemindSaving = true;
        const userParam = {
            InsecurePromptEnabled: false
        };
        this.loginService.modifyUserInfo(
            this.userInfo.userId,
            userParam,
            this.userParams.pwd
        ).subscribe((res) => {
            // 成功后，跳转到首页
            this.goHome();
        });
    }

    // 从密码修改页面跳回操作选择
    returnOperator() {
        this.updatePwd = false;
        this.needModifyPwd = true;
    }

    // 密码修改确认按钮
    public upgradePwd() {
        // 校验整个表单,如果没有出错信息，result为空对象
        const result = TiValidators.check(this.modifyForm);
        if (result) {
            return;
        }
        // 表示正在保存
        if (this.modifyState === ModifyState.Modifying) {
            return;
        }
        const userParam = {
            Password: this.modifyForm.get('newPwd').value
        };
        this.modifyState = ModifyState.Modifying;

        this.loginService.modifyUserInfo(this.userInfo.userId, userParam, this.modifyForm.get('oldPwd').value).subscribe((res) => {
            // 成功后，需要跳回登录页面，重新登录
            this.modifyState = ModifyState.Success;
            // 延时3秒后跳转到正常登录页重新登录
            setTimeout(() => {
                this.needModifyPwd = false;
                this.updatePwd = false;
                this.common.clear();
                this.userParams.account = '';
                this.userParams.pwd = '';
                this.loginFailed = false;
                this.isLogining = false;
            }, 3000);
        }, (error) => {
            this.modifyState = ModifyState.Fail;
            const errorId = utils.getMessageId(error.error)[0].errorId;
            if (errorId === 'ReauthFailed') {
                this.savePwdFailedMessage = this.translate.instant('SERVICE_PASSWOR_INCORRECT');
            } else {
                this.savePwdFailedMessage = this.errorTip.getErrorMessage(errorId || 'LOGIN_MODIFY_FAILED');
            }
        });
    }

    private goHome() {
        this.userInfo.isExited = false;
        this.router.navigate(['/navigate']);
        this.hideTip();
    }

    // 点击登录按钮后执行方法
    public login() {
        this.isLogining = true;
        // 如果用户名和密码有一个为空，则提示用户名或密码不能为空。
        if (this.userParams.account === '' || this.userParams.pwd === '') {
            this.errorMessage = this.translate.instant('LOGIN_NOT_EMPTY');
            this.loginFailed = true;
            this.isLogining = false;
            this.clearContent('user');
            this.clearContent('pwd');
            return;
        }

        const loginParams = {
            UserName: this.userParams.account,
            Password: this.userParams.pwd,
            Type: this.userParams.domain.type,
            Domain: this.userParams.domain.value
        };

        this.isLogining = true;

        // 发送登录请求，并自定义处理错误
        this.loginService.login(loginParams).subscribe((res: any) => {
            if (res.error) {
                const errorId = utils.getMessageId(res)[0].errorId;
                if (errorId === 'PasswordNeedReset') {
                    this.pwdNeedReset(res);
                }
            } else {
                this.loginSuccess(res, this.loginType);
            }
        }, async (error) => {
            this.loginFailed = true;
            this.userParams.account = '';
            this.userParams.pwd = '';
            this.isLogining = false;
            const errorId = utils.getMessageId(error.error)[0].errorId;
            switch (errorId) {
                case '':
                    this.errorMessage = this.translate.instant('LOGIN_FAILED');
                    break;
                case 'NoAccess':
                    this.errorMessage = this.translate.instant('LOGIN_NOACCESS');
                    break;
                case 'UserLocked':
                    this.errorMessage = this.errorTipService.getErrorMessage('LoginUserLocked');
                    break;
                case 'AuthorizationFailed':
                    this.errorMessage = utils.formatEntry(this.translate.instant('LOGIN_FAILED_ERR'), [this.lockedTime / 60]);
                    break;
                case 'UserPasswordExpired':
                    this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_137');
                    break;
                case 'UserLoginRestricted':
                    this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_144');
                    break;
                default:
                    this.errorMessage = this.errorTipService.getErrorMessage(errorId) || this.translate.instant('LOGIN_FAILED');
            }
        });
    }
    // 点击单点登录按钮后执行方法
    public krbSSOLogin() {
        if (this.isLogining) {
            return;
        }
        this.isLogining = true;
        this.loginFailed = false;
        this.loginService.getSSOKerberos().subscribe((response) => {
            // 截取响应体Negotiate值作为用户名密码参数
            const authorization = response['body'].slice(10);
            const loginParams = {
                'UserName': authorization,
                'Password': authorization,
                'Type': 'KerberosSSO',
                'Domain': 'KerberosSSO'
            };
            this.loginService.login(loginParams).subscribe((resourceInfo) => {
                this.loginType = this.baseLoginType.krb;
                this.saveLoginWithoutPassword();
                this.loginSuccess(resourceInfo, this.loginType);
            }, (error) => {
                this.loginFailed = true;
                this.isLogining = false;
                const errorId: string = utils.getMessageId(error.error)[0].errorId;
                // 根据具体的错误ID来提示信息
                switch (errorId) {
                    case 'AuthorizationFailed':
                        this.errorMessage = this.translate.instant('LOGIN_SSO_NOAUTHRIZED');
                        break;
                    case 'LoginFailed':
                        this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_1');
                        break;
                    case 'NoAccess':
                        this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_136');
                        break;
                    case 'UserLocked':
                        this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_131');
                        break;
                    case 'UserPasswordExpired':
                        this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_137');
                        break;
                    case 'UserLoginRestricted':
                        this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_144');
                        break;
                    case 'SessionLimitExceeded':
                        this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_4096');
                        break;
                    default:
                        this.errorMessage = this.translate.instant('LOGIN_FAILED');
                }
            });
        }, () => {
            // Hanlder接口查询失败后报 单点登录授权失败。
            this.loginFailed = true;
            this.errorMessage = this.translate.instant('LOGIN_SSO_NOAUTHRIZED');
            this.isLogining = false;
        });
    }

    // 登录成功后处理逻辑，正常登录和双因素登录都一样
    private loginSuccess(res, loginType, token?) {
        this.userInfo.token = res.XCSRFToken || token;
        this.userInfo.userId = res.Session ? res.Session.UserID : res.UserID;
        this.userInfo.resourceId = res.Session ? res.Session.SessionID : res.SessionID;
        this.userInfo.isLogin = true;
        this.userInfo.userRole = res.Session ? res.Session.Role : [res.Role];
        this.userInfo.userName = res.Session ? res.Session.UserName : res.UserName;
        this.userInfo.userIp = res.Session ? res.Session.IP : res.IPAddress;
        this.userInfo.lastLoginIp = res.LastLoginIP;

        const loginTime = res.Session ? res.Session.LoginTime : res.LoginTime;
        const lastLoginTime = res.LastLoginTime;
        this.userInfo.loginTime = loginTime ? loginTime.substr(0, 19).replace('T', ' ') : '';
        this.userInfo.lastLoginTime = lastLoginTime ? lastLoginTime.substr(0, 19).replace('T', ' ') : '';

        if (loginType === this.baseLoginType.common || loginType === this.baseLoginType.sso) {
            this.userInfo.loginWithoutPassword = false;
        } else {
            this.userInfo.loginWithoutPassword = true;
        }

        const roles = this.userInfo.userRole;
        // 查询账号权限，并跳转到首页
        const passwordReset = {
            insecurePrompt: res.InsecurePromptEnabled,
            expiration: res.DaysBeforeExpiration
        };
        this.getPrivileges(roles, passwordReset);
    }

    // 登录403需要强制修改密码
    private async pwdNeedReset(userInfo: any) {
        this.userInfo.userId = userInfo.Session ? userInfo.Session.UserID : null;
        this.userInfo.token = userInfo.XCSRFToken ? userInfo.XCSRFToken : null;
        this.userInfo.resourceId = userInfo.Session ? userInfo.Session.SessionID : null;
        // localStorage中保存一下用户sessionId和token，便于用户不更改密码刷新界面时，注销会话
        localStorage.setItem('resourceId', this.userInfo.resourceId);
        localStorage.setItem('resourceToken', this.userInfo.token);
        this.loginFailed = false;
        this.isInitPwd = true;
        // 获取密码复杂度
        await this.getPasswordComplexityCheck();
        this.operator(1);
    }

    // 初始化密码修改表单
    public initModifyInfo() {
        this.modifyForm = this.fb.group({
            'userName': [this.userParams.account],
            'oldPwd': ['', [TiValidators.required]],
            'newPwd': [''],
            'confirmPwd': ['', [commonValidtor.MultVaild.equalTo('newPwd')]]
        });
    }

    // 根据是否开启密码检查设置校验规则
    private setPwdValidator(pwdCheck) {
        if (pwdCheck) {
            // 开启密码校验提示配置
            const userPasswordChecked: TiValidationConfig = {
                type: 'password',
                errorMessage: {
                    rangeSize: this.translate.instant('VALIDTOR_INVALID_PASSWORD'),
                    pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR'),
                    specialChartAndSpace: this.translate.instant('VALIDTOR_FORMAT_ERROR'),
                    notEqualPosRev: this.translate.instant('VALIDTOR_FORMAT_ERROR')
                },
                passwordConfig: {
                    validator: {
                        rule: 'validatorPwd',
                        params: {
                            rangeSize: [this.minPwdLength, 20]
                        },
                        message: {
                            rangeSize: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [this.minPwdLength, 20]),
                            pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                            specialChartAndSpace: this.translate.instant('VALIDATOR_PWD_VALIDATOR3'),
                            notEqualPosRev: this.translate.instant('VALIDATOR_PWD_VALIDATOR4')
                        }
                    }
                }
            };

            const validator = Object.assign({}, userPasswordChecked);
            const validatorArr = [
                TiValidators.rangeSize(this.minPwdLength, 20),
                commonValidtor.MultVaild.pattern(),
                commonValidtor.MultVaild.specialChartAndSpace(),
                TiValidators.notEqualPosRev(() => this.modifyForm.get('userName'))
            ];
            validator.errorMessage['required'] = '';
            validatorArr.unshift(TiValidators.required);

            this.modifyForm.root.get('newPwd').setValidators(validatorArr);
            this.userPasswordValidation = validator;
        } else {
            // 关闭密码检查时的密码校验提示配置
            const userPasswordNoCheck: TiValidationConfig = {
                type: 'blur',
                errorMessage: {},
                tip: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_STANDARD'), [1, 20]),
                tipPosition: 'top'
            };
            const validatorNocheck = Object.assign({}, userPasswordNoCheck);
            const nocheckValidatorArr = [TiValidators.rangeSize(1, 20)];
            nocheckValidatorArr.unshift(TiValidators.required);
            validatorNocheck['errorMessage']['required'] = '';

            this.modifyForm.root.get('newPwd').setValidators(nocheckValidatorArr);
            this.userPasswordValidation = validatorNocheck;
        }

        // 确认密码校验
        this.confirmPasswordValidation = {
            type: 'blur',
            errorMessage: {
                equalTo: this.translate.instant('COMMON_PASSWORDS_DIFFERENT')
            }
        };
    }

    // 登录成功后获取权限列表
    public getPrivileges(roles, passwordReset) {
        if (roles === null) {
            return;
        }
        // 查询所有的角色权限
        this.loginService.getPrivilegs(roles).subscribe({
            next: async (privileges) => {
                // 保存权限
                this.userInfo.privileges = privileges;
                this.common.saveUserToStorage();
                const needResetPassword = (passwordReset.insecurePrompt === 'On');
                const expiration = passwordReset.expiration || 30;
                // SSO认证登录不需要判定是否需要密码
                if (this.loginType === this.baseLoginType.krb ||
                    this.loginType === this.baseLoginType.sso ||
                    !this.userInfo.hasPrivileges(['ConfigureSelf'])
                ) {
                    this.goHome();
                } else if (this.loginType === this.baseLoginType.common && !needResetPassword && expiration > 10) {
                    this.goHome();
                } else {
                    await this.getPasswordComplexityCheck();
                    // 需要修改密码
                    this.needModifyPwd = true;
                    // 不是初始密码
                    this.isInitPwd = false;
                }
            },
            error: (error) => {
                this.loginFailed = true;
                this.errorMessage = this.translate.instant('COMMON_SECONDPWD_ERROR');
                this.clearLoginInfo();
            }
        });
    }

    // 查询是否密码检查开关
    private getPasswordComplexityCheck(): Promise<any> {
        return new Promise((resolve, reject) => {
            this.loginService.getPwdChecked().subscribe({
                next: (res) => {
                    this.pwdCheck = res.PasswordComplexityCheckEnabled;
                    this.hideNoWarming = res.SystemLockDownEnabled;
                    this.minPwdLength = res.MinimumPasswordLength;
                    resolve(res);
                }
            });
        });
    }

    private ssoError(stateCode): string {
        let errorMsg = '';
        switch (Number(stateCode)) {
            case 4096:
                errorMsg = 'LOGING_SSO_ERROR_4096';
                break;
            case 130:
                errorMsg = 'LOGING_SSO_ERROR_130';
                break;
            case 131:
                errorMsg = 'LOGING_SSO_ERROR_131';
                break;
            case 136:
                errorMsg = 'LOGING_SSO_ERROR_136';
                break;
            case 137:
                errorMsg = 'LOGING_SSO_ERROR_137';
                break;
            case 144:
                errorMsg = 'LOGING_SSO_ERROR_144';
                break;
            case 145:
                errorMsg = 'LOGING_SSO_ERROR_145';
                break;
            default:
                errorMsg = 'LOGING_SSO_ERROR_1';
                break;
        }
        return errorMsg;
    }

    // 查询是否开启了SSO登录
    public checkSSOLogin() {
        const ssoLoginInfo = JSON.parse(localStorage.getItem('ssoLoginInfo'));
        if (!ssoLoginInfo) {
            this.loginType = this.baseLoginType.common;
            this.noPwdLoginError = false;
            this.noPwdLoginState = '';
            return;
        }
        // 有sso登录信息
        localStorage.removeItem('ssoLoginInfo');
        this.loginType = this.baseLoginType.sso;
        if (Number(ssoLoginInfo.stateCode) === 0) {
            this.noPwdLoginState = 'LOGIN_WAY_SSO';
            this.loginService.getSessions().subscribe((res) => {
                const userInfo = res.Members.filter(
                    (user: any) => {
                        return user.SessionID === ssoLoginInfo.index;
                    }
                )[0];
                this.loginSuccess(userInfo, this.loginType, ssoLoginInfo.token);
            });
        } else {
            // SSO登录失败状态信息展示
            this.noPwdLoginError = true;
            this.noPwdLoginState = this.ssoError(ssoLoginInfo.stateCode);
            setTimeout(() => {
                this.loginType = this.baseLoginType.common;
                this.noPwdLoginState = '';
                this.noPwdLoginError = false;
            }, 10000);
        }
    }

    // 获取二维码
    public getCode(obj) {
        if (this.languageToken === 'zh-cn') {
            obj.imgUrl = 'assets/common-assets/image/helpdoc_zh.png';
        } else {
            obj.imgUrl = 'assets/common-assets/image/helpdoc_en.png';
        }
    }
}
