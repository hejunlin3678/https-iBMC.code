
import { AlertMessageService, PRODUCT, UserInfoService } from 'src/app/common-app/service';
import { Subscription } from 'rxjs';
import { Component, OnInit, Input } from '@angular/core';
import { TiTableColumns, TiTableSrcData, TiModalService, TiModalRef } from '@cloud/tiny3';
import { ITableRow } from './authority-management.datatype';
import { AuthorityManagementService } from './service';
import { SaveParamModalComponent } from './components/save-param-modal/save-param-modal.component';
import { CommonData } from 'src/app/common-app/models';
import { Pangen } from 'src/app/pangea-app/models/pangen.model';
import { OceanType } from 'src/app/pangea-app/models/common.datatype';

@Component({
    selector: 'app-authority-management',
    templateUrl: './authority-management.component.html',
    styleUrls: ['./authority-management.component.scss']
})

export class AuthorityManagementComponent implements OnInit {

    @Input() systemLockDownEnabled: boolean;

    public isOemSecurity = this.userServ.hasPrivileges(['OemSecurityMgmt']);

    public displayed: ITableRow[] = [];

    private formerTableData: object = {};

    public srcData: TiTableSrcData;

    private getRoleSub: Subscription;

    private getSaveSub: Subscription;
    // 判断是否大西洋直通板
    public isAtlanticSMM: boolean = Pangen.ocean === OceanType.AtlanticSMM;

    public isEmmApp: boolean = CommonData.productType === PRODUCT.EMM;

    public isIrm: boolean = CommonData.productType === PRODUCT.IRM;

    public columns: TiTableColumns[] = [
        {
            title: 'SECURITY_ROLE_NAME',
            width: '9%',
            hide: false,
        },
        {
            title: 'SECURITY_USER_CONFIGURATION',
            width: '9%',
            hide: false,
        },
        {
            title: 'SECURITY_GENERAL_SETTINGS',
            width: '9%',
            hide: false,
        },
        {
            title: 'SECURITY_REMOTE_CONTROL',
            width: '9%',
            hide: this.isEmmApp || this.isIrm || this.isAtlanticSMM,
        },
        {
            title: 'SECURITY_REMOTE_MEDIA',
            width: '9%',
            hide: this.isEmmApp || this.isIrm || this.isAtlanticSMM,
        },
        {
            title: 'SECURITY_SECURITY_CONFIG',
            width: '9%',
            hide: false,
        },
        {
            title: 'HOME_POWER_CONTROL',
            width: '9%',
            hide: false,
        },
        {
            title: 'SECURITY_COMMISSIONING_DIAGNOSIS',
            width: '9%',
            hide: false,
        },
        {
            title: 'SECURITY_QUERYFUNCTION',
            width: '9%',
            hide: false,
        },
        {
            title: 'SECURITY_CONFIGUREITSELF',
            width: '9%',
            hide: false,
        },
        {
            title: 'COMMON_OPERATE',
            width: '150px',
            hide: false,
        }
    ];

    constructor(
        private authorityServ: AuthorityManagementService,
        private userServ: UserInfoService,
        private alertServ: AlertMessageService,
        private tiModal: TiModalService,
    ) { }

    ngOnInit() {
        this.getRoleSub = this.authorityServ.getData().subscribe((roles: ITableRow[]) => {
            this.srcData = {
                data: roles,
                state: {
                    searched: false,
                    sorted: false,
                    paginated: false
                }
            };
        });
    }

    public trackByFn(index: number, item: ITableRow): number {
        return item.id;
    }

    public permissionSave(row: ITableRow) {
        if (row.isSave) {
            return;
        }
        if (this.getRoleSub && this.getRoleSub.unsubscribe) {
            this.getRoleSub.unsubscribe();
        }
        if (this.getSaveSub && this.getSaveSub.unsubscribe) {
            this.getSaveSub.unsubscribe();
        }

        row.isSave = true;

        if (this.userServ.loginWithoutPassword) {
            this.savePar(row);
        } else {
            this.openModal(row);
        }
    }

    private savePar(row: ITableRow) {
        this.getSaveSub = this.authorityServ.savePar(row).subscribe((data: ITableRow) => {
            this.alertServ.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            row.isEdit = false;
            this.resetRow(row, data);
            row.isSave = false;
        });
    }

    private openModal(row: ITableRow) {
        const modal = this.tiModal.open(SaveParamModalComponent, {
            id: 'saveParamModal',
            context: {
                row
            },
            close: () => {
                row.isSave = false;
            },
            dismiss: () => {
                row.isSave = false;
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                // reason为true表示确定按钮
                if (reason) {
                    const pwd = this.authorityServ.getPwd();
                    this.getSaveSub = this.authorityServ.saveParWithPwd(row, pwd).subscribe((rowData: ITableRow) => {
                        this.resetRow(row, rowData);
                        row.isEdit = false;
                        modalRef.destroy(reason);
                        this.alertServ.eventEmit.emit({
                            type: 'success',
                            label: 'COMMON_SUCCESS'
                        });
                    }, (e) => {
                        // 发生任何错误时，二次认证密码清除
                        modalRef.content.instance.passwordInput.value = '';
                        if (e.error && e.error.error && Array.isArray(e.error.error)) {
                            if (e.error.error.some(errorItem => errorItem.code === 'ReauthFailed')) {
                                this.authorityServ.sendErroMsg('COMMON_SECONDPWD_ERROR');
                            }
                        }
                    });
                } else {
                    modalRef.destroy(reason);
                }
            },
        });

    }

    public permissionCancel(row: ITableRow) {
        if (row.isSave) {
            return;
        }
        this.resetRow(row, this.formerTableData[row.id]);
        row.isEdit = false;
    }

    private resetRow(row: ITableRow, newRow: ITableRow) {
        const keys = Object.keys(row);
        for (const key of keys) {
            row[key] = newRow[key];
        }
    }

    public permissionEdit(row: ITableRow) {
        this.formerTableData[row.id] = Object.assign({}, row);
        if (this.isOemSecurity) {
            row.isEdit = true;
        }
    }

    // 电源控制选中的时候 如果 常规设置为未选择，那么修改其为选择状态
    public powerControlEnabledChange(e) {
        if (e.powerControlEnabled && !e.basicConfigEnabled && CommonData.productType === PRODUCT.EMM) {
            e.basicConfigEnabled = true;
        }
    }

    ngOnDestroy(): void {
        this.getRoleSub.unsubscribe();
        if (this.getSaveSub && this.getSaveSub.unsubscribe) {
            this.getSaveSub.unsubscribe();
        }
    }

}
