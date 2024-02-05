import { Component, OnInit, ViewChild, ViewContainerRef, ComponentFactory, ComponentFactoryResolver, ElementRef } from '@angular/core';
import {
    TiTableRowData,
    TiTableSrcData,
    TiTableColumns,
    TiModalService,
    TiMessageService,
    TiModalRef, TiFileInfo,
    TiValidationConfig,
    TiValidators
} from '@cloud/tiny3';
import { LdapService } from './ldap.service';
import { ILdapControllerTab, ILdapGroup, ILoginRule, ILdapControllerItem } from '../dataType/data.interface';
import { UntypedFormBuilder, UntypedFormGroup, Validators } from '@angular/forms';
import {
    LoadingService,
    AlertMessageService,
    UserInfoService,
    ErrortipService,
    SystemLockService,
    TimeoutService
} from 'src/app/common-app/service';
import { HalfModalLdapComponent } from './components/half-modal-ldap/half-modal-ldap.component';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { TranslateService } from '@ngx-translate/core';
import { cutStr, getByte, DOMAIN_NAME, getMessageId } from 'src/app/common-app/utils/common';
import { ILdapGroupRow } from '../dataType/data.interface';
import { timer } from 'rxjs/internal/observable/timer';
import { Operators, SecondAuthorityFail, FileAddFaild } from '../dataType/data.model';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { DomSanitizer } from '@angular/platform-browser';

@Component({
    selector: 'app-ldap',
    templateUrl: './ldap.component.html',
    styleUrls: ['./ldap.component.scss']
})
export class LdapComponent implements OnInit {
    @ViewChild('halfModal', { read: ViewContainerRef, static: true }) halfModal: ViewContainerRef;
    @ViewChild('ldapTitle', { read: ElementRef, static: true }) ldapTitle: ElementRef;

    constructor(
        private ldapService: LdapService,
        private loading: LoadingService,
        private fb: UntypedFormBuilder,
        private modal: TiModalService,
        private alertService: AlertMessageService,
        private resolver: ComponentFactoryResolver,
        private user: UserInfoService,
        private errorTip: ErrortipService,
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private systemLock: SystemLockService,
        private domSanitizer: DomSanitizer,
        private timeoutService: TimeoutService,
    ) { }

    public systemLocked: boolean = this.user.systemLockDownEnabled;
    public ldapSwitchState: boolean = false;
    public certificateVerificationEnabled: boolean = false;
    public saveBtnStatus: boolean = true;
    public validState: boolean = true;
    public ldapForm: UntypedFormGroup;
    public activeController: ILdapControllerTab;
    public groupCount: number = 5;
    public userLoginPwd: string = '';
    public secondError: boolean = false;
    public loginWithoutPassword: boolean = this.user.loginWithoutPassword;

    // 原始数据保存
    private baseLdapFormData: { [key: string]: any } = {};
    private baseCertifyData: { [key: string]: any } = {};
    private changeData: { [key: string]: any } = {};
    private PORTCONTROLNAME: string = 'port';

    // 默认值6个
    public ldapControlls: ILdapControllerTab[] = [];

    public radioList: { key: string; id: string; disable: boolean }[] = [
        {
            key: 'Demand',
            id: 'Demand',
            disable: false
        },
        {
            key: 'Allow',
            id: 'Allow',
            disable: false
        }
    ];
    public selected: string = this.radioList[0].id;

    // 表格相关数据
    public displayed: TiTableRowData[] = [];
    public baseLdapGroupData: TiTableRowData[] = [];
    public srcData: TiTableSrcData = null;
    // 表头
    public columns: TiTableColumns[] = [
        {
            title: 'TABLE_TITLE_NUM',
            width: '5%'
        },
        {
            title: 'TABLE_TITLE_GROUP_NAME',
            width: '10%'
        },
        {
            title: 'TABLE_TITLE_ROLE',
            width: '10%'
        },
        {
            title: 'TABLE_TITLE_LOGIN_INTERFACE',
            width: '15%'
        },
        {
            title: 'TABLE_TITLE_FOLDER',
            width: '15%'
        },
        {
            title: 'TABLE_TITLE_RULE',
            width: '25%'
        },
        {
            title: 'TABLE_TITLE_OPERATOR',
            width: '18%'
        }
    ];

