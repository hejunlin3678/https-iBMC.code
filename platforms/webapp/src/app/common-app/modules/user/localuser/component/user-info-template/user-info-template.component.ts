/**
 * 新增用户得页面加载基本流程
 * 1. 查询已经使用得用户ID。
 * 2. 查询是否开启密码检查
 */
import { Component, OnInit, Output, Input, EventEmitter } from '@angular/core';
import { LocalUserService } from '../../localuser.service';

import { TranslateService } from '@ngx-translate/core';
import { TiValidationConfig, TiValidators, TiModalService } from '@cloud/tiny3';
import { formatEntry, errorTip, computeRepeatTimes } from 'src/app/common-app/utils';

// form组件,校验
import { UntypedFormBuilder, UntypedFormControl, UntypedFormGroup, Validators, ValidationErrors } from '@angular/forms';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import { IUserId, IAlgorithm, ICustError, IUsersInfo } from '../../../dataType/data.interface';
import {
    FormControlOperators,
    SnmpV3Tips,
    LoginInterfaces,
    PwdValidatorOperators,
    OperatorResult,
    SecondAuthorityFail,
    UnDeleteUsers
} from '../../../dataType/data.model';
import { timer } from 'rxjs/internal/observable/timer';
import {
    LoadingService,
    AlertMessageService,
    UserInfoService,
    SystemLockService,
    CommonService
} from 'src/app/common-app/service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { PasswordLength } from 'src/app/common-app/models/common.interface';
import { TimeoutService } from 'src/app/common-app/service/timeout.service';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT, PRODUCTTYPE } from 'src/app/common-app/service/product.type';
import { LoginInterfaceTipComponent } from '../login-interface-tip/login-interface-tip.component';

@Component({
    selector: 'app-user-info-template',
    templateUrl: './user-info-template.component.html',
    styleUrls: ['./user-info-template.component.scss']
})
export class UserInfoTemplateComponent implements OnInit {
    @Input() row: IUsersInfo = null;
    @Input() id: string = '';
    @Input() halfTitle: string = '';
    @Output() close: EventEmitter<boolean> = new EventEmitter<boolean>();
    @Output() reInit: EventEmitter<boolean> = new EventEmitter<boolean>();
    // 响应式表单类型定义
    public systemLocked: boolean = false;
    public userFormInfo: UntypedFormGroup;
    public scrollTop: number = null;
    private baseFormInfo: { [key: string]: any } = {};
    private baseLoginRule: { [key: string]: any }[] = [];
    private baseLoginInterface: string[] = [];
    private changedData: { [key: string]: any } = {};
    private saveState: boolean = false;
    public destoryed: boolean = false;
    // 当前用户登录密码输入错误时得报错
    public secondError: boolean = false;
    public hasConfigSelf: boolean = false;
    public hasConfigUsers: boolean = false;
    public isLastAdmin: boolean = false;
    public snmpv3PwdInitTip: string = '';
    private initFinished: boolean = false;
    private minPwdLength: number = this.userInfo.customizedId === 1 ? PasswordLength.CUSTOM : PasswordLength.MIN;
    public loginWithoutPassword: boolean = this.userInfo.loginWithoutPassword;
    // 首次登录密码修改策略是否支持
    public pwdResetPolicySupport: boolean = false;

    private ADD_IPMI_ERROR_PREFIX: string = 'AddIPMI';
    // 新增或修改用户名时，以下几种错误应该在页面的输入框后做提示
    private USERNAME_ERRORS: string[] = [
        'ResourceAlreadyExists',
        'ConflictWithLinuxDefaultUser',
        'UserIsLoggingIn',
        'UserNameIsRestricted',
        'InvalidUserName',
        'TrapV3loginUser'
    ];
    // 密码修改失败时的错误类型
    private PASSWORD_ERRORS: string[] = [
        'PasswordNotSatisfyComplexity',
        'PasswordComplexityCheckFail',
        'DuringMinimumPasswordAge',
        'InvalidPasswordSameWithHistory',
        'PasswordInWeakPWDDict',
        'PropertyValueExceedsMaxLength',
        'InvalidPassword'
    ];

    constructor(
        private userInfo: UserInfoService,
        private globalData: GlobalDataService,
        private localUserService: LocalUserService,
        private translate: TranslateService,
        private loading: LoadingService,
        private alertService: AlertMessageService,
        private fb: UntypedFormBuilder,
        private lockService: SystemLockService,
        private commonService: CommonService,
        private timeoutService: TimeoutService,
        private tiModal: TiModalService
    ) {
        // 判断用户是否具有配置自身和用户配置权限
        this.hasConfigSelf = this.userInfo.hasPrivileges(['ConfigureSelf']);
        this.hasConfigUsers = this.userInfo.hasPrivileges(['ConfigureUsers']);
    }

    // 页面报错信息，输入框的后端校验报错
    public pageError: { [key: string]: any } = {
        userName: {
            asyncError: {
                show: false,
                message: ''
            }
        },
        userPwd1: {
            asyncError: {
                show: false,
                message: ''
            },
            forceReset: {
                author: false,
                ipmi: false
            }
        },
        snmpPwd1: {
            asyncError: {
                show: false,
                message: ''
            },
            forceReset: {
                author: false
            }
        }
    };

