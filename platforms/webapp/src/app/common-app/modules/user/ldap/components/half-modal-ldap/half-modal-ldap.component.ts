import { Component, OnInit, ViewChild, ElementRef, Input, Output, EventEmitter } from '@angular/core';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormGroup, UntypedFormBuilder, Validators } from '@angular/forms';
import { ILoginRule } from '../../../dataType/data.interface';
import { LdapService } from '../../ldap.service';
import { TiValidationConfig, TiModalService } from '@cloud/tiny3';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import * as utils from 'src/app/common-app/utils';
import { Router } from '@angular/router';
import { timer } from 'rxjs';
import { FormControlOperators } from '../../../dataType/data.model';
import {
    AlertMessageService,
    LoadingService,
    SystemLockService,
    UserInfoService,
    CommonService
} from 'src/app/common-app/service';
import { LoginInterfaceTipComponent } from '../../../localuser/component/login-interface-tip/login-interface-tip.component';

@Component({
    selector: 'app-half-modal-ldap',
    templateUrl: './half-modal-ldap.component.html',
    styleUrls: ['./half-modal-ldap.component.scss']
})
export class HalfModalLdapComponent implements OnInit {
    // 查询半屏组件，获取到该实例
    @ViewChild(TpHalfmodalComponent, { static: true }) modal: TpHalfmodalComponent;
    @ViewChild('userParams', { read: ElementRef, static: true }) userParams: ElementRef;
    @Input() row;
    @Input() id: number;
    @Input() halfTitle: string;
    @Output() close: EventEmitter<any> = new EventEmitter<any>();

    public interfaceList = ['Web', 'Redfish', 'SSH'];
    public interfaceChecked = [];
    public userLoginPwd = '';
    public formGroup: UntypedFormGroup;
    public disabled: boolean = true;
    public unposition: boolean = false;
    private baseFormData;
    private baseInterface = [];
    private baseRules = [];
    private changedData = {};
    public saveError: boolean = false;
    public systemLocked: boolean = false;
    public loginWithoutPassword: boolean = this.userInfo.loginWithoutPassword;
    public destoryed: boolean = false;