    public hasConfigUsers = this.user.hasPrivileges(['ConfigureUsers']);
    public cerInfo: any = {};
    public uploaderInstance = null;
    public fileName = '';
    public toggleCollspan = false;
    public ldapUpload = {
        url: '',
        filters: [{
            name: 'maxCount',
            params: [1]
        },
        {
            name: 'type',
            params: ['.cer,.pem,.cert,.crt']
        },
        {
            name: 'maxSize',
            params: [1024 * 1024]
        }
        ],
        headers: {
            'From': 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        autoUpload: false,
        method: 'POST',
        alias: 'Content',
        onBeforeSendItems: (fileItem) => {
            if (!this.loginWithoutPassword) {
                fileItem[0].formData = {
                    ReauthKey: window.btoa(this.userLoginPwd)
                };
            }
        },
        onAddItemFailed: (data: { file: TiFileInfo, validResults: string[] }) => {
            const validResultValue = data.validResults[0];
            if (validResultValue === FileAddFaild.MAXSIZE) {
                this.alertService.eventEmit.emit({ type: 'error', label: 'LDAP_FILE_SCOPE' });
            } else if (validResultValue === FileAddFaild.TYPE) {
                this.alertService.eventEmit.emit({ type: 'error', label: 'LDAP_FORMAT' });
            }
        },
        onCompleteItem: (data) => {
            this.userLoginPwd = '';
            const response = data.response;
            this.uploaderInstance.remove();
            this.uploaderInstance = null;
            this.loading.state.next(false);
            if (data.status === 200) {
                this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.initControllers(this.activeController, true);
            } else {
                this.dealUploadError(response, true);
                this.saveBtnStatus = true;
            }
        },
        onRemoveItem: () => {
            this.fileName = '';
            this.saveBtnStatus = true;
            this.baseCertifyData.fileSelected = false;
            this.uploaderInstance = null;
            this.computeState();
        },
        onAddItemSuccess: (fileItem) => {
            fileItem.mouseenter = undefined;
            this.baseCertifyData.fileSelected = true;
            this.uploaderInstance = fileItem;
            this.uploaderInstance.url = `/UI/Rest/AccessMgnt/LDAP/${this.activeController.id}/ImportLDAPCertificate`;
            this.computeState();
        }
    };

    // LDAP域名校验
    public serverValidator: TiValidationConfig;
    public portValidator: TiValidationConfig;
    public domainValidator: TiValidationConfig;
    public bindFlagValidator: TiValidationConfig;
    public bindPwdValidator: TiValidationConfig;
    public userFolderValidator: TiValidationConfig;

    ngOnInit(): void {
        // 初始化 formGroup
        this.ldapForm = this.fb.group({
            'server': [{ value: '', disabled: !this.hasConfigUsers }, [SerAddrValidators.validateIp46AndDomin()]],
            'port': [
                { value: '', disabled: !this.hasConfigUsers },
                [TiValidators.required, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
            ],
            'domain': [{ value: '', disabled: !this.hasConfigUsers }, [Validators.pattern(DOMAIN_NAME)]],
            'bddn': [{ value: '', disabled: !this.hasConfigUsers }],
            'bdpwd': [{ value: '', disabled: !this.hasConfigUsers }],
            'folder': [{ value: '', disabled: !this.hasConfigUsers }],
        });

        // 获取 LDAP使能
        this.loading.state.next(true);
        this.ldapService.getLdapBase().subscribe({
            next: (res: boolean) => {
                this.ldapSwitchState = res;
            },
            error: () => {
                this.loading.state.next(false);
            }
        });

        const temp = [];
        for (let i = 1; i <= 6; i++) {
            const controll: ILdapControllerTab = {
                title: 'COMMON_CONTROLLER',
                id: i,
                active: false,
                onActiveChange: (isActive: boolean) => {
                    this.onActiveChange(isActive);
                }
            };
            if (i === 1) {
                controll.active = true;
            }
            temp.push(controll);
        }
        this.ldapControlls = temp;
        this.activeController = temp[0];
        this.initControllers(temp[0]);
        this.ldapUpload.url = `/UI/Rest/AccessMgnt/LDAP/${this.activeController.id}/ImportLDAPCertificate`;
    }

    // 系统锁定状态变化事件
    public lockStateChange(state: boolean): void {
        const lockState = this.systemLock.getState();
        this.systemLocked = lockState;
        let method = lockState ? Operators.DISABLE : Operators.ENABLE;
        method = this.hasConfigUsers ? method : Operators.DISABLE;
        const controls = this.ldapForm.controls;
        Object.keys(controls).forEach((key: string) => {
            const isEnabled = controls[key].enabled;
            const curKeyState = isEnabled ? Operators.ENABLE : Operators.DISABLE;
            if (curKeyState !== method) {
                controls[key][method]();
            }
        });
    }

    // CRL证书上传成功后，重新加载数据
    public crlUploadSuccess(): void {
        this.initControllers(this.activeController, true);
    }

    // 保存失败时的错误处理
    private dealUploadError(response, initPwd: boolean = false): void {
        if (getMessageId(response).length === 0) {
            this.initControllers(this.activeController);
            return;
        }
        const errorId = getMessageId(response)[0]?.errorId;
        switch (errorId) {
            case SecondAuthorityFail.AUTHORIZATIONFAILED:
                this.secondError = true;
                return;
            case 'NoValidSession':
                this.timeoutService.error401.next(errorId);
                break;
            default:
                const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alertService.eventEmit.emit({ type: 'error', label: errorMessage });
                if (initPwd) {
                    this.userLoginPwd = '';
                }
        }
        this.initControllers(this.activeController);
    }
    private initControllers(control: ILdapControllerTab, reload?: boolean): void {
        if (reload) {
            this.loading.state.next(true);
        }
        this.saveBtnStatus = true;
        this.userLoginPwd = '';
        this.secondError = false;
        this.ldapService.getLdapControllItem(control.id).subscribe({
            next: (res: ILdapControllerItem) => {
                this.cerInfo = res.cerInfo;
                if (this.cerInfo.middle && this.cerInfo.root) {
                    this.toggleCollspan = true;
                }
                this.renderController(res);
                this.resetValidatorMessage();
                this.resetChangeData();
                this.loading.state.next(false);
            },
            error: () => {
                this.loading.state.next(false);
            }
        });
    }

    private renderController(res: ILdapControllerItem): void {
        this.baseLdapGroupData = res.LdapGroups;
        this.displayed = res.LdapGroups.filter((item) => {
            return item.groupName !== '';
        });
        // 设置表单数据
        this.ldapForm.patchValue({
            'server': res.ldapServerAddress,
            'port': res.ldapPort,
            'domain': res.domain,
            'bddn': res.bindDN,
            'bdpwd': res.bindPassword,
            'folder': res.userFolder
        });
        this.certificateVerificationEnabled = res.certificateVerificationEnabled;
        this.selected = res.certificateVerificationLevel;
        this.groupCount = res.LdapGroups.filter((item: ILdapGroup) => {
            return item.groupName !== null && item.groupName !== '';
        }).length;

        // 保存LDAP证书使能、证书校验级别、是否有文件选中的基础数据
        this.baseCertifyData.certificateVerificationEnabled = this.certificateVerificationEnabled;
        this.baseCertifyData.certificateVerificationLevel = this.selected;
        this.baseCertifyData.fileSelected = false;
        // 保存表单基础数据
        this.baseLdapFormData = JSON.parse(JSON.stringify(this.ldapForm.value));
    }

    // 表单的变更事件
    public checkValueChange(controlName: string, maxlength?: number): void {
        const controls = this.ldapForm.controls;
        const value = controls[controlName].value;
        const maxLength = maxlength || 255;
        const cutValue = cutStr(value, maxLength);
        const length = getByte(value);
        // 当字节数大于255时，截取字符
        if (length > 255) {
            this.ldapForm.controls[controlName].patchValue(cutValue);
        }

        if (this.baseLdapFormData[controlName] !== cutValue) {
            this.changeData[controlName] = true;
        } else {
            delete this.changeData[controlName];
        }

        timer(10).subscribe(() => {
            this.computeState();
        });
    }

    // 重置数据变化状态
    private resetChangeData(): void {
        this.changeData = {};
    }

    // 设置校验报错信息
    private resetValidatorMessage(): void {
        // LDAP服务器地址错误提示信息
        this.serverValidator = {
            tip: this.translate.instant('COMMON_IPV4_IPV6_DOMIN'),
            type: 'blur',
            tipPosition: 'right',
            errorMessage: {
                pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR')
            }
        };

        // 端口校验错误信息
        this.portValidator = {
            type: 'blur',
            errorMessage: {
                required: this.translate.instant('VALIDTOR_RANGE'),
                integer: this.translate.instant('VALIDTOR_RANGE'),
                pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR')
            }
        };

        this.domainValidator = {
            tip: this.translate.instant('IBMC_DOMAIN_TIP').replace(/\{\d\}/, '255'),
            type: 'blur',
            errorMessage: {
                pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR')
            }
        };

        this.bindFlagValidator = {
            tip: this.translate.instant('LDAP_BIND_FLAG_TIP'),
            type: 'blur'
        };

        this.bindPwdValidator = {
            tip: this.translate.instant('LDAP_BIND_PASSWORD_TIP'),
            type: 'blur'
        };

        this.userFolderValidator = {
            tip: this.translate.instant('LDAP_USERFOLDER_TIP'),
            tipMaxWidth: '320px',
            errorMessage: {
                inputError: this.translate.instant('VALIDATOR_INPUT_ERROR')
            },
            type: 'blur'
        };
    }

    // controller切换事件,切换一次，该方法会执行2次，一次是从激活到未激活，一次是从未激活到激活
    private onActiveChange(active: boolean) {
        timer(100).subscribe(() => {
            if (active) {
                const activeItem: ILdapControllerTab = this.ldapControlls.filter((item) => {
                    if (item.active) {
                        return true;
                    }
                    return false;
                })[0];
                this.activeController = activeItem;
                if (this.uploaderInstance) {
                    this.uploaderInstance.remove();
                }
                this.initControllers(activeItem, true);
            }
        });
    }

    // 开启关闭LDAP使能
    public switchChange(): void {
        this.loading.state.next(true);
        this.ldapService.switchLdap(this.ldapSwitchState).subscribe({
            next: () => {
                this.alertService.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });
                this.loading.state.next(false);
            },
            error: () => {
                this.alertService.eventEmit.emit({
                    type: 'error',
                    label: 'COMMON_FAILED'
                });
                this.loading.state.next(false);
            }
        });
    }

