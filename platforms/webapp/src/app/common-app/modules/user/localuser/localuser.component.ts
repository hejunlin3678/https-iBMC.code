import { Component, OnInit, ViewChild, ViewContainerRef, ComponentFactory, ComponentFactoryResolver } from '@angular/core';
import { TiTableRowData, TiTableColumns, TiModalService, TiModalRef, TiMessageService } from '@cloud/tiny3';
import { LocalUserService } from './localuser.service';
import { UserInfoTemplateComponent } from './component/user-info-template/user-info-template.component';
import { SshUploadComponent } from './component/ssh-upload/ssh-upload.component';
import { LoadingService, AlertMessageService, UserInfoService } from 'src/app/common-app/service';
import { HalfComponentTitle, Operators, SecondAuthorityFail, SendMethodName } from '../dataType/data.model';
import { ICustError, IUsersInfo } from '../dataType/data.interface';
import { TranslateService } from '@ngx-translate/core';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { DomSanitizer } from '@angular/platform-browser';

@Component({
    selector: 'app-localuser',
    templateUrl: './localuser.component.html',
    styleUrls: ['./localuser.component.scss']
})
export class LocaluserComponent implements OnInit {
    @ViewChild('add', { read: ViewContainerRef, static: true }) addContainer: ViewContainerRef;

    constructor(
        private localUserService: LocalUserService,
        private resolver: ComponentFactoryResolver,
        private loading: LoadingService,
        private modal: TiModalService,
        private alertService: AlertMessageService,
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private usersInfo: UserInfoService,
        private domSanitizer: DomSanitizer

    ) {
        // 有配置用户权限和配置自身权限
        this.hasConfigUsers = this.usersInfo.hasPrivileges(['ConfigureUsers']);
        this.hasConfigSelf = this.usersInfo.hasPrivileges(['ConfigureSelf']);
    }

    public userSearchValue: string = '';
    public hasConfigUsers: boolean = false;
    public hasConfigSelf: boolean = false;
    public systemLocked: boolean = this.usersInfo.systemLockDownEnabled;
    public loginWithoutPassword: boolean = this.usersInfo.loginWithoutPassword;
    public state: boolean = false;
    public modalInstant: TiModalRef = null;
    private MAX_SUPPORT_COUNT: number = 16;

    public data: TiTableRowData[] = [];
    public usersList = {
        displayed: [],
        srcData: {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        },
        searchWords: [this.userSearchValue],
        searchKeys: ['userName']
    };


    public columns: TiTableColumns[] = [
        {
            title: 'ID',
            width: '10%'
        },
        {
            title: 'COMMON_USERNAME',
            width: '15%'
        },
        {
            title: 'COMMON_ROLE',
            width: '15%'
        },
        {
            title: 'COMMON_LOGIN_INTERFACE',
            width: '40%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '20%'
        }
    ];
    public userCountState: boolean = false;

    ngOnInit(): void {
        this.getUserList(false);
    }

    // 获取用户列表数据
    public getUserList(hideLoading: boolean): void {
        if (!hideLoading) {
            this.loading.state.next(true);
        }

        this.localUserService.getUserList().subscribe((res) => {
            this.usersList.srcData.data = res;
            this.userCountState = res.length < this.MAX_SUPPORT_COUNT;
            if (!hideLoading) {
                this.loading.state.next(false);
            }
        }, () => {
            this.loading.state.next(false);
        });
    }

    // 展开用户详情
    public beforeToggle(row: TiTableRowData): void {
        row.showDetails = !row.showDetails;
    }

    // 添加用户点击事件
    public addUser(ev: Event): void {
        if (this.systemLocked) {
            return;
        }
        ev.stopPropagation();
        this.createComponent();
    }

    // 点击新增或编辑时，创建半屏弹窗组件并绑定相关数据
    private createComponent(row?): void {
        this.addContainer.clear();
        const componentFactory: ComponentFactory<any> = this.resolver.resolveComponentFactory(UserInfoTemplateComponent);
        const component = this.addContainer.createComponent(componentFactory);

        component.instance.halfTitle = row ? HalfComponentTitle.EDIT : HalfComponentTitle.ADD;
        component.instance.roleDisabled = row ? !row.canDisabled : undefined;
        component.instance.row = row || undefined;

        component.instance.close.subscribe(() => {
            component.destroy();
            this.getUserList(false);
        });
        component.instance.reInit.subscribe(() => {
            this.getUserList(true);
        });
    }

