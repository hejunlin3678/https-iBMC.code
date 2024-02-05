import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormGroup, UntypedFormBuilder, Validators } from '@angular/forms';
import { KerberosService } from '../../kerberos.service';
import { ILoginRule, IGroupItem, IKerberosSaveData } from '../../../dataType/data.interface';
import { getMessageId } from 'src/app/common-app/utils';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import { TiValidationConfig } from '@cloud/tiny3/directives/validation/TiValidationModule';
import { Operators } from '../../../dataType/data.model';
import { timer } from 'rxjs/internal/observable/timer';
import { TiModalService } from '@cloud/tiny3';
import { AlertMessageService, SystemLockService, CommonService, UserInfoService } from 'src/app/common-app/service';
import { LoginInterfaceTipComponent } from '../../../localuser/component/login-interface-tip/login-interface-tip.component';

@Component({
    selector: 'app-half-modal-kerberos',
    templateUrl: './half-modal-kerberos.component.html',
    styleUrls: ['./half-modal-kerberos.component.scss']
})
export class HalfModalKerberosComponent implements OnInit {
    @Input() type: 'add' | 'edit';
    @Input() row: IGroupItem;
    @Input() url: string;
    @Input() halfTitle: string;
    @Output() close: EventEmitter<boolean> = new EventEmitter<boolean>();

    public interfaceList: string[] = ['Web', 'Redfish'];
    public interfaceChecked: string[] = [];
    public userLoginPwd: string = '';
    public formGroup: UntypedFormGroup = null;
    public disabled: boolean = true;
    public saveError: boolean = false;
    public systemLocked: boolean = false;
    private baseFormData: { [key: string]: any } = null;
    private baseInterface: string[] = [];
    private baseRules: ILoginRule[] = [];
    private changedData: { [key: string]: any } = {};
    public loginWithoutPassword: boolean = this.userInfo.loginWithoutPassword;
    public destoryed: boolean = false;

    // 登录规则列表和登录规则选中列表
    public ruleList: ILoginRule[] = [];
    public ruleChecked: ILoginRule[] = [];

    // 角色列表, 如果传入权限为 No Access, 则替换为Common User
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

