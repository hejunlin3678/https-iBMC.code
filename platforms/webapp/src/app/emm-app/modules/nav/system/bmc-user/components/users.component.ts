import { Component, OnInit, TemplateRef, ElementRef, ViewChild, ViewContainerRef, ComponentFactoryResolver, ComponentFactory } from '@angular/core';
import { IBlade, IBmcUser } from './model/index';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiModalService, TiModalRef, TiMessageService } from '@cloud/tiny3';
import { EmmSecondComponent } from './emm-second/emm-second.component';
import { UserManagerComponent } from './user-manager/user-manager.component';
import { UsersService } from './users.service';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { forkJoin } from 'rxjs';
import { AlertMessageService, UserInfoService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';
import { DomSanitizer } from '@angular/platform-browser';
import { SecondAuthorityFail } from 'src/app/common-app/modules/user/dataType/data.model';

@Component({
    selector: 'app-users',
    templateUrl: './users.component.html',
    styleUrls: ['./users.component.scss']
})
export class UsersComponent implements OnInit {

    constructor(
        private userInfo: UserInfoService,
        private loading: LoadingService,
        private tiModal: TiModalService,
        private tiMessage: TiMessageService,
        private resolve: ComponentFactoryResolver,
        private userService: UsersService,
        private alertService: AlertMessageService,
        private translate: TranslateService,
        private domSanitizer: DomSanitizer
    ) { }

    @ViewChild('myModal', { static: true }) modal: ElementRef;
    @ViewChild('addContainer', { read: ViewContainerRef, static: true }) addContainer: ViewContainerRef;
    public isSystemLock: boolean = false;
    public bladeList: IBlade[] = [];
    public currentBlade: IBlade = null;
    public isLoading: boolean = false;

    public checkedList: TiTableRowData[] = [];
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public columns: TiTableColumns[] = [
        {
            title: '',
            width: '10%'
        },
        {
            title: 'COMMON_USERNAME',
            width: '25%'
        },
        {
            title: 'IBMC_USER_PRIVIL',
            width: '25%'
        },
        {
            title: 'IBMC_USER_BELONG',
            width: '25%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '20%'
        }
    ];

    ngOnInit(): void {
        this.init();
    }

    private init() {
        this.loading.state.next(true);
        this.userService.getObjects().subscribe({
            next: (res: IBlade[]) => {
                this.bladeList = res;
                this.currentBlade = this.bladeList[0];

                // 查询当前选中的用户列表
                this.getSingleUsers(this.currentBlade);
            },
            error: () => {
                this.loading.state.next(false);
            }
        });
    }

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public getSingleUsers(selectSlot: IBlade) {
        this.isLoading = true;
        this.loading.state.next(true);
        this.userService.getSingleObjectUsers(selectSlot?.id).subscribe({
            next: (res2: IBmcUser[]) => {
                // 设置用户归属对象
                res2.forEach((user: IBmcUser) => {
                    user.belongTo = selectSlot.label;
                });

                this.isLoading = false;
                this.loading.state.next(false);
                this.currentBlade.list = res2;
                this.srcData = {
                    data: res2,
                    state: {
                        searched: false,
                        sorted: false,
                        paginated: false
                    }
                };
            },
            error: () => {
                this.isLoading = false;
                this.loading.state.next(false);
            }
        });
    }

    // 切换Blade事件
    public switchBlade(currentBlade: IBlade) {
        this.getSingleUsers(currentBlade);
    }

    // 添加BMC用户
    public addBmcUser(type: string): void {
        this.createHalfModal('add');
    }

    // 编辑BMC用户
    public editBmcUser(row?: TiTableRowData): void {
        this.createHalfModal('edit', row);
    }

    // 创建半屏弹窗
    private createHalfModal(type: 'add' | 'edit', row?: TiTableRowData): void {
        const componentFactor: ComponentFactory<any> = this.resolve.resolveComponentFactory(UserManagerComponent);
        this.addContainer.clear();
        const component = this.addContainer.createComponent(componentFactor);
        component.instance.type = type;
        component.instance.bladeList = this.currentBlade;
        component.instance.currentBlade = this.currentBlade;
        if (row) {
            component.instance.row = row;
        }

        component.instance.close.subscribe(() => {
            component.destroy();
            this.getSingleUsers(this.currentBlade);
        });
        component.instance.reInit.subscribe(() => {
            this.getSingleUsers(this.currentBlade);
        });
    }

    // 删除BMC用户（包括批量删除）
    public delBmcUser(row?: TiTableRowData): void {
        const rows = row ? [row] : this.checkedList;
        // 是否有选中用户
        if (rows.length > 0) {
            this.delBmcUser_haveData(rows);
        } else {
            this.delBmcUser_noData();
        }
    }

    // 点击删除用户，没有选中数据
    public delBmcUser_noData() {
        const msg = `<span id="confirmText" class="font-14">${this.translate.instant('COMMON_ASK_CHECK_DATA')}</span>`;
        this.tiMessage.open({
            id: 'delBmcUserModalNoData',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.domSanitizer.bypassSecurityTrustHtml(msg),
            okButton: {
                text: this.translate.instant('COMMON_OK'),
                autofocus: false
            },
            cancelButton: {
                show: false
            }
        });
    }

    // 点击删除用户，有选中数据
    public delBmcUser_haveData(rows) {
        // 是否免密登录
        if (this.userInfo.loginWithoutPassword) {
            const msg = `<span id="confirmText" class="font-14">${this.translate.instant('COMMON_ASK_OK')}</span>`;
            this.tiMessage.open({
                id: 'delBmcUserModal',
                type: 'prompt',
                title: this.translate.instant('COMMON_CONFIRM'),
                content: this.domSanitizer.bypassSecurityTrustHtml(msg),
                okButton: {
                    text: this.translate.instant('COMMON_OK'),
                    autofocus: false
                },
                beforeClose: (modalRef: TiModalRef, reason: boolean) => {
                    if (reason) {
                        this.deleteUsers(rows, modalRef);
                    } else {
                        modalRef.destroy(false);
                    }
                }
            });
        } else {
            this.tiModal.open(EmmSecondComponent, {
                id: 'delBmcUserModal',
                context: {
                    passowrd: '',
                    active: false,
                    state: false
                },
                beforeClose: (modalRef: TiModalRef, reason: boolean) => {
                    if (reason) {
                        this.deleteUsers(rows, modalRef, modalRef.content.instance.password);
                    } else {
                        modalRef.destroy(false);
                    }
                }
            });
        }
    }

    // 删除用户、组用户
    private deleteUsers(users: TiTableRowData[], modalRef: TiModalRef, password?: string) {
        if (users.length === 0) {
            return;
        }
        const deleteList = [];
        users.map(item => {
            deleteList.push(this.userService.deleteUser(item.url, password));
        });
        forkJoin(deleteList).subscribe({
            next: (res) => {
                this.alertService.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });
                this.checkedList = [];
                modalRef.destroy(true);
                this.getSingleUsers(this.currentBlade);
            },
            error: (error) => {
                // 反生错误时，清除二次认证密码
                modalRef.content.instance.password = '';
                const errorId = getMessageId(error.error)[0].errorId;
                if (errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                    modalRef.content.instance.state = true;
                } else {
                    modalRef.destroy(true);
                    this.alertService.eventEmit.emit({
                        type: 'error',
                        label: this.translate.instant(errorId)
                    });
                }
            }
        });
    }
}