    // 编辑用户
    public editUser(row: IUsersInfo): void {
        if (this.systemLocked) {
            return;
        }
        this.createComponent(row);
    }

    // 删除用户
    public deleteUser(row: IUsersInfo): void {
        if (!row.canDeleted || this.systemLocked || !this.hasConfigUsers) {
            return;
        }
        this.openModal(Operators.DELETE, row);
    }

    // 禁用用户
    public disableUser(row: IUsersInfo): void {
        if (!row.canDisabled || this.systemLocked) {
            return;
        }
        this.openModal(Operators.DISABLE, row);
    }

    // 启用用户
    public enableUser(row: IUsersInfo): void {
        if (this.systemLocked || this.systemLocked) {
            return;
        }
        this.openModal(Operators.ENABLE, row);
    }

    // 打开二次密码认证框
    private openModal(type: string, row: IUsersInfo): void {
        const msg = `<span id="confirmText" class="font-14">${this.translate.instant('COMMON_ASK_OK')}</span>`;
        if (this.loginWithoutPassword) {
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
                    let method: string = SendMethodName.NORMAL;
                    switch (type) {
                        case Operators.DELETE:
                            method = SendMethodName.DELETE;
                            break;
                        case Operators.DISABLE:
                        case Operators.ENABLE:
                            method = SendMethodName.MODIFY;
                            break;
                        case Operators.DELETESSHKEY:
                            method = SendMethodName.SSHDELETE;
                            break;
                        default:
                            method = SendMethodName.NORMAL;
                    }

                    if (method === SendMethodName.NORMAL) {
                        return;
                    }

                    // 调用方法
                    this.localUserService[method](row.id, '', !row.state).subscribe({
                        next: () => {
                            this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            this.getUserList(false);
                        },
                        error: (error: ICustError) => { }
                    });
                }
            });
        } else {
            this.modal.open(SecondPasswordComponent, {
                id: 'myModal',
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
                        return;
                    }

                    if (modalRef.content.instance.password === SecondAuthorityFail.EMPTY || !modalRef.content.instance.active) {
                        return;
                    }
                    modalRef.content.instance.active = false;
                    let method: string = SendMethodName.NORMAL;
                    switch (type) {
                        case Operators.DELETE:
                            method = SendMethodName.DELETE;
                            break;
                        case Operators.DISABLE:
                        case Operators.ENABLE:
                            method = SendMethodName.MODIFY;
                            break;
                        case Operators.DELETESSHKEY:
                            method = SendMethodName.SSHDELETE;
                            break;
                        default:
                            method = SendMethodName.NORMAL;
                    }

                    if (method === SendMethodName.NORMAL) {
                        return;
                    }

                    // 调用方法
                    this.localUserService[method](row.id, modalRef.content.instance.password, !row.state).subscribe({
                        next: () => {
                            modalRef.destroy(reason);
                            this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            this.getUserList(false);
                        },
                        error: (error: ICustError) => {
                            // 反生错误时，清除二次认证密码
                            modalRef.content.instance.password = '';
                            const errorId = error.errors[0].errorId;
                            if (errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                                modalRef.content.instance.state = true;
                            } else {
                                modalRef.destroy(false);
                            }
                        }
                    });
                }
            });
        }
    }

    // 上传SSH证书
    public uploadSSH(row: IUsersInfo): void {
        this.modalInstant = this.modal.open(SshUploadComponent, {
            id: 'uploadSSH',
            modalClass: 'sshUpload',
            context: {
                activeRow: row
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                modalRef.destroy(reason);
                this.modalInstant = null;
                if (reason) {
                    this.getUserList(false);
                }
            }
        });
    }

    // 删除SSH证书
    public deleteSSH(row: IUsersInfo): void {
        if (this.systemLocked) {
            return;
        }
        this.openModal(Operators.DELETESSHKEY, row);
    }

    ngOnDestroy(): void {
        if (this.modalInstant) {
            this.modalInstant.destroy(true);
        }
    }

    // 当系统锁定状态变化时，更新组件的控件状态
    public lockStateChange(state: boolean): void {
        this.systemLocked = state;
    }

    // 搜索用户
    public filterUsers(): void {
        this.usersList.searchWords = [this.userSearchValue];
    }
}