    // 组名校验
    public groupNameValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            'required': this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            'outOfRange': this.translate.instant('VALIDATOR_OUTOF_RANGE'),
            'notAllWhiteSpace': this.translate.instant('VALIDTOR_FORMAT_ERROR')
        }
    };

    // 组文件夹校验
    public groupFolderValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            'outOfRange': this.translate.instant('VALIDATOR_OUTOF_RANGE')
        }
    };

    // 组SID校验
    public groupSidValidation: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            'required': this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            'notIncludeSpace': this.translate.instant('VALIDTOR_FORMAT_ERROR'),
            'outOfRange': this.translate.instant('VALIDATOR_OUTOF_RANGE')
        }
    };

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private kerberosService: KerberosService,
        private alertService: AlertMessageService,
        private commonService: CommonService,
        private lockService: SystemLockService,
        private userInfo: UserInfoService,
        private tiModal: TiModalService
    ) { }

    ngOnInit(): void {
        // 系统锁定功能
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe((res: boolean) => {
            this.systemLocked = res;
            const method = res ? Operators.DISABLE : Operators.ENABLE;
            Object.keys(this.formGroup.controls).forEach((key: string) => {
                this.formGroup.controls[key][method]();
            });
        });

        // 初始化表单组件
        this.formGroup = this.fb.group({
            'groupName': ['', [Validators.required, MultVaild.maxLength255(), MultVaild.notAllWhiteSpace()]],
            'groupDomain': ['', [MultVaild.maxLength255()]],
            'groupSID': ['', [Validators.required, MultVaild.notIncludeSpace(), MultVaild.maxLength255()]],
            'groupRole': [this.roleList[2]]
        });

        // 如果是编辑则开始设置表单值
        this.formGroup.patchValue({ 'groupName': this.row.groupName });
        this.formGroup.patchValue({ 'groupSID': this.row.groupSID });
        const role = this.roleList.filter((item: { id: string; label: string }) => {
            if (item.id === this.row.groupRole) {
                return item;
            }
        })[0];
        this.formGroup.patchValue({ 'groupRole': role });
        this.formGroup.patchValue({ 'groupDomain': this.row.groupDomain });

        // 采用map写法主要是复制操作，避免引用同一对象
        this.interfaceChecked = this.row && [...this.row.groupLoginInterface];
        // 开始保存基础数据，输入框和下拉框，登
        this.baseFormData = {
            groupName: this.row.groupName,
            groupDomain: this.row.groupDomain,
            groupSID: this.row.groupSID,
            groupRole: role
        };
        this.baseRules = [...this.ruleChecked.sort()];
        this.baseInterface = [...this.interfaceChecked.sort()];

        // 查询登录规则
        this.getLoginRules();
    }

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

    public checkCheckBoxChange(name: 'groupRule' | 'groupInterface') {
        const compareOrigin = name === 'groupRule' ? this.baseRules : this.baseInterface;
        const compareTarget = name === 'groupRule' ? this.ruleChecked : this.interfaceChecked;
        const flag = JSON.stringify(compareOrigin.sort()) === JSON.stringify(compareTarget.sort());
        if (flag) {
            delete this.changedData[name];
        } else {
            this.changedData[name] = true;
        }
        this.computedState();
    }

    public getLoginRules() {
        // 获取登录规则
        this.kerberosService.getRules().then((rules: ILoginRule[]) => {
            // 设置规则相关信息
            this.ruleList = rules;
            this.ruleList.forEach((item, index) => {
                if (index > 0) {
                    item.active = false;
                } else {
                    item.active = true;
                }
            });

            this.ruleChecked = [];
            this.ruleList.forEach((item: ILoginRule) => {
                if (this.row && this.row.groupLoginRule.indexOf(item.memberId) > -1) {
                    this.ruleChecked.push(item);
                }
            });

            // 保存基础数据
            this.baseRules = [...this.ruleChecked.sort()];
        });
    }

    // 计算保存按钮的状态
    public computedState(): void {
        timer(10).subscribe(() => {
            // 表单是否是有效变化
            const formState = this.formGroup.valid && Object.keys(this.changedData).length > 0;
            // 密码是否是有效的（包括后端验证的结果）
            const pwdState = this.loginWithoutPassword ? true : (this.saveError === false && this.userLoginPwd.length > 0);
            // 只有当表单是有效变化和密码是有效密码时，按钮才高亮
            this.disabled = !(formState && pwdState);
        });
    }

    // 显示登录规则详情
    public showDetails(item: ILoginRule): void {
        item.active = !item.active;
    }

    // 保存数据
    public save(): void {
        if (this.systemLocked) {
            return;
        }
        const params = this.getChangedData();
        const msgTips = [
            this.translate.instant('BMC_LOGIN_TIP'),
            this.translate.instant('WEB_TIP'),
            this.translate.instant('REDFISH_TIP'),
          ];
        this.tiModal.open(LoginInterfaceTipComponent, {
            id: 'loginInterfaceTip',
            context: {
              msgTips,
            },
            close: () => {
                this.kerberosService.saveGroup(params, this.userLoginPwd).subscribe({
                    next: (res) => {
                        this.destoryed = true;
                        this.alertService.eventEmit.emit({
                            type: 'success',
                            label: 'COMMON_SUCCESS',
                        });
                    },
                    error: (error) => {
                        this.userLoginPwd = '';
                        const errorId = getMessageId(error.error)[0].errorId;
                        if (errorId === 'ReauthFailed') {
                            this.saveError = true;
                        }
                    },
                });
            },
            dismiss: (): void => {},
        });
    }

    // 编辑,新增用户时，组装变化的数据
    private getChangedData(): { Group: IKerberosSaveData[] } {
        const param = { Group: [] };
        const temp: IKerberosSaveData = {};
        let memberId = this.row.memberId;
        while (memberId > 0) {
            param.Group.push({});
            memberId--;
        }
        // 组装checkbox数据和表单数据
        if (this.type === Operators.ADD) {
            temp.LoginRule = this.ruleChecked.map((item) => item.memberId).sort();
            temp.LoginInterface = this.interfaceChecked.sort();
            temp.Name = this.formGroup.get('groupName').value;
            temp.Domain = this.formGroup.get('groupDomain').value;
            temp.SID = this.formGroup.get('groupSID').value;
            temp.RoleID = this.formGroup.get('groupRole').value.id;
        } else {
            if (this.changedData.groupSID) {
                temp.SID = this.formGroup.get('groupSID').value;
            }
            if (this.changedData.groupRule) {
                temp.LoginRule = this.ruleChecked.map((item) => item.memberId).sort();
            }
            if (this.changedData.groupInterface) {
                temp.LoginInterface = this.interfaceChecked.sort();
            }

            if (this.changedData.groupName) {
                temp.Name = this.formGroup.get('groupName').value;
            }
            if (this.changedData.groupDomain) {
                temp.Domain = this.formGroup.get('groupDomain').value;
            }

            if (this.changedData.groupRole) {
                temp.RoleID = this.formGroup.get('groupRole').value.id;
            }
        }
        param.Group.push(temp);
        return param;
    }

    // 按回车键时，保存数据
    public enter(ev: KeyboardEvent): void {
        // tslint:disable-next-line: deprecation
        if (ev.keyCode === 13) {
            this.save();
        }
    }

    // 输入密码时，删除账户密码锁定的提示
    public pwdChange(): void {
        this.saveError = false;
        this.computedState();
    }

    // 关闭半屏弹窗
    public closeModal(param: { reload: boolean }): void {
        this.close.next(param.reload);
    }

    // 跳转到安全配置登录规则
    public jumpToSec(): void {
        this.commonService.jumpToSec();
    }
}