    // 鉴权算法数据和加密算法数据
    public authorityOptions: IAlgorithm[] = [
        { label: 'MD5', id: 'MD5' },
        { label: 'SHA', id: 'SHA' },
        { label: 'SHA1', id: 'SHA1', disabled: true },
        { label: 'SHA256', id: 'SHA256' },
        { label: 'SHA384', id: 'SHA384' },
        { label: 'SHA512', id: 'SHA512' }
    ];
    public encrypOptions: IAlgorithm[] = [
        { label: 'DES', id: 'DES' },
        { label: 'AES', id: 'AES' },
        { label: 'AES256', id: 'AES256' }
    ];

    /**
     * 输入校验相关配置信息，注：校验规则不是在这里定义，校验规则在fb.group中定义，后续对校验规则的修改，需要用到 formControl的setValidators方法
     * 1.用户名校验 userNameValidation
     * 2.密码校验 userPasswordValidation
     */
    public userNameValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            'required': '',
            'pattern': this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            'notPoints': this.translate.instant('VALIDTOR_FORMAT_ERROR')
        },
        tipPosition: 'top',
        tip: this.translate.instant('USER_USERNAME_TIP'),
        tipMaxWidth: '350px'
    };

    public userPasswordValidation: TiValidationConfig;
    public snmpPasswordValidation: TiValidationConfig;
    // 密码校验提示配置
    public userPasswordChecked: TiValidationConfig = {
        type: 'password',
        errorMessage: {
            required: '',
            rangeSize: this.translate.instant('VALIDTOR_INVALID_PASSWORD'),
            pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            specialChartAndSpace: this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            notEqualPosRev: this.translate.instant('VALIDTOR_FORMAT_ERROR')
        },
        passwordConfig: {
            validator: {
                rule: 'validatorPwd',
                params: {
                    rangeSize: [this.minPwdLength, PasswordLength.MAX]
                },
                message: {
                    rangeSize: formatEntry(this.translate.instant('VALIDATOR_PWD_VALIDATOR1'),
                        [this.minPwdLength, PasswordLength.MAX]),
                    pattern: this.translate.instant('COMMON_SPECIAL_CHARACTERS'),
                    specialChartAndSpace: this.translate.instant('VALIDATOR_PWD_VALIDATOR3'),
                    notEqualPosRev: this.translate.instant('VALIDATOR_PWD_VALIDATOR4')
                }
            }
        }
    };
    // 关闭密码检查时的密码校验提示配置
    public userPasswordNoCheck: TiValidationConfig = {
        type: 'blur',
        errorMessage: {},
        tip: formatEntry(this.translate.instant('VALIDATOR_PWD_STANDARD'), [PasswordLength.NOPWDCHECKLENGTH, PasswordLength.MAX]),
        tipPosition: 'top'
    };
    // 确认密码校验
    public confirmPasswordValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: '',
            equalTo: this.translate.instant('COMMON_PASSWORDS_DIFFERENT')
        }
    };

    // 确认SNMPV3密码
    public confirmSnmpPwdValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            require: '',
            equalTo: this.translate.instant('COMMON_PASSWORDS_DIFFERENT')
        }
    };

    // disabled: 当数据无效变化时为true, 当校验失败时为true
    public disabled: boolean = true;
    // 源表单数据
    public unposition: boolean = false;
    public userIdList: IUserId[] = [];

    // 用户ID,密码绑定
    public userLoginPwd: string = '';
    public pwdCheck: boolean = false;
    public initUserData: { [key: string]: any } = {};
    public passwordTip: string = '';

    // 规则列表和规则选中列表
    public ruleList: { [key: string]: any }[] = [];
    public ruleChecks: { [key: string]: any }[] = [];

    // 接口列表
    public interfaceList: string[] = ['SSH', 'IPMI', 'Local', 'SFTP', 'Web', 'SNMP', 'Redfish'];
    // 接口选中项数组和是否选中SNMP
    public interfaceChecks: string[] = [];

    // 首次登录密码修改策略
    public modifyPwdStrategy = [
        {
            label: this.translate.instant('USER_MODIFY_PWD_HINT'),
            value: 1
        },
        {
            label: this.translate.instant('USER_MODIFY_PWD_FORCE'),
            value: 2
        }
    ];

    // 角色列表
    public roleList = [
        {
            id: 'Administrator',
            label: this.translate.instant('COMMON_ROLE_ADMINISTRATOR')
        },
        {
            id: 'Operator',
            label: this.translate.instant('COMMON_ROLE_OPERATOR')
        },
        {
            id: 'Commonuser',
            label: this.translate.instant('COMMON_ROLE_COMMONUSER')
        },
        {
            id: 'CustomRole1',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE1')
        },
        {
            id: 'CustomRole2',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE2')
        },
        {
            id: 'CustomRole3',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE3')
        },
        {
            id: 'CustomRole4',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE4')
        },
        {
            id: 'Noaccess',
            label: this.translate.instant('COMMON_ROLE_NOACCESS')
        }
    ];

    // 显示隐藏登录规则
    public showDetails(item): void {
        item.active = !item.active;
    }

    ngOnInit() {
        // 如果是编辑用户，则标识是否是最后有个管理员用户
        if (this.row) {
            this.isLastAdmin = this.row.unDeletedReason === UnDeleteUsers.LASTADMIN;
        }
        // 盘古-首次登录策略只保留强制修改选项
        if (this.globalData.productType === PRODUCTTYPE.PANGEA) {
            // 三大洋不支持snmp
            this.interfaceList.splice(5, 1);
            this.modifyPwdStrategy = [
                {
                    label: this.translate.instant('USER_MODIFY_PWD_FORCE'),
                    value: 2
                }
            ];
        }

        // 系统锁定功能
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
            const method = res ? FormControlOperators.DISABLE : FormControlOperators.ENABLE;
            const controls = this.userFormInfo.controls;
            Object.keys(controls).forEach(key => {
                // 当状态变为开启时，需要验证是否具有权限
                if (method === FormControlOperators.ENABLE) {
                    // 如果目标是用户名，用户ID，角色时，没有权限操作，则不处理
                    if (!this.hasConfigUsers && (key === 'userId' || key === 'userName' || key === 'roleId' || key === 'forceModify')) {
                        return;
                    }
                }

                // 最后一个管理员账户不支持修改角色
                if (key === 'roleId' && this.isLastAdmin) {
                    return;
                }

                // 获取控件当前状态
                const isEnabled = controls[key].enabled;
                const curState = isEnabled ? FormControlOperators.ENABLE : FormControlOperators.DISABLE;
                // 当状态变化时，才去修改
                if (method !== curState) {
                    controls[key][method]();
                }
            });
        });

        // 初始化formGroup
        this.initFormGroup();

        // 开始loading
        this.loading.state.next(true);

        // 若不支持snmp，则不显示snmp接口
        if (!this.userInfo.snmpSupport) {
           this.interfaceList.splice(5, 1);
        }

        // 添加用户和编辑用户分别执行不同的初始化操作
        if (this.row === undefined) {
            this.loadAddUserData();
        } else {
            this.loadEditUserData();
        }
    }

    private initFormGroup(): void {
        // form表单formGroup抽象
        const userPwd2Validator: ValidationErrors[] = [MultVaild.equalTo('userPwd1')];
        if (this.row === undefined) {
            userPwd2Validator.unshift(TiValidators.required);
        }

        /**
         * 用户名禁用得条件，没有用户配置权限，系统锁定，角色禁用得条件，无用户配置权限，最后一个管理员用户及系统锁定
         * 页面初始化时，禁用没有加入系统锁定是因为，系统锁定时，进不了用户编辑和新增页面
         */
        this.userFormInfo = this.fb.group({
            'userId': [{ value: this.row || null, disabled: !this.hasConfigUsers }],
            'userName': [{ value: '', disabled: !this.hasConfigUsers }, [
                Validators.required,
                Validators.pattern(/^[a-zA-Z0-9_\-`~!@#$^*()=+|{};\[\].?]{1,16}$/),
                Validators.pattern(/^[^#+-].*$/),
                MultVaild.notPoints()
            ]
            ],
            'userPwd1': ['', []],
            'userPwd2': ['', userPwd2Validator],
            'forceModify': [{ value: null, disabled: !this.hasConfigUsers }],
            'roleId': [{ value: null, disabled: !this.hasConfigUsers || this.isLastAdmin }]
        });
    }

    // 新增用户时的初始化操作
    private loadAddUserData(): void {
        this.localUserService.getAddUserParams().subscribe((res) => {
            this.loading.state.next(false);

            // 设置剩余ID，角色
            this.userIdList = res.userIdList;
            this.userFormInfo.patchValue({ 'userId': this.userIdList[0] });
            this.userFormInfo.patchValue({ 'roleId': this.roleList[7] });
            this.pwdCheck = res.pwdCheck;
            this.minPwdLength = res.minPwdLength;

            // 设置首次登录密码修改策略, 只有支持该功能时才设置
            if (res.firstLoginResetSupport) {
                this.pwdResetPolicySupport = true;
                // 盘古-新增用户时只有强制修改选项
                if (this.globalData.productType === PRODUCTTYPE.PANGEA) {
                    this.userFormInfo.patchValue({ 'forceModify': this.modifyPwdStrategy[0] });
                } else {
                    this.userFormInfo.patchValue({ 'forceModify': this.modifyPwdStrategy[1] });
                }
            }

            // 登录规则
            this.ruleList = res.rules;

            // 新增时，接口全部选中
            this.interfaceChecks.push(...this.interfaceList);

            // 设置密码校验规则
            this.setPasswordValidator(this.pwdCheck, 'add');

            // 绑定数据变化监听,Form表单和checkbox
            this.bindDataListen();
            this.saveBaseData();

            timer(100).subscribe(() => {
                this.initFinished = true;
            });
        });
    }

    // 编辑用户时的初始化操作
    private loadEditUserData(): void {
        this.localUserService.getEditUserParams(this.row.id).subscribe((res) => {
            this.initUserData = JSON.parse(JSON.stringify(res));
            this.loading.state.next(false);
            if (!res.userInfo.roleId) {
                this.alertService.eventEmit.emit({ type: 'error', label: 'USER_MODIFY_NOT_EXIST' });
                setTimeout(() => {
                    this.close.next(true);
                }, 1000);
                return;
            }
            // 设置用户ID,设置用户名
            this.userIdList = [{ id: this.row.id, label: String(this.row.id) }];
            this.userFormInfo.patchValue({ 'userId': this.userIdList[0] });
            this.userFormInfo.patchValue({ 'userName': res.userInfo.userName });
            this.snmpv3PwdInitTip = res.snmpv3PwdInit ? SnmpV3Tips.NOTSAFE : SnmpV3Tips.SAFE;
            this.pwdCheck = res.pwdCheck;
            this.minPwdLength = res.minPwdLength;

            // 设置首次登录密码修改策略
            if (res.userInfo.firstLoginPolicy !== undefined) {
                this.pwdResetPolicySupport = true;
                const selectStrategyArr = this.modifyPwdStrategy.filter((item: any) => {
                    return item.value === res.userInfo.firstLoginPolicy;
                });
                const selectStrategy = selectStrategyArr.length > 0 ? selectStrategyArr[0] : this.modifyPwdStrategy[1];
                this.userFormInfo.patchValue({ 'forceModify': selectStrategy });
            }

            // 设置用户的角色
            this.roleList.forEach((item) => {
                if (item.id === res.userInfo.roleId) {
                    this.userFormInfo.patchValue({ 'roleId': item });
                }
            });

            // 登录规则
            this.ruleList = res.rules;
            // 根据用户的规则信息，选中规则项
            this.ruleChecks = this.ruleList.filter(item => {
                return this.row.loginRole.indexOf(String(item.id)) > -1;
            });

            // 根据用户的接口配置，确定哪些被选中
            this.interfaceList.forEach((item: string) => {
                if (res.userInfo.interfaceList.indexOf(item) > -1) {
                    this.interfaceChecks.push(item);
                }
            });

            // 设置用户密码校验
            this.setPasswordValidator(this.pwdCheck);
            this.setSnmpv3PwdControl('add', this.pwdCheck);

            // 绑定数据变化监听.Form表单和checkbox
            this.bindDataListen();
            this.saveBaseData();
            timer(100).subscribe(() => {
                this.initFinished = true;
            });
        });
    }

    // 保存基础数据
    private saveBaseData(): void {
        // 设置基础原始数据
        this.baseFormInfo = JSON.parse(JSON.stringify(this.userFormInfo.value));
        this.baseLoginRule = [...this.ruleChecks];
        this.baseLoginInterface = [...this.interfaceChecks];
    }

    // 根据密码的当前长度，返回validator提示对象信息
    private changeTipOver20(length: number): TiValidationConfig {
        let validator: TiValidationConfig = null;
        if (this.pwdCheck) {
            if (length >= 20) {
                const tempLimitValidator = Object.assign({}, this.userPasswordChecked);
                tempLimitValidator.passwordConfig.validator.message.rangeSize = this.translate.instant('VALIDATOR_MAX_LIMIT');
                validator = tempLimitValidator;
            } else {
                const tempValidator = Object.assign({}, this.userPasswordChecked);
                tempValidator.passwordConfig.validator.message.rangeSize = formatEntry(
                    this.translate.instant('VALIDATOR_PWD_VALIDATOR1'),
                    [this.minPwdLength, PasswordLength.MAX]
                );
                validator = tempValidator;
            }
        } else {
            if (length >= 20) {
                validator = Object.assign({}, this.userPasswordNoCheck, {
                    tip: this.translate.instant('VALIDATOR_MAX_LIMIT')
                });
            } else {
                validator = Object.assign({}, this.userPasswordNoCheck, {
                    tip: formatEntry(this.translate.instant('VALIDATOR_PWD_STANDARD'),
                        [PasswordLength.NOPWDCHECKLENGTH, PasswordLength.MAX]),
                });
            }
        }
        return validator;
    }

    // 监听form表单数据变化
    private bindDataListen = () => {
        const controls = this.userFormInfo.controls;
        Object.keys(controls).map((key) => {
            controls[key].valueChanges.subscribe((value) => {
                // 未完成初始化时，不做处理
                if (!this.initFinished) {
                    return;
                }
                if (JSON.stringify(this.baseFormInfo[key]) !== JSON.stringify(value)) {
                    this.changedData[key] = true;
                } else {
                    delete this.changedData[key];
                }

                if (key === 'snmpPwd1') {
                    const snmpv3ConfirmValue = this.userFormInfo.controls.snmpPwd2.value;
                    this.userFormInfo.controls.snmpPwd2.setValue(snmpv3ConfirmValue);
                    this.snmpPasswordValidation = this.changeTipOver20(value.length);
                    this.globalData.appDetectChagne.next(true);
                }

                if (key === 'userPwd1') {
                    // 触发确认密码的校验
                    const pwd2Value = this.userFormInfo.controls.userPwd2.value;
                    this.userFormInfo.controls.userPwd2.setValue(pwd2Value);

                    // 当输入密码长度超出20位时，提示 已达到最大字符限制,因为tip组件不支持动态更新，因此只能触发一次根组件变更检测来被动触发tip组件重新渲染
                    this.userPasswordValidation = this.changeTipOver20(value.length);
                    this.globalData.appDetectChagne.next(true);
                }

                // 如果是鉴权算法发生改变，需要强制修改密码和snmpv3密码。
                if (key === 'authority') {
                    if (value.id === this.initUserData.userInfo.authProtocol) {
                        this.forceUpdatePwd(false, 'author');
                    } else {
                        this.forceUpdatePwd(true, 'author');
                    }

                    if (value.id === 'MD5' || value.id === 'SHA') {
                        this.alertService.eventEmit.emit({ type: 'warn', label: 'INSECURE_AUTHENTICATION_ALGORITHM_' + value.id });
                    }
                }

                // 加密算法AES256目前只支持的鉴权算法 SHA256,SHA384,SHA512
                if (key === 'encryp') {
                    this.updateAuthorityList(value.id);

                    if (value.id === 'DES') {
                        this.alertService.eventEmit.emit({ type: 'warn', label: 'INSECURE_ENCRYPTION_ALGORITHM' });
                    }
                }

                this.computedState();
            });
        });
    }

    public clearAsyncError(target: string): void {
        this.pageError[target].asyncError.show = false;
        this.pageError[target].asyncError.message = '';
    }

    // 登录规则变更时事件
    public ruleChanged(key): void {
        const flag = JSON.stringify(this.ruleChecks.sort()) === JSON.stringify(this.baseLoginRule.sort());
        if (flag) {
            delete this.changedData[key];
        } else {
            this.changedData[key] = true;
        }
        this.computedState();
    }

    // 登录接口变更时事件
    public interfaceChanged(item: string, key: string): void {
        if (!this.initFinished) {
            return;
        }

        // 开启SNMP不需要强制修改密码，开启IPMI，则需要强制修改密码
        const baseIpmiEnabled = this.baseLoginInterface.indexOf(LoginInterfaces.IPMI) > -1;
        const newIpmiEnabled = this.interfaceChecks.indexOf(LoginInterfaces.IPMI) > -1;

        // IPMI变化从关闭到开启时，强制修改密码
        if (!baseIpmiEnabled) {
            if (newIpmiEnabled) {
                this.forceUpdatePwd(true, 'ipmi');
            } else {
                this.forceUpdatePwd(false, 'ipmi');
            }
        }
        if (item === LoginInterfaces.SNMP && this.row) {
            this.SNMPChange();
        }
        const flag = JSON.stringify(this.interfaceChecks.sort()) === JSON.stringify(this.baseLoginInterface.sort());
        if (flag) {
            delete this.changedData[key];
        } else {
            this.changedData[key] = true;
        }
        this.computedState();
    }

    // SNMP关闭开启事件, 开启时增加校验，关闭时删除校验
    private SNMPChange(): void {
        if (this.interfaceChecks.indexOf('SNMP') > -1) {
            this.pageError.snmpPwd1.asyncError.show = false;
        } else {
            // 除由于鉴权算法变更导致的强制修改密码的功能
            this.forceUpdatePwd(false, 'author');
        }
    }

    // 计算保存按钮的状态
    private computedState(): void {
        setTimeout(() => {
            // 表示formControl是否发生了有效的变化
            let formState = Object.keys(this.changedData).length > 0;
            formState = formState && this.userFormInfo.valid;

            // 表示二次认证密码是否是有效的
            const pwdState = this.loginWithoutPassword ? true : (this.userLoginPwd.length > 0 && !this.secondError);

            // 表示是否不存在有异步的错误
            const notExistAsyncError = !this.pageError.userName.asyncError.show
                && !this.pageError.userPwd1.asyncError.show
                && !this.pageError.snmpPwd1.asyncError.show;

            // 只要3个状态有任意一个为false，保存按钮将被禁用
            this.disabled = !(formState && pwdState && notExistAsyncError && !this.saveState);
        }, 0);
    }

    // 当前登录密码变化时，清除密码报错信息
    public pwdChange(): void {
        this.secondError = false;
        this.computedState();
    }

    // 按回车键时，保存数据
    public enter(ev: KeyboardEvent): void {
        if (ev.keyCode === 13) {
            this.save();
        }
    }

    // 设置密码校验规则和V3密码的提示信息
    private setPasswordValidator(pwdCheck, type?: string): void {
        if (pwdCheck) {
            let validator: TiValidationConfig = null;
            const tempValidator = Object.assign({}, this.userPasswordChecked);
            tempValidator.passwordConfig.validator.message.rangeSize = formatEntry(
                this.translate.instant('VALIDATOR_PWD_VALIDATOR1'),
                [this.minPwdLength, PasswordLength.MAX]
            );
            validator = tempValidator;
            const validatorArr = [
                TiValidators.rangeSize(this.minPwdLength, PasswordLength.MAX),
                MultVaild.pattern(),
                MultVaild.specialChartAndSpace(),
                TiValidators.notEqualPosRev(() => this.userFormInfo.get('userName'))
            ];
            // 添加用户时，密码是必填项
            if (type === PwdValidatorOperators.ADD) {
                validator.errorMessage.required = '';
                validatorArr.unshift(TiValidators.required);
            }

            this.userFormInfo.root.get('userPwd1').setValidators(validatorArr);
            this.userPasswordValidation = validator;

        } else {
            const nocheckValidatorArr = [TiValidators.rangeSize(1, 20)];
            const validatorNocheck = Object.assign({}, this.userPasswordNoCheck);
            // 添加用户是，密码是必填项
            if (type === PwdValidatorOperators.ADD) {
                nocheckValidatorArr.unshift(TiValidators.required);
                validatorNocheck.errorMessage.required = '';
            }
            this.userFormInfo.root.get('userPwd1').setValidators(nocheckValidatorArr);
            this.userPasswordValidation = validatorNocheck;
        }
    }

    // 新增和删除snmpv3密码和确认密码
    private setSnmpv3PwdControl(type: 'add' | 'delete', pwdCheck: boolean) {
        const validatorChecked = [
            TiValidators.rangeSize(this.minPwdLength, PasswordLength.MAX),
            MultVaild.pattern(),
            MultVaild.specialChartAndSpace(),
            TiValidators.notEqualPosRev(() => this.userFormInfo.get('userName'))
        ];
        const validatorNoChecked = [TiValidators.rangeSize(PasswordLength.NOPWDCHECKLENGTH, PasswordLength.MAX)];
        const validator = this.pwdCheck ? Object.assign({}, this.userPasswordChecked) : Object.assign({}, this.userPasswordNoCheck);
        this.snmpPasswordValidation = validator;

        // 如果是添加SNMP控件（也就是SNMP接口是开启状态的）
        if (type === PwdValidatorOperators.ADD) {
            let snmpv3Pwd1Control = null;
            if (pwdCheck) {
                snmpv3Pwd1Control = new UntypedFormControl('', validatorChecked);
            } else {
                snmpv3Pwd1Control = new UntypedFormControl('', validatorNoChecked);
            }
            const snmpv3Pwd2Control = new UntypedFormControl('', [MultVaild.equalTo('snmpPwd1')]);
            const encrypItem = this.encrypOptions.filter(item => {
                return item.id === this.initUserData.userInfo.encryProtocol;
            })[0];

            // 加密算法为AES256的场景下，配置鉴权算法时options只有下面三项
            if (encrypItem?.id === 'AES256') {
                this.authorityOptions = [
                    { label: 'SHA256', id: 'SHA256' },
                    { label: 'SHA384', id: 'SHA384' },
                    { label: 'SHA512', id: 'SHA512' }
                ];
            }
            const authorItem = this.authorityOptions.filter(item => {
                return item.id === this.initUserData.userInfo.authProtocol;
            })[0];
            this.userFormInfo.addControl('snmpPwd1', snmpv3Pwd1Control);
            this.userFormInfo.addControl('snmpPwd2', snmpv3Pwd2Control);
            this.userFormInfo.addControl('authority', new UntypedFormControl(authorItem));
            this.userFormInfo.addControl('encryp', new UntypedFormControl(encrypItem));
        } else {
            this.userFormInfo.removeControl('snmpPwd1');
            this.userFormInfo.removeControl('snmpPwd2');
            this.userFormInfo.removeControl('authority');
            this.userFormInfo.removeControl('encryp');
        }
    }

    // 新增或编辑用户方法
    private saveData(): void {
        // Id传字符串
        const param: any = { Id: parseInt(this.userFormInfo.get('userId').value.id, 10) };
        if (this.row === undefined) {
            // 如果是新增用户页面, 则下发所有参数
            param.UserName = this.userFormInfo.get('userName').value;
            param.OldUserName = this.baseFormInfo.userName;
            param.Password = this.userFormInfo.get('userPwd1').value;
            param.RoleId = this.userFormInfo.get('roleId').value.id;
            param.LoginInterface = this.interfaceChecks;
            param.LoginRule = this.ruleChecks.map((item) => {
                return `Rule${item.id}`;
            }).sort();
            // 如果支持密码修改策略，则加上该参数
            if (this.pwdResetPolicySupport) {
                param.FirstLoginPolicy = this.userFormInfo.get('forceModify').value.value;
            }
        } else {
            // 编辑用户页面，则下发有变化的参数
            if (this.changedData.userName) {
                param.UserName = this.userFormInfo.value.userName;
            }
            if (this.changedData.userPwd1) {
                param.Password = this.userFormInfo.value.userPwd1;
            }
            if (this.changedData.roleId) {
                param.RoleId = this.userFormInfo.value.roleId && this.userFormInfo.value.roleId.id;
            }

            if (this.changedData.interface) {
                param.LoginInterface = this.interfaceChecks.sort();
            }
            if (this.changedData.rule) {
                param.LoginRule = this.ruleChecks.map((item) => {
                    return `Rule${item.id}`;
                }).sort();
            }
            // 如果支持密码修改策略切修改策略有变更，则添加该参数
            if (this.pwdResetPolicySupport && this.changedData.forceModify) {
                param.FirstLoginPolicy = this.userFormInfo.value.forceModify.value;
            }
            // snmpv3密码
            if (this.changedData.snmpPwd1) {
                param.SNMPV3Password = this.userFormInfo.value.snmpPwd1;
            }
            if (this.changedData.authority) {
                param.SnmpV3AuthProtocol = this.userFormInfo.value.authority && this.userFormInfo.value.authority.id;
            }
            if (this.changedData.encryp) {
                param.SnmpV3PrivProtocol = this.userFormInfo.value.encryp && this.userFormInfo.value.encryp.id;
            }
        }
        const method = this.row === undefined ? 'addUser' : 'editUser';
        this.disabled = true;
        this.loading.state.next(true);
        this.saveState = true;
        this.localUserService[method](param, this.userLoginPwd).subscribe({
            next: (res: ICustError) => {
                this.loading.state.next(false);
                this.saveState = false;
                const isSelf = this.row?.id.toString() === this.userInfo.userId.toString();
                if (res.type === OperatorResult.SUCCESS) {
                    this.alertService.eventEmit.emit({
                        type: 'success',
                        label: 'COMMON_SUCCESS'
                    });

                    // 若涉及修改用户自身的账户信息（用户名，密码，角色）
                    if (isSelf && (this.changedData.userName || this.changedData.userPwd1 || this.changedData.roleId)) {
                        this.timeoutService.error401.next('SessionChanged');
                    }
                    this.destoryed = true;
                    return;
                }

                const errorMsg = res.type === OperatorResult.SOMEFAILED ? 'USER_EDIT_PARTIALLY_FAILED' : 'COMMON_FAILED';
                this.alertService.eventEmit.emit({
                    type: 'error',
                    label: errorMsg
                });
                this.userLoginPwd = '';
                this.reInit.next(true);
                this.errorHandler(res.errors);
            },
            error: (error: { type: string; errors: { errorId: string; relatedProp: string }[] } | null) => {
                this.userLoginPwd = '';
                this.saveState = false;
                this.errorHandler(error.errors);
                if (error.errors[0].errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                    return;
                } else if (error.errors[0].errorId) {
                    this.alertService.eventEmit.emit(errorTip(error.errors[0].errorId, this.translate));
                    return;
                }
                this.alertService.eventEmit.emit({
                    type: 'error',
                    label: 'COMMON_FAILED'
                });
            }
        });
    }

    // 保存按钮点击方法
    public save(): void {
      // 如果被禁用，则不做处理
      if (this.disabled) {
        return;
      }

      let message = '';
      // 检查密码是否为空，不为空则需要验证密码是否有重复
      const pwd = this.userFormInfo.get('userPwd1').value;
      if (pwd !== '' && pwd !== null) {
        if (pwd.length < 8) {
          message = this.translate.instant('USER_PWD_INSUFFICIENT');
        }
        const count = computeRepeatTimes(pwd);
        if (count > 1) {
          if (message !== '') {
            message = this.translate.instant('USER_PWD_INSUFFICIENT_REPEAT');
          } else {
            message = this.translate.instant('USER_PWD_REPEAT');
          }
        }
      }
      message = message || this.translate.instant('COMMON_ASK_OK');
      const msgTips = [
        this.translate.instant('BMC_LOGIN_TIP'),
        this.translate.instant('SNMP_TIP'),
        this.translate.instant('SSH_TIP'),
        this.translate.instant('IPMI_TIP'),
        this.translate.instant('LOCAL_TIP'),
        this.translate.instant('SFTP_TIP'),
        this.translate.instant('WEB_TIP'),
        this.translate.instant('REDFISH_TIP'),
      ];
      this.tiModal.open(LoginInterfaceTipComponent, {
        id: 'loginInterfaceTip',
        context: {
          msgTips,
        },
        close: () => {
          this.saveData();
        },
        dismiss: (): void => {},
      });
    }

    // 保存时错误处理，处理的错误是用户、密码、v3密码
    private errorHandler(errorMsgArr: { errorId: string; relatedProp: string }[]): void {
        // 用户信息的错误id列表
        const userErrorList: string[] = this.USERNAME_ERRORS;
        // 密码的错误ID列表
        const pwdErrorList: string[] = this.PASSWORD_ERRORS;
        // 用户密码或v3密码错误时，表示是密码还是v3密码错误
        const relatedPops: string[] = ['Password', 'SnmpV3PrivPasswd'];

        // 获取所有的错误属性
        errorMsgArr.forEach((errorItem: { errorId: string; relatedProp: string }) => {
            if (errorItem.errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                this.secondError = true;
            }

            // 用户名信息错误
            if (userErrorList.indexOf(errorItem.errorId) > -1) {
                this.addErrorValidation('userName', errorItem.errorId);
                this.scrollTop = 0;
            }

            // 密码或v3密码信息错误
            if (pwdErrorList.indexOf(errorItem.errorId) > -1) {
                // 用户密码保存失败
                if (relatedPops[0].indexOf(errorItem.relatedProp) > -1) {
                    this.addErrorValidation('userPwd1', errorItem.errorId);
                    this.scrollTop = 0;
                }

                // v3密码保存失败
                if (relatedPops[1].indexOf(errorItem.relatedProp) > -1) {
                    this.addErrorValidation('snmpPwd1', errorItem.errorId);
                }

                if (errorItem.errorId.indexOf(this.ADD_IPMI_ERROR_PREFIX) > -1) {
                    this.alertService.eventEmit.emit({
                        type: 'error',
                        label: this.translate.instant('USER_IPMI_SAVE_ERROR')
                    });
                }
            }
        });
    }

    // 给对应的属性框附加校验错误的提示,摒弃tiny2时的添加一个校验规则的做法，而是直接在输入框下添加提示
    private addErrorValidation(formControlName: string, errorId: string): void {
        const errorMsg = this.translate.instant(errorId);
        this.pageError[formControlName].asyncError.show = true;
        this.pageError[formControlName].asyncError.message = errorMsg.attrMessage || errorMsg.errorMessage;
    }
    // 关闭半屏弹窗
    public closeModal(param: { reload: boolean }): void {
        this.close.next(param.reload);
    }

    // 跳转到安全配置登录规则
    public jumpToSec(): void {
        this.commonService.jumpToSec();
    }

    // 重置用户密码和V3密码的校验规则（用于提示强制密码重置功能）
    private resetForceValidator(target: string, type: 'add' | 'delete'): void {
        const control = this.userFormInfo.controls[target];
        const msg = ((target === 'snmpPwd1') ? 'SNMP_RESET_ENCRY_PASSWORD' : 'SNMP_RESET_PASSWORD');
        const validatorChecked = [
            MultVaild.forceResetPassword(this.translate.instant(msg)),
            TiValidators.rangeSize(this.minPwdLength, PasswordLength.MAX),
            MultVaild.pattern(),
            MultVaild.specialChartAndSpace(),
            TiValidators.notEqualPosRev(() => this.userFormInfo.get('userName'))
        ];
        const validatorNoChecked = [
            MultVaild.forceResetPassword(this.translate.instant(msg)),
            TiValidators.rangeSize(1, 20)
        ];
        const finallyValid = this.pwdCheck ? validatorChecked : validatorNoChecked;
        if (type === 'delete') {
            finallyValid.shift();
        }

        control.clearValidators();
        control.setValidators([...finallyValid]);
        control.markAsTouched();
        control.updateValueAndValidity();
    }

    // 强制触发与取消触发（修重置密码和snmpv3密码）
    private forceUpdatePwd(force: boolean, reason: 'ipmi' | 'author'): void {
        const targets = reason === 'ipmi' ? ['userPwd1'] : ['userPwd1', 'snmpPwd1'];
        targets.forEach((target) => {
            this.pageError[target].forceReset[reason] = force;
        });

        if (force) {
            // 如果是强制重置密码
            if (reason === 'ipmi') {
                this.resetForceValidator('userPwd1', 'add');
            } else {
                this.resetForceValidator('userPwd1', 'add');
                this.resetForceValidator('snmpPwd1', 'add');
            }
        } else {
            // 如果是取消强制重置密码
            if (reason === 'ipmi') {
                if (!this.pageError.userPwd1.forceReset.author) {
                    this.resetForceValidator('userPwd1', 'delete');
                }
            } else {
                // 如果是鉴权算法导致的要删除强制密码提示的,1 可以直接删除snmpv3的强制提示。2需要判断IPMI是否也存在强制重置
                this.resetForceValidator('snmpPwd1', 'delete');
                if (!this.pageError.userPwd1.forceReset.ipmi) {
                    this.resetForceValidator('userPwd1', 'delete');
                }
            }
        }
    }

    // 加密算法变更时，更新鉴权算法的列表
    private updateAuthorityList(value: string): void {
        let curValue = this.userFormInfo.controls.authority.value;
        // 当选中项的id是 AES256 时
        if (value === this.encrypOptions[2].id) {
            this.authorityOptions = [
                { label: 'SHA256', id: 'SHA256' },
                { label: 'SHA384', id: 'SHA384' },
                { label: 'SHA512', id: 'SHA512' }
            ];
        } else {
            this.authorityOptions = [
                { label: 'MD5', id: 'MD5' },
                { label: 'SHA', id: 'SHA' },
                { label: 'SHA1', id: 'SHA1', disabled: true },
                { label: 'SHA256', id: 'SHA256' },
                { label: 'SHA384', id: 'SHA384' },
                { label: 'SHA512', id: 'SHA512' }
            ];
        }
        // 如果当前值在修改后的列表中，则继续选中，如果不在，则选中修改后的列表的第一项
        const isExists = this.authorityOptions.filter((item: { label: string, id: string }) => {
            if (item.id === curValue.id) {
                curValue = item;
                return true;
            }
            return false;
        }).length > 0;

        if (!isExists) {
            curValue = this.authorityOptions[0];
        }
        this.userFormInfo.controls.authority.patchValue(curValue);
    }

    // 计算后端校验密码错误信息和强制修改密码提示信息是否显示
    public computePasswordState(target: 'userPwd1' | 'snmpPwd1'): boolean {
        // 显示逻辑 1. 优先显示强制修改密码。 2.该控件上的异步信息要支持显示。
        if (this.pageError[target].forceReset.author || this.pageError[target].forceReset.ipmi) {
            const value = this.userFormInfo.controls[target].value;
            if (value.length > 0) {
                return this.pageError[target].asyncError.show;
            }
            return false;
        } else {
            return this.pageError[target].asyncError.show;
        }
    }
}