    // 登录规则列表和登录规则选中列表
    public ruleList = [];
    public ruleChecked = [];

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
            id: 'Common User',
            label: this.translate.instant('COMMON_ROLE_COMMONUSER')
        },
        {
            id: 'Custom Role 1',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE1')
        },
        {
            id: 'Custom Role 2',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE2')
        },
        {
            id: 'Custom Role 3',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE3')
        },
        {
            id: 'Custom Role 4',
            label: this.translate.instant('COMMON_ROLE_CUSTOMROLE4')
        }
    ];

    // 组文件夹校验
    public folderValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            'outOfRange': this.translate.instant('VALIDATOR_OUTOF_RANGE')
        }
    };

    // 组名校验
    public groupNameValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            'required': this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            'notAllWhiteSpace': this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            'outOfRange': this.translate.instant('VALIDATOR_OUTOF_RANGE'),
        }
    };

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private alertService: AlertMessageService,
        private ldapService: LdapService,
        private loading: LoadingService,
        private lockService: SystemLockService,
        private router: Router,
        private userInfo: UserInfoService,
        private tiModal: TiModalService,
        private commonService: CommonService
    ) {

    }

    ngOnInit() {
        // 系统锁定监听
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
            const method = res ? FormControlOperators.DISABLE : FormControlOperators.ENABLE;
            const controls = this.formGroup.controls;
            Object.keys(controls).forEach(key => {
                // 确定是否需要修改状态
                const curState = controls[key].disabled ? FormControlOperators.DISABLE : FormControlOperators.ENABLE;
                if (curState !== method) {
                    this.formGroup.controls[key][method]();
                }
            });
        });

        this.initForm();
        this.getRules();
        let role = this.roleList.filter((item) => item.id === this.row.groupRole)[0];
        role = role || this.roleList[2];

        this.formGroup.patchValue({
            'groupName': this.row.groupName,
            'groupDomain': this.row.groupFolder,
            'groupRole': role
        });

        // 保存基础数据
        this.baseInterface = [...this.row.groupLoginInterface];
        this.baseFormData = {
            groupName: this.formGroup.get('groupName').value,
            groupDomain: this.formGroup.get('groupDomain').value,
            groupRole: this.formGroup.get('groupRole').value
        };

        // 赋值登录接口，登录规则是在getRules方法里面设置
        this.interfaceChecked = [...this.row.groupLoginInterface];
    }

    // 初始化formGroup
    private initForm() {
        this.formGroup = this.fb.group({
            'groupName': ['', [Validators.required, MultVaild.notAllWhiteSpace(), MultVaild.maxLength255()]],
            'groupDomain': ['', [MultVaild.maxLength255()]],
            'groupRole': [this.roleList[0]]
        });
    }

    // 表单输入框变化
    public checkValueChange(controlName) {
        const controls = this.formGroup.controls;
        const value = controls[controlName].value;
        if (this.baseFormData[controlName] !== value) {
            this.changedData[controlName] = true;
        } else {
            delete this.changedData[controlName];
        }
        this.computedState();
    }

    // 接口checkbox变更
    public interfaceChange(key) {
        const flag = JSON.stringify(this.interfaceChecked.sort()) === JSON.stringify(this.baseInterface.sort());
        if (flag) {
            delete this.changedData[key];
        } else {
            this.changedData[key] = true;
        }
        this.computedState();
    }

    // 规则checkbox变更
    public ruleChange(key) {
        const flag = JSON.stringify(this.ruleChecked.sort()) === JSON.stringify(this.baseRules.sort());
        if (flag) {
            delete this.changedData[key];
        } else {
            this.changedData[key] = true;
        }
        this.computedState();
    }

    // 禁止事件冒泡，防止点击其他空白处导致半屏组件意外关闭
    public forbidCubble(ev) {
        if (ev.target.id === 'backgroundLayer') {
            ev.stopPropagation();
            ev.preventDefault();
        }
    }

    // 获取登录规则
    private getRules() {
        this.loading.state.next(true);
        this.ldapService.getRules().then((rules: ILoginRule[]) => {
            this.ruleList = rules;
            this.ruleList.forEach((item, index) => {
                if (index > 0) {
                    item['active'] = false;
                } else {
                    item['active'] = true;
                }
            });

            this.ruleChecked = [];
            this.ruleList.forEach((item: ILoginRule) => {
                if (this.row.groupLoginRule.indexOf(item.memberId) > -1) {
                    this.ruleChecked.push(item);
                }
            });

            this.baseRules = [...this.ruleChecked];

            // 绑定resize事件，计算保存按钮的位置
            this.loading.state.next(false);
        });
    }

    // 显示登录规则详情
    public showDetails(item) {
        item.active = !item.active;
    }


    // 按回车键时，保存数据
    public enter(ev) {
        if (ev.keyCode === 13) {
            this.save();
        }
    }

    // 输入密码时，删除账户密码锁定的提示
    public pwdChange() {
        this.saveError = false;
        this.computedState();
    }

    // 保存数据
    public save() {
        if (this.disabled) {
            return;
        }

        if (!this.loginWithoutPassword && this.userLoginPwd === '') {
            return;
        }

        // LDAP总共也是支持5个组，保存时，需在当前组的位置前插入空的数据
        const ldapGroups = { LdapGroup: [] };
        const param = {
            Folder: this.formGroup.get('groupDomain').value.trim(),
            LoginInterface: this.interfaceChecked,
            LoginRule: this.ruleChecked.map((item) => item.memberId).sort(),
            Name: this.formGroup.get('groupName').value.trim(),
            RoleID: this.formGroup.get('groupRole').value.id
        };
        // 如果是编辑操作
        if (this.row.groupName !== '') {
            if (!this.changedData['groupName']) {
                delete param['Name'];
            }
            if (!this.changedData['groupDomain']) {
                delete param['Folder'];
            }
            if (!this.changedData['groupRole']) {
                delete param['RoleID'];
            }
            if (!this.changedData['rule']) {
                delete param['LoginRule'];
            }
            if (!this.changedData['interface']) {
                delete param['LoginInterface'];
            }
        }
        let memberId = this.row.memberId;
        while (memberId > 0) {
            ldapGroups.LdapGroup.push({});
            memberId--;
        }
        ldapGroups.LdapGroup.push(param);

        const msgTips = [
            this.translate.instant('BMC_LOGIN_TIP'),
            this.translate.instant('WEB_TIP'),
            this.translate.instant('REDFISH_TIP'),
            this.translate.instant('SSH_TIP'),
          ];
        this.tiModal.open(LoginInterfaceTipComponent, {
            id: 'loginInterfaceTip',
            context: {
              msgTips,
            },
            close: () => {
                this.loading.state.next(true);
                this.ldapService.saveGroup(this.id, ldapGroups, this.userLoginPwd).subscribe({
                    next: (res) => {
                        this.loading.state.next(false);
                        this.destoryed = true;
                        this.alertService.eventEmit.emit({
                            type: 'success',
                            label: 'COMMON_SUCCESS',
                    });
                },
                error: (error) => {
                    this.userLoginPwd = '';
                    const errorId = utils.getMessageId(error.error)[0].errorId;
                    if (errorId === 'ReauthFailed') {
                        this.saveError = true;
                    }
                }});
            },
            dismiss: (): void => {},
        });
    }

    // 计算保存按钮的状态
    private computedState() {
        timer(10).subscribe(() => {
            // 表单是否是有效变化
            const formState = this.formGroup.valid && Object.keys(this.changedData).length > 0;
            // 密码是否是有效的（包括后端验证的结果）
            const pwdState = this.loginWithoutPassword ? true : (this.saveError === false && this.userLoginPwd.length > 0);
            // 只有当表单是有效变化和密码是有效密码时，按钮才高亮
            this.disabled = !(formState && pwdState);
        });
    }

    // 关闭半屏弹窗
    public closeModal(param: { reload: boolean }): void {
        this.close.next(param.reload);
    }

    // 跳转到安全配置登录规则
    public jumpToSec() {
        this.commonService.jumpToSec();
    }
}