    // 证书验证使能开关
    public certSwitchChange(): void {
        this.computeState();
    }

    // 证书校验级别
    public certVerifyLevelChange(): void {
        this.computeState();
    }

    /**
     * 保存LDAP数据和文件上传（手动上传）
     * 1. 如果没有文件上传，直接调用保存LDAP参数的接口
     * 2. 如果有文件上传，则同时调用ldap参数接口和文件上传接口（文件上传成功后，在调用导入文件接口）
     */
    public save(): void {
        if (this.saveBtnStatus || this.systemLocked) {
            return;
        }
        this.saveLdap();
    }

    // 保存LDAP数据
    private saveLdap(): void {
        const param: { [key: string]: any } = {};
        // 组合form表单的参数
        Object.keys(this.changeData).forEach((controlName) => {
            let value = this.ldapForm.value[controlName];
            if (controlName === this.PORTCONTROLNAME) {
                value = parseInt(value, 10);
            }
            param[controlName] = value;
        });

        // 添加证书验证参数
        if (this.baseCertifyData.certificateVerificationEnabled !== this.certificateVerificationEnabled) {
            param.certificateVerificationEnabled = this.certificateVerificationEnabled;
        }

        if (this.baseCertifyData.certificateVerificationLevel !== this.selected) {
            param.certificateVerificationLevel = this.selected;
        }
        this.loading.state.next(true);
        this.saveBtnStatus = true;
        if (JSON.stringify(param) === '{}') {
            this.uploaderInstance.upload();
            return;
        }
        this.ldapService.saveLdap(this.activeController.id, param, this.userLoginPwd).subscribe({
            next: () => {
                // 存在选择文件则上传；不存在文件则本次save请求结束
                if (this.uploaderInstance) {
                    this.uploaderInstance.upload();
                } else {
                    this.alertService.eventEmit.emit({
                        type: 'success',
                        label: 'COMMON_SUCCESS'
                    });
                    this.initControllers(this.activeController, true);
                    this.loading.state.next(false);
                }
            },
            error: (error) => {
                this.userLoginPwd = '';
                const response = error?.error;
                this.dealUploadError(response);
                this.loading.state.next(false);
            }
        });
    }


