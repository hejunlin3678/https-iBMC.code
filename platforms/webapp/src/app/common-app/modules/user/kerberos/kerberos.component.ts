import { Component, OnInit, ViewChild, ViewContainerRef, ComponentFactory, ComponentFactoryResolver } from '@angular/core';
import {
    TiTableColumns,
    TiTableRowData,
    TiTableSrcData,
    TiModalService,
    TiModalRef,
    TiValidationConfig,
    TiMessageService,
    TiValidators,
    TiFileItem,
    TiUploadConfig,
    TiFileInfo
} from '@cloud/tiny3';
import { KerberosService } from './kerberos.service';
import { UntypedFormGroup, UntypedFormBuilder, Validators } from '@angular/forms';
import { IGroup, IGroupItem, IKerberos, IKrbSave } from '../dataType/data.interface';
import { HalfModalKerberosComponent } from './components/half-modal-kerberos/half-modal-kerberos.component';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { AlertMessageService, LoadingService, UserInfoService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { getMessageId, cutStr, KRB_DOMAIN, getByte } from 'src/app/common-app/utils';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { SecondAuthorityFail, KrbErrorIds, Operators, FileAddFaild } from '../dataType/data.model';
import { HttpErrorResponse } from '@angular/common/http';
import { timer } from 'rxjs/internal/observable/timer';
import { DomSanitizer } from '@angular/platform-browser';

@Component({
    selector: 'app-kerberos',
    templateUrl: './kerberos.component.html',
    styleUrls: ['./kerberos.component.scss']
})
export class KerberosComponent implements OnInit {
    @ViewChild('group', { read: ViewContainerRef, static: true }) groupContainer: ViewContainerRef;
    public systemLocked: boolean = this.userInfo.systemLockDownEnabled;
    public kerberosForm: UntypedFormGroup = null;
    public kerberosInfo: IGroup = null;
    public validState: boolean = false;
    public disabled: boolean = true;
    public userPassword: string = '';
    public baseData: { realm?: string; server?: string; port?: string } = {};
    public paramData: IKrbSave = {};
    public uploadState: boolean = false;
    private hasConfigUsers: boolean = false;
    public loginWithoutPassword: boolean = this.userInfo.loginWithoutPassword;

    // 开关状态
    public switchState: boolean = false;
    public saveError: boolean = false;

    constructor(
        private kerberosService: KerberosService,
        private fb: UntypedFormBuilder,
        private alertService: AlertMessageService,
        private resolver: ComponentFactoryResolver,
        private modal: TiModalService,
        private loading: LoadingService,
        private userInfo: UserInfoService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private domSanitizer: DomSanitizer
    ) {
        this.hasConfigUsers = this.userInfo.hasPrivileges(['ConfigureUsers']);
    }

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
            title: 'SID',
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

    public krbDomainValidator: TiValidationConfig = null;
    public krbServerValidator: TiValidationConfig = null;
    public krbPortValidator: TiValidationConfig = null;

    // 上传密钥表
    public fileItem: TiFileItem = null;

    public keyTabs: TiUploadConfig = {
        url: '',
        autoUpload: false,
        alias: 'Content',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.userInfo.getToken()
        },
        filters: [{
            name: 'maxCount',
            params: [1]
        }, {
            name: 'type',
            params: ['.keytab']
        }, {
            name: 'maxSize',
            params: [1024 * 1024]
        }],
        onAddItemSuccess: (fileItem: TiFileItem) => {
            this.uploadState = false;
            this.fileItem = fileItem;
            this.computedSaveState();
        },
        onRemoveItems: (fileItem: TiFileItem[]) => {
            this.fileItem = null;
            this.computedSaveState();
        }
    };

    // 表格相关数据
    public displayed: TiTableRowData[] = [];
    public baseKrbGroupData: TiTableRowData[] = [];
    public srcData: TiTableSrcData = null;

    ngOnInit(): void {
        this.kerberosForm = this.fb.group({
            'realm': [{ value: '', disabled: !this.hasConfigUsers }, [Validators.pattern(KRB_DOMAIN)]],
            'server': [{ value: '', disabled: !this.hasConfigUsers }, [SerAddrValidators.validateIp46AndDomin()]],
            'port': [
                { value: '', disabled: !this.hasConfigUsers },
                [TiValidators.required, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
            ]
        });

        this.init();
    }

    // 添加文件失败时的提示信息
    public onAddItemFailed(fileObject: { file: TiFileInfo, validResults: string[] }) {
        const validResultValue = fileObject.validResults[0];
        if (validResultValue === FileAddFaild.MAXSIZE) {
            this.alertService.eventEmit.emit({ type: 'error', label: 'LDAP_FILE_SCOPE' });
        } else if (validResultValue === FileAddFaild.TYPE) {
            this.alertService.eventEmit.emit({ type: 'error', label: 'KERBEROS_SUPPORTED' });
        }
    }

    // 系统锁定状态变化事件
    public lockStateChange(state: boolean): void {
        this.systemLocked = state;
        let method = state ? Operators.DISABLE : Operators.ENABLE;
        method = this.hasConfigUsers ? method : Operators.DISABLE;
        const curState = this.kerberosForm.controls.realm.disabled ? Operators.DISABLE : Operators.ENABLE;
        if (method !== curState) {
            this.kerberosForm.controls.realm[method]();
            this.kerberosForm.controls.server[method]();
            this.kerberosForm.controls.port[method]();
        }
    }

    // 初始化数据
    private init(): void {
        this.loading.state.next(true);
        this.kerberosService.getKerberos().subscribe((response: [IKerberos, IGroup]) => {
            this.switchState = response[0].KerberosEnabled;
            this.userPassword = '';
            const member: IGroup = response[1];
            this.kerberosInfo = response[1];
            this.baseData = {
                'realm': member.realm,
                'server': member.kerberosServerAddress,
                'port': member.kerberosPort
            };
            // 开始初始化表单数据
            this.kerberosForm.setValue({
                'realm': this.kerberosInfo.realm,
                'server': this.kerberosInfo.kerberosServerAddress,
                'port': this.kerberosInfo.kerberosPort
            });

            // 设置表格数据
            this.baseKrbGroupData = this.kerberosInfo.kerberosGroups;
            this.displayed = member.kerberosGroups.filter((item) => {
                return item.groupSID !== '';
            });
            this.validState = this.displayed.length < 5;
            // 设置校验信息
            this.resetKrbValidator();
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }

    // 设置校验信息
    private resetKrbValidator(): void {
        this.krbDomainValidator = {
            tip: this.translate.instant('KERBEROS_REALM_TIPS'),
            type: 'blur',
            errorMessage: {
                pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR')
            }
        };

        this.krbServerValidator = {
            tip: this.translate.instant('KERBEROS_SERVICE_TIPS'),
            type: 'blur',
            errorMessage: {
                pattern: this.translate.instant('VALIDTOR_FORMAT_ERROR')
            }
        };

        this.krbPortValidator = {
            type: 'blur',
            errorMessage: {
                required: this.translate.instant('VALIDTOR_RANGE'),
                integer: this.translate.instant('VALIDTOR_RANGE')
            }
        };
    }

    // 开启关闭kerberos使能
    public switchChange(): void {
        this.loading.state.next(true);
        this.kerberosService.switchKerberos(this.switchState).subscribe({
            next: () => {
                this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.loading.state.next(false);
            },
            error: () => {
                this.alertService.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                this.switchState = !this.switchState;
                this.loading.state.next(false);
            }
        });
    }

    // 清除密码报错信息
    public clearError(): void {
        this.saveError = false;
        this.computedSaveState();
    }

    // 保存Kerberos参数（若有文件，则上传文件，注：上传文件和参数保存时同时发送请求）
    public save(): void {
        const result = this.checkChange();
        if (this.disabled || !this.hasConfigUsers || this.systemLocked) {
            return;
        }
        if (!this.loginWithoutPassword && this.userPassword === '') {
            return;
        }
        this.paramData = {};
        if (result.realm) {
            this.paramData.Domain = this.kerberosForm.get('realm').value;
        }
        if (result.server) {
            this.paramData.IPAddress = this.kerberosForm.get('server').value;
        }
        if (result.port) {
            this.paramData.Port = parseInt(this.kerberosForm.get('port').value, 10);
        }

        let krbCert: FormData = null;
        if (this.fileItem) {
            krbCert = new FormData();
            krbCert.append('Content', this.fileItem._file, this.fileItem.file.name);
            krbCert.append('Type', 'URI');
            if (!this.loginWithoutPassword) {
                krbCert.append('ReauthKey', window.btoa(this.userPassword));
            }
        }

        this.disabled = true;
        this.loading.state.next(true);

        // 保存数据
        forkJoin([
            this.kerberosService.saveKerberos(this.paramData, this.userPassword),
            this.kerberosService.uploadKrbCert({ secPwd: this.userPassword, formData: krbCert })
        ]).subscribe({
            next: () => {
                this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.userPassword = '';
                if (this.fileItem) {
                    this.fileItem.remove();
                }
                this.loading.state.next(false);
                this.init();
            },
            error: (error: HttpErrorResponse) => {
                this.userPassword = '';
                this.loading.state.next(false);
                const errorId = getMessageId(error.error)[0].errorId;
                if (errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                    this.saveError = true;
                    return;
                }

                if (this.fileItem) {
                    this.fileItem.remove();
                }

                if (errorId === KrbErrorIds.KRBKeytabUploadFail) {
                    const message = this.translate.instant('KeyImportError.errorMessage');
                    this.alertService.eventEmit.emit({ type: 'error', label: message });
                } else {
                    const errMsg = this.translate.instant(errorId)?.errorMessage || 'COMMON_FAILED';
                    this.alertService.eventEmit.emit({ type: 'error', label: errMsg });
                }
            }
        });
    }

    // 值变化检测
    private checkChange(): { state: boolean; realm?: boolean; server?: boolean; port?: boolean } {
        const result = {
            state: JSON.stringify(this.baseData) !== JSON.stringify(this.kerberosForm.value)
        };
        Object.keys(this.kerberosForm.value).forEach((key) => {
            if (this.kerberosForm.value[key] !== this.baseData[key]) {
                result[key] = true;
            } else {
                delete result[key];
            }
        });

        return result;
    }

    // 输入框值变化事件
    public checkValueChange(key?: string): void {
        let value = this.kerberosForm.controls[key].value;
        const vLen = getByte(value);
        if (vLen > 255) {
            value = cutStr(value, 255);
            this.kerberosForm.controls[key].patchValue(value);
        }

        timer(10).subscribe(() => {
            this.computedSaveState();
        });
    }

    // 计算保存按钮的状态
    private computedSaveState(): void {
        // 表单数据是否是有效的变化
        const formState = this.checkChange().state;
        const valid = this.kerberosForm.valid;
        // 没有文件变化时，state为false
        const fileState = this.fileItem !== null;
        // 确保密码是有效的
        const isValidPwd = this.loginWithoutPassword ? true : (this.userPassword.length > 0 && !this.saveError);
        // 保存按钮禁用得条件：1. 密码无效  2. 表单和证书都无变化， 3.表单校验出错。出现任意一个，则按钮禁用
        if (!isValidPwd || (!formState && !fileState) || !valid) {
            this.disabled = true;
        } else {
            this.disabled = false;
        }
    }

    // 新增用户组
    public addGroup(ev: MouseEvent): void {
        ev.stopPropagation();
        // 新增组时，也是相当于编辑组，只不过时传入的组的相关信息都是null，因此传入组件的数据时第一个为null的对象
        const row: IGroupItem = this.kerberosInfo.kerberosGroups.filter((item: IGroupItem) => {
            return item.groupName === '';
        })[0];
        row.groupRole = 'Common User';
        this.createComponent(row, 'add');
    }

    // 编辑用户组
    public editGroup(row: IGroupItem): void {
        if (this.systemLocked) {
            return;
        }
        this.createComponent(row, 'edit');
    }

    // 删除用户
    public deleteGroup(row: IGroupItem): void {
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
                    const param: { Group: any[] } = { Group: [] };
                    let memberId = row.memberId;
                    while (memberId > 0) {
                        param.Group.push({});
                        memberId--;
                    }
                    param.Group.push({ DelGroupFlag: true });

                    // 调用方法
                    this.kerberosService.saveGroup(param).subscribe({
                        next: () => {
                            this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            this.init();
                        },
                        error: () => { }
                    });
                }
            });
        } else {
            this.modal.open(SecondPasswordComponent, {
                id: 'myModal',
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
                        const param: { Group: any[] } = { Group: [] };
                        let memberId = row.memberId;
                        while (memberId > 0) {
                            param.Group.push({});
                            memberId--;
                        }
                        param.Group.push({ DelGroupFlag: true });

                        // 调用方法
                        this.kerberosService.saveGroup(param, modalRef.content.instance.password).subscribe({
                            next: () => {
                                modalRef.destroy(reason);
                                this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                                this.init();
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

    // 动态创建添加组合编辑组的半屏窗组件
    private createComponent(row: IGroupItem, type: string): void {
        this.groupContainer.clear();
        const componentFactory: ComponentFactory<any> = this.resolver.resolveComponentFactory(HalfModalKerberosComponent);
        const compoennt = this.groupContainer.createComponent(componentFactory);
        compoennt.instance.row = row;
        compoennt.instance.type = type;
        compoennt.instance.halfTitle = type === Operators.ADD ? 'COMMON_ADD_GROUP' : 'COMMON_EDIT_GROUP';

        // 监听关闭事件
        compoennt.instance.close.subscribe(() => {
            compoennt.destroy();
            this.init();
        });
    }
}
