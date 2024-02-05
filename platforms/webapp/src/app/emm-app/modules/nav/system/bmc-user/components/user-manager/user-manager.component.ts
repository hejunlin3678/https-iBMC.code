import { Component, OnInit, Output, EventEmitter, Input } from '@angular/core';
import { UntypedFormBuilder, UntypedFormGroup, ValidationErrors, Validators } from '@angular/forms';
import { DomSanitizer } from '@angular/platform-browser';
import { TiMessageService, TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { timer } from 'rxjs/internal/observable/timer';
import { PasswordLength } from 'src/app/common-app/models';
import { ICustError } from 'src/app/common-app/modules/user/dataType/data.interface';
import { OperatorResult, Operators, SecondAuthorityFail } from 'src/app/common-app/modules/user/dataType/data.model';
import { AlertMessageService, GlobalDataService, LoadingService, TimeoutService, UserInfoService } from 'src/app/common-app/service';
import { computeRepeatTimes, formatEntry } from 'src/app/common-app/utils/common';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import { IBlade, IBmcUser } from '../model';
import { UsersService } from '../users.service';

@Component({
    selector: 'app-user-manager',
    templateUrl: './user-manager.component.html',
    styleUrls: ['./user-manager.component.scss']
})
export class UserManagerComponent implements OnInit {
    @Output() close: EventEmitter<boolean> = new EventEmitter<boolean>();
    @Output() reInit: EventEmitter<boolean> = new EventEmitter<boolean>();
    @Input() type: string = '';
    @Input() bladeList: IBlade;
    @Input() row: IBmcUser = null;

    public belongList: IBlade[];
    public disabled: boolean = true;
    private saveState: boolean = false;
    public halfTitle: string = '';
    public destoryed: boolean = false;
    public loginWithoutPassword: boolean = this.userInfo.loginWithoutPassword;
    public userFormInfo: UntypedFormGroup;
    public userLoginPwd: string = '';
    public secondError: boolean = false;
    public systemLocked: boolean = false;
    public isLastAdmin: boolean = false;
    public scrollTop: number = null;
    private initFinished: boolean = false;
    private baseFormInfo: { [key: string]: any } = {};
    private changedData: { [key: string]: any } = {};

    public roleList = [
        { id: 'Administrator', label: this.translate.instant('USERINFO_ROLE_ADMINISTRATOR') },
        { id: 'Operator', label: this.translate.instant('USERINFO_ROLE_OPERATOR') },
        { id: 'Commonuser', label: this.translate.instant('USERINFO_ROLE_COMMONUSER') },
        { id: 'CustomRole1', label: this.translate.instant('COMMON_ROLE_CUSTOMROLE1')},
        { id: 'CustomRole2', label: this.translate.instant('COMMON_ROLE_CUSTOMROLE2')},
        { id: 'CustomRole3', label: this.translate.instant('COMMON_ROLE_CUSTOMROLE3')},
        { id: 'CustomRole4', label: this.translate.instant('COMMON_ROLE_CUSTOMROLE4')},
        { id: 'Noaccess', label: this.translate.instant('COMMON_ROLE_NOACCESS')}
    ];

    public pageError: any = {
        userName: {
            asyncError: {
                show: false,
                message: ''
            },
        },
        userPwd1: {
            asyncError: {
                show: false,
                message: ''
            }
        }
    };

    // 新增或修改用户名时，以下几种错误应该在页面的输入框后做提示
    private USERNAME_ERRORS: string[] = [
        'ResourceAlreadyExists',
        'ConflictWithLinuxDefaultUser',
        'UserIsLoggingIn',
        'UserNameIsRestricted',
        'InvalidUserName'
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

    // 用户名校验规则
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
    // 密码的校验提示
    public userPasswordValidation: TiValidationConfig;

    // 密码校验提示配置
    public userPasswordChecked: TiValidationConfig = {
        type: 'blur',
        errorMessage: {},
        tip: formatEntry(this.translate.instant('VALIDATOR_PWD_STANDARD'), [PasswordLength.MIN, PasswordLength.MAX]),
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

    constructor(
        private userInfo: UserInfoService,
        private fb: UntypedFormBuilder,
        private loading: LoadingService,
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private userService: UsersService,
        private globalData: GlobalDataService,
        private domSanitizer: DomSanitizer,
        private alertService: AlertMessageService
    ) { }

    ngOnInit(): void {
        this.belongList = [this.bladeList];
        const userPwd2Validator: ValidationErrors[] = [MultVaild.equalTo('userPwd1')];
        if (this.type === 'add') {
            this.halfTitle = 'USER_TEMPLATE_TITLE_ADD';
            userPwd2Validator.unshift(TiValidators.required);
        } else {
            this.halfTitle = 'USER_TEMPLATE_TITLE_EDIT';
        }

        this.userFormInfo = this.fb.group({
            'belong': [this.bladeList],
            'userName': [this.row ? this.row.userName : '', [
                Validators.required,
                Validators.pattern(/^[a-zA-Z0-9_\-`~!@#$^*()=+|{};\[\].?]{1,16}$/),
                Validators.pattern(/^[^#+-].*$/),
                MultVaild.notPoints()
            ]],
            'userPwd1': [''],
            'userPwd2': ['', userPwd2Validator],
            'roleId': [this.roleList.filter(item => this.row?.role === item.id)?.[0] || this.roleList[0]]
        });

        this.setPasswordValidator(this.type);
        this.bindDataListen();
        this.saveBaseData();

        timer(100).subscribe(() => {
            this.initFinished = true;
        });
    }

    // 保存基础数据
    private saveBaseData(): void {
        // 设置基础原始数据
        this.baseFormInfo = JSON.parse(JSON.stringify(this.userFormInfo.value));
    }

    public clearAsyncError(target: string): void {
        this.pageError[target].asyncError.show = false;
    }

    public save() {
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
        const tMsg = this.domSanitizer.bypassSecurityTrustHtml(`<span id="confirmText" class="font-14">${message}</span>`);
        this.tiMessage.open({
            id: 'secondConfirm',
            type: 'prompt',
            backdrop: false,
            title: this.translate.instant('COMMON_CONFIRM'),
            content: tMsg,
            okButton: {
                text: this.translate.instant('COMMON_OK'),
                autofocus: false
            },
            close: () => {
                if (this.type === Operators.ADD) {
                    // 新增用户
                    this.addUser();
                } else {
                    this.editUser();
                }
            }
        });
    }

    // 添加新用户
    private addUser() {
        const controls = this.userFormInfo.controls;
        const param: any = {
            Password: controls['userPwd1'].value,
            RoleId: controls['roleId'].value.id,
            Slot: controls['belong'].value.id,
            UserName: controls['userName'].value
        };
        this.disabled = true;
        this.loading.state.next(true);
        this.userService.addBmcUser(param, this.userLoginPwd).subscribe({
            next: (res) => {
                this.loading.state.next(false);
                this.alertService.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });
                this.closeModal({ reload: true });
            },
            error: (error: ICustError) => {
                this.loading.state.next(false);
                this.saveState = false;
                this.userLoginPwd = '';

                // errorHandler的功能时将用户名或密码的错误放置在输入框的下边
                this.errorHandler(error.errors);
                const errorId = error.errors[0].errorId;
                if ((errorId === SecondAuthorityFail.AUTHORIZATIONFAILED)) {
                    return;
                }

                this.alertService.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            }
        });
    }

    // 编辑用户
    private editUser() {
        const controls = this.userFormInfo.controls;
        // 编辑用户
        const param: any = {};
        if (this.changedData.userName) {
            param.UserName = controls['userName'].value;
        }
        if (this.changedData.roleId) {
            param.RoleId = controls['roleId'].value.id;
        }
        if (this.changedData.userPwd1) {
            param.Password = controls['userPwd1'].value;
        }
        this.disabled = true;
        // 根据用户角色调整参数值（针对无权限和自定义用户编辑）
        if (this.row?.role !== this.userFormInfo.value.roleId.id) {
            param.RoleId = this.userFormInfo.value.roleId.id;
        }
        this.loading.state.next(true);
        this.userService.queryEtag(this.row.url).subscribe(etag => {
            this.userService.editBmcUser(this.row.url, param, this.userLoginPwd, etag).subscribe({
                next: (res: ICustError) => {
                    this.loading.state.next(false);
                    this.saveState = false;
                    if (res.type === OperatorResult.SUCCESS) {
                        this.alertService.eventEmit.emit({
                            type: 'success',
                            label: 'COMMON_SUCCESS'
                        });
                        this.destoryed = true;
                        return;
                    }

                    this.userLoginPwd = '';
                    const errorMsg = res.type === OperatorResult.SOMEFAILED ? 'USER_EDIT_PARTIALLY_FAILED' : 'COMMON_FAILED';
                    this.alertService.eventEmit.emit({
                        type: 'error',
                        label: errorMsg
                    });
                    this.reInit.emit(true);
                    this.errorHandler(res.errors);
                },
                error: (error) => {
                    this.loading.state.next(false);
                    this.saveState = false;
                    this.userLoginPwd = '';

                    // errorHandler的功能时将用户名或密码的错误放置在输入框的下边
                    this.errorHandler(error.errors);
                    const errorId = error.errors[0].errorId;
                    if ((errorId === SecondAuthorityFail.AUTHORIZATIONFAILED)) {
                        return;
                    }

                    this.alertService.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                }
            });
        });
    }

    // 保存时错误处理，处理的错误是用户、密码、v3密码
    private errorHandler(errorMsgArr: { errorId: string; relatedProp: string }[]): void {
        // 用户信息的错误id列表
        const userErrorList: string[] = this.USERNAME_ERRORS;
        // 密码的错误ID列表
        const pwdErrorList: string[] = this.PASSWORD_ERRORS;
        // 用户密码或v3密码错误时，表示是密码还是v3密码错误
        const relatedPops: string[] = ['Password'];

        // 获取所有的错误属性
        errorMsgArr.forEach((errorItem: { errorId: string; relatedProp: string }) => {
            if (errorItem.errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                this.secondError = true;
                this.userLoginPwd = '';
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
            }
        });
    }

    // 给对应的属性框附加校验错误的提示,摒弃tiny2时的添加一个校验规则的做法，而是直接在输入框下添加提示
    private addErrorValidation(formControlName: string, errorId: string): void {
        const errorMsg = this.translate.instant(errorId);
        this.pageError[formControlName].asyncError.show = true;
        this.pageError[formControlName].asyncError.message = errorMsg.attrMessage || errorMsg.errorMessage;
    }

    // 设置密码校验规则和V3密码的提示信息
    private setPasswordValidator(type: string): void {
        const validator = Object.assign({}, this.userPasswordChecked);
        const validatorArr = [
            TiValidators.rangeSize(8, 20),
        ];
        // 添加用户时，密码是必填项
        if (type === 'add') {
            validator.errorMessage.required = '';
            validatorArr.unshift(TiValidators.required);
        }

        this.userFormInfo.root.get('userPwd1').setValidators(validatorArr);
        this.userPasswordValidation = validator;
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

                if (key === 'userPwd1') {
                    // 触发确认密码的校验
                    const pwd2Value = this.userFormInfo.controls.userPwd2.value;
                    this.userFormInfo.controls.userPwd2.setValue(pwd2Value);

                    // 当输入密码长度超出20位时，提示 已达到最大字符限制,因为tip组件不支持动态更新，因此只能触发一次根组件变更检测来被动触发tip组件重新渲染
                    this.userPasswordValidation = this.changeTipOver20(value.length);
                    this.globalData.appDetectChagne.next(true);
                }
                this.computedState();
            });
        });
    }

    // 根据密码的当前长度，返回validator提示对象信息
    private changeTipOver20(length: number): TiValidationConfig {
        let validator: TiValidationConfig = null;
        if (length >= 20) {
            const tempLimitValidator = Object.assign({}, this.userPasswordChecked, {
                tip: this.translate.instant('VALIDATOR_MAX_LIMIT')
            });
            validator = tempLimitValidator;
        } else {
            const tempValidator = Object.assign({}, this.userPasswordChecked, {
                tip: formatEntry(this.translate.instant('VALIDATOR_PWD_STANDARD'), [PasswordLength.MIN, PasswordLength.MAX])
            });
            validator = tempValidator;
        }
        return validator;
    }

    // 计算按钮的状态
    public computedState(): void {
        setTimeout(() => {
            // 表示formControl是否发生了有效的变化
            let formState = Object.keys(this.changedData).length > 0;
            formState = formState && this.userFormInfo.valid;
            // 表示二次认证密码是否是有效的
            const pwdState = this.loginWithoutPassword ? true : (this.userLoginPwd.length > 0 && !this.secondError);

            // 表示是否不存在有异步的错误
            const notExistAsyncError = !this.pageError.userName.asyncError.show && !this.pageError.userPwd1.asyncError.show;
            // 根据用户角色判断formControl是否发生了有效的变化（针对无权限和自定义用户编辑）
            if (this.row?.role !== this.userFormInfo.value.roleId.id) {
                formState = true;
            }
            // 只要3个状态有任意一个为false，保存按钮将被禁用
            this.disabled = !(formState && pwdState && notExistAsyncError && !this.saveState);
        }, 0);
    }

    // 计算后端校验密码错误信息和强制修改密码提示信息是否显示
    public computePasswordState(target: 'userPwd1'): boolean {
        return this.pageError[target].asyncError.show;
    }

    public enter(ev: KeyboardEvent): void {
        // tslint:disable-next-line: deprecation
        if (ev.keyCode === 13) {
            this.save();
        }
    }

    public pwdChange(): void {
        this.secondError = false;
        this.computedState();
    }

    public closeModal(param: { reload: boolean }): void {
        this.close.emit(param.reload);
    }

}