    public addGroup(ev: MouseEvent): void {
        ev.stopPropagation();
        const row = this.baseLdapGroupData.filter((item) => {
            return item.groupName === '';
        })[0];
        // 传入前，前将row的数据做下处理，因为有些row的数据格式有其他多余的项
        const addItem: ILdapGroupRow = {
            memberId: row.memberId,
            groupName: '',
            groupFolder: '',
            groupRole: 'Common User',
            groupLoginRule: [],
            groupLoginInterface: ['Web', 'Redfish', 'SSH']
        };
        this.createComponent(addItem, 'add');
    }

    public editGroup(ev: MouseEvent, row: ILdapGroupRow): void {
        ev.stopPropagation();
        if (this.systemLocked) {
            return;
        }
        this.createComponent(row, 'edit');
    }

    private createComponent(row: ILdapGroupRow, type: 'add' | 'edit') {
        this.halfModal.clear();
        const componentFactory: ComponentFactory<any> = this.resolver.resolveComponentFactory(HalfModalLdapComponent);
        const component = this.halfModal.createComponent(componentFactory);
        component.instance.halfTitle = type === Operators.ADD ? 'COMMON_ADD_GROUP' : 'COMMON_EDIT_GROUP';
        component.instance.row = row;
        component.instance.id = this.activeController.id;

        // 监听关闭事件
        component.instance.close.subscribe((value) => {
            component.destroy();
            setTimeout(() => {
                this.initControllers(this.activeController, true);
            }, 500);
        });
    }

