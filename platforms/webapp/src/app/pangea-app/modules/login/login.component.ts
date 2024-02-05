import { Component, OnInit, ChangeDetectorRef, ViewChild, TemplateRef, OnDestroy } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { LoginService } from './login.service';
import { Router } from '@angular/router';

import { TiTipRef, TiTipService, TiMessageService, TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { FormGroup, FormBuilder } from '@angular/forms';
import * as commonValidtor from 'src/app/common-app/utils/multValid';

// 通用方法引用
import * as utils from 'src/app/common-app/utils';
import { HttpResponse, HttpHeaders, HttpClient } from '@angular/common/http';
import { LanguageService, UserInfoService, CommonService, ErrortipService, GlobalDataService, HelpRelationService } from 'src/app/common-app/service';
import { LoginType } from './model/loginType.model';
import { GlobalData, InputLimit } from 'src/app/common-app/models';

@Component({
    selector: 'app-login',
    templateUrl: './login.component.html',
    styleUrls: ['./scss/login.component.scss']
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
        private fb: FormBuilder,
        private errorTip: ErrortipService,
        private httpClient: HttpClient
    ) {
        this.globalData.sessionTimeOut = false;
        // 盘古只有普通账户密码登录
        this.loginType = this.baseLoginType.common;
        this.noPwdLoginError = false;
        this.noPwdLoginState = '';

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
    public imgLoginUrl: string = 'extern/custom/login.png';
    public imgLoginTitleUrl: string = '/extern/custom/login_logo.png';
    public loginFailed: boolean = false;
    public isLogining: boolean = false;
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

    // 密码修改表单
    public modifyForm: FormGroup;
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
    public modifyState: number = -1;
    public savePwdFailedMessage: string = '';

    // 域名列表
    public domainList = [
        {
            value: 'LocaliBMC',
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
        this.userInfo.isTwoFacCertification = false;
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

        if (localStorage.getItem('ssoError')) {
            this.loginType = this.baseLoginType.sso;
            this.noPwdLoginError = true;
            this.noPwdLoginState = this.ssoError('');
            setTimeout(() => {
                this.noPwdLoginState = '';
                this.noPwdLoginError = false;
                this.loginType = this.baseLoginType.common;
            }, 10000);
            localStorage.removeItem('ssoError');
        }
    }

    // 获取不同风格的登录背景图片
    public getLoginImg(obj) {
        obj.imgLoginUrl = '/assets/pangea-assets/image/home/default/login.jpg';
    }
    public getLoginTitleImg(obj) {
        obj.imgLoginTitleUrl = '/assets/pangea-assets/image/home/default/login_logo.png';
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

            if (data.SecurityBanner !== undefined) {
                this.isNoticeShow = true;
                const notice = utils.htmlEncode(data.SecurityBanner);
                this.securityNotice = notice.replace(/\r|\n/g, '<br>').replace(/\s/g, '&nbsp;');
            }

            this.lockedTime = data.AccountLockoutDuration;
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
        if (item.value === 'LocaliBMC') {
            this.userNameLength = InputLimit.len16;
            this.pwdLength = InputLimit.len20;
        } else {
            this.userNameLength = InputLimit.len255;
            this.pwdLength = InputLimit.len255;
        }
        this.clearContent('user');
        this.clearContent('pwd');
        this.destoryDoaminTip();
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
        this.modifyState = -1;
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
            case 1:
                if (this.pwdCheck === -1) {
                    break;
                }
                this.updatePwd = true;
                this.needModifyPwd = false;
                this.modifyState = -1;
                this.savePwdFailedMessage = '';
                this.initModifyInfo();
                this.setPwdValidator(this.pwdCheck);
                break;
            case 2:
                this.goHome();
                break;
            case 3:
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
        if (this.modifyState === 2) {
            return;
        }
        const userParam = {
            Password: this.modifyForm.get('newPwd').value
        };
        this.modifyState = 2;

        this.loginService.modifyUserInfo(this.userInfo.userId, userParam, this.modifyForm.get('oldPwd').value).subscribe((res) => {
            // 成功后，需要跳回登录页面，重新登录
            this.modifyState = 1;
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
            this.modifyState = 0;
            this.modifyForm.get('oldPwd').setValue('');
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
        this.destoryDoaminTip();
    }

    // 过滤KVMHandler的search值
    private checkKVMHandler(search) {
        let searchStr = '';
        if (search && search.length) {
            const searchArr = search.substr(1, search.length).split('&');
            searchArr.forEach((item) => {
                if (item.split('=')[0].toLowerCase() === 'openway'
                    && (item.split('=')[1] === 'html5' || item.split('=')[1] === 'jre' || item.split('=')[1] === 'java')) {
                    if (item.split('=')[1] === 'java') {
                        item = item.replace('java', 'jre');
                    }
                    searchStr += item + '&';
                }
                if (item.split('=')[0] === 'authParam' && (/^[0-9a-fA-f]+$/.test(item.split('=')[1]))) {
                    searchStr += item + '&';
                }
                if (item.split('=')[0] === 'lp' && (/^[a-z]{2,10}$/.test(item.split('=')[1]))) {
                    searchStr += item + '&';
                }
                if (item.split('=')[0] === 'lang' && (/^[a-z]{2,10}$/.test(item.split('=')[1]))) {
                    searchStr += item + '&';
                }
            });
            if (searchStr.length) {
                return '?' + searchStr.substr(0, searchStr.length - 1);
            } else {
                return '';
            }
        } else {
            return '';
        }
    }

    public kvmLogin(param) {
        const url = '/UI/Rest/KVMHandler';
        let par = '';
        for (const key in param) {
            if (param.hasOwnProperty(key)) {
                const value = param[key];
                par += `${key}=${value}&`;
            }
        }
        par = par.substring(0, par.length - 1);
        const headers: HttpHeaders = new HttpHeaders({
            'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
        });
        const httpHeaders = { headers };
        return this.httpClient.post(url, par, httpHeaders);
    }

    // 点击登录按钮后执行方法
    public login() {
        if (sessionStorage.getItem('kvmConnect')) {
            const param = {
                'check_pwd': this.userParams.pwd,
                'logtype': this.userParams.domain.type === 'Local' ? 0 : 1,
                'user_name': this.userParams.account,
                'func': 'DirectKVM',
                'IsKvmApp': '0',
                'KvmMode': '1'
            };
            this.isLogining = true;
            this.kvmLogin(param).subscribe((res) => {
                this.clearLoginInfo();
                const stateCode = res['DirectKVM'][0];
                if (stateCode === 0) {
                    const search = this.checkKVMHandler(sessionStorage.getItem('kvmSearch'));
                    if (localStorage.getItem('kvmJava')) {
                        let searchURL = search;
                        if (searchURL &&
                            (searchURL.includes('openWay=jre') ||
                                searchURL.includes('openway=jre') ||
                                searchURL.includes('openWay=java') ||
                                searchURL.includes('openway=java'))) {
                            searchURL += `&jumpflag=notjump`;
                        }
                        self.window.location.href = '/UI/Rest/KVMHandler' + searchURL;
                        localStorage.removeItem('kvmJava');
                    } else {
                        let url = '/UI/Rest/KVMHandler' + search;
                        if (search && search.includes('authParam=')) {
                            url += `&jumpflag=notjump`;
                        }
                        this.loginService.getKVMHandler(url).subscribe(response => {
                            this.router.navigate(['/kvm_h5'], {
                                state: {
                                    ['kvmHtml5Info']: JSON.stringify(response?.kvmHtml5Info)
                                }
                            });
                        });
                        sessionStorage.removeItem('kvmConnect');
                        sessionStorage.removeItem('kvmSearch');
                    }
                } else {
                    this.loginFailed = true;
                    const errorMsgCode = this.ssoError(stateCode);
                    this.errorMessage = this.translate.instant(errorMsgCode);
                }
                this.isLogining = false;
            }, () => {
                this.clearLoginInfo();
                this.loginFailed = true;
                this.isLogining = false;
                this.errorMessage = this.translate.instant('LOGING_SSO_ERROR_1');
            });
            return;
        }

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

        if (loginType === 0) {
            this.userInfo.loginWithoutPassword = false;
        } else {
            this.userInfo.loginWithoutPassword = true;
            if (loginType === this.baseLoginType.twofactor) {
                this.userInfo.isTwoFacCertification = true;
            }
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
                            rangeSize: [8, 20]
                        },
                        message: {
                            rangeSize: utils.formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'), [8, 20]),
                            pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                            specialChartAndSpace: this.translate.instant('VALIDATOR_PWD_VALIDATOR3'),
                            notEqualPosRev: this.translate.instant('VALIDATOR_PWD_VALIDATOR4')
                        }
                    }
                }
            };

            const validator = Object.assign({}, userPasswordChecked);
            const validatorArr = [
                TiValidators.rangeSize(8, 20),
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
                const needResetPassword = passwordReset.insecurePrompt === 'On' ? true : false;
                const expiration = passwordReset.expiration || 30;
                // SSO和双因素认证登录不需要判定是否需要密码
                if (this.loginType === this.baseLoginType.twofactor ||
                    this.loginType === this.baseLoginType.krb ||
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
                    this.pwdCheck = res;
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
            default:
                errorMsg = 'LOGING_SSO_ERROR_1';
                break;
        }
        return errorMsg;
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