    public deleteGroup(row: ILdapGroupRow): void {
        if (this.systemLocked) {
            return;
        }
        if (this.loginWithoutPassword) {
            const msg = `<span id="confirmText" class="font-14">${this.translate.instant('COMMON_ASK_OK')}</span>`;
            this.tiMessage.open({
                id: 'secondConfirm',
                type: 'prompt',
                backdrop: false,
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.domSanitizer.bypassSecurityTrustHtml(msg),
                okButton: {
                    text: this.translate.instant('COMMON_OK'),
                    autofocus: false
                },
                close: () => {
                    // 组装参数
                    const param: { LdapGroup: any[] } = { LdapGroup: [] };
                    let memberId: number = row.memberId;
                    while (memberId > 0) {
                        param.LdapGroup.push({});
                        memberId--;
                    }
                    param.LdapGroup.push({
                        Name: ''
                    });

                    // 调用方法;
                    this.ldapService.saveGroup(this.activeController.id, param).subscribe({
                        next: () => {
                            this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            this.initControllers(this.activeController, true);
                        },
                        error: () => { }
                    });
                }
            });
        } else {
            this.modal.open(SecondPasswordComponent, {
                id: 'delModal',
                modalClass: 'secondModal',
                context: {
                    password: '',
                    state: false,
                    active: false
                },
                beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                    // reason, true:点击ok， false:点击取消
                    if (!reason) {
                        modalRef.destroy(reason);
                    } else {
                        if (modalRef.content.instance.password === '' || !modalRef.content.instance.active) {
                            return;
                        }
                        modalRef.content.instance.active = false;

                        // 组装参数
                        const param: { LdapGroup: any[] } = { LdapGroup: [] };
                        let memberId: number = row.memberId;
                        while (memberId > 0) {
                            param.LdapGroup.push({});
                            memberId--;
                        }
                        param.LdapGroup.push({
                            Name: ''
                        });

                        // 调用方法;
                        this.ldapService.saveGroup(this.activeController.id, param, modalRef.content.instance.password).subscribe({
                            next: () => {
                                modalRef.destroy(reason);
                                this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                                this.initControllers(this.activeController, true);
                            },
                            error: () => {
                                modalRef.content.instance.state = true;
                                modalRef.content.instance.password = '';
                            }
                        });
                    }
                }
            });
        }
    }

    // 密码框回车时，保存数据
    public enter(ev: KeyboardEvent): void {
        // tslint:disable-next-line: deprecation
        if (ev.keyCode === 13) {
            this.save();
        }
    }

    // 密码变化时，清除报错状态
    public pwdChange(): void {
        this.secondError = false;
        this.computeState();
    }

    // CRL证书上传成功后执行一次初始化操作
    public uploadSuccess(): void {
        this.initControllers(this.activeController, true);
    }

    // 计算保存按钮的状态
    private computeState(): boolean {
        // 1. 首先是计算表格表单元素数据是否变化或校验通过
        const formChanged: boolean = Object.keys(this.changeData).length > 0;
        const valid: boolean = this.ldapForm.valid;

        // 3. 计算证书验证的数据是否变化
        const certState: boolean = this.baseCertifyData.certificateVerificationEnabled !== this.certificateVerificationEnabled
            || this.baseCertifyData.certificateVerificationLevel !== this.selected
            || this.baseCertifyData.fileSelected;

        // 4. 最后计算登录密码是否已输入，并且后端校验是否通过
        const pwdState: boolean = this.loginWithoutPassword ? true : (this.userLoginPwd.length > 0 && !this.secondError);
        // 按钮禁用的条件：1. 密码无效  2. 表单和证书都无变化， 3.表单校验出错。
        this.saveBtnStatus = !pwdState || (!certState && !formChanged) || !valid;
        return this.saveBtnStatus;
    }

    // 该方法得作用在于解决自动化脚本再重置输入框得值为空值时，不能触发代码得change事件，同时 html的事件需要修改为ngModelChange
    public dipatchEvent(param: { value: string, name: string }): void {
        this.ldapForm.controls[param.name].patchValue(param.value);
    }

}
