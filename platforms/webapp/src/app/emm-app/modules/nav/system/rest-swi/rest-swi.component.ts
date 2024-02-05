import { Component, ComponentFactory, ComponentFactoryResolver, OnInit, ViewChild, ViewContainerRef } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiModalRef, TiModalService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService, UserInfoService, AlertMessageService, SystemLockService } from 'src/app/common-app/service';
import { ConfigRecoveryComponent } from './component/config-recovery/config-recovery.component';
import { SwiDetailComponent } from './component/swi-detail/swi-detail.component';
import { SwapInfo } from './model/rest-swi.model';
import { RestSwiService } from './services/rest-swi.service';

@Component({
    selector: 'app-rest-swi',
    templateUrl: './rest-swi.component.html',
    styleUrls: ['./rest-swi.component.scss'],
})
export class RestSwiComponent implements OnInit {
    @ViewChild('detail', { read: ViewContainerRef, static: true }) detailContainer: ViewContainerRef;
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private user: UserInfoService,
        private loading: LoadingService,
        private tiModal: TiModalService,
        private lockService: SystemLockService,
        private resolver: ComponentFactoryResolver,
        private translate: TranslateService,
        private alertService: AlertMessageService,
        private restSwiService: RestSwiService
    ) {
        this.displayed = [];
        this.srcData = {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
    }
    public oemPowerControl = this.user.hasPrivileges(['OemPowerControl']);
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public modalInstant: TiModalRef;
    public swapModal;
    public colsNumber = 1;
    public verticalAlign = 'middle';
    public labelWidth = '88px';
    public columns: TiTableColumns[] = [
        {
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '15%'
        },
        {
            title: 'EMM_SWAP_TYPE',
            width: '15%'
        },
        {
            title: 'EMM_SWAP_PLANE',
            width: '15%'
        },
        {
            title: 'EMM_SWAP_CURRENT_FILE_NAME',
            width: '15%'
        },
        {
            title: 'EMM_SWAP_BACKUP_COUNT',
            width: '20%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '20%'
        },
    ];
    public columnsDetail: TiTableColumns[] = [
        {
            title: 'EVENT_ORDER',
            width: '25%'
        },
        {
            title: 'EMM_SWAP_FILE_NAME',
            width: '25%'
        },
        {
            title: 'EMM_SWAP_TIME',
            width: '25%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '25%'
        },
    ];
    // 展开用户详情
    public beforeToggle(row: TiTableRowData): void {
        row.showDetails = !row.showDetails;
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.swapModal) {
                this.swapModal.dismiss();
            }
            if (res && this.detailContainer) {
                this.detailContainer.clear();
            }
        });
        this.init();
    }
    public init() {
        this.loading.state.next(true);
        this.restSwiService.getSwapInfo().subscribe((res: SwapInfo[]) => {
            this.loading.state.next(false);
            this.srcData.data = res;
        }, () => {
            this.loading.state.next(false);
        });
    }
    public trackByFn(index: number, item: any): number {
        return item.id;
    }
    // 详情弹窗
    public openDetail(row, backupTime?) {
        if (this.detailContainer) {
            this.detailContainer.clear();
        }
        const componentFactory: ComponentFactory<any> = this.resolver.resolveComponentFactory(SwiDetailComponent);
        const compoennt = this.detailContainer.createComponent(componentFactory);
        compoennt.instance.row = row;
        compoennt.instance.backupTime = backupTime;
        compoennt.instance.halfTitle = row.slotSwi + this.translate.instant('EMM_SWAP_DETAIL_TITLE');

        // 监听关闭事件
        compoennt.instance.close.subscribe((value) => {
            compoennt.destroy();
            if (value) {
                this.recoverySet(row, backupTime);
            }
        });
    }
    // 配置恢复
    public recoverySet(row, backupTime) {
        if (this.isSystemLock) {
            return;
        }
        const swapData = this.srcData.data.filter((item) => {
            return item.slotNo !== row.slotNo && item.planeType === row.planeType;
        });
        this.swapModal = this.tiModal.open(ConfigRecoveryComponent, {
            id: 'recoverySettingId',
            modalClass: 'swapModal',
            context: {
                swapData,
                columns: this.columnsDetail,
                rowData: row,
                backupTime,
                param: null
            },
            close: (modalRef: TiModalRef): void => {
                const param = modalRef.content.instance.param;
                if (param) {
                    param.forEach(item => {
                        this.restSwiService.recoverySetting(item).subscribe((res) => {
                            this.loading.state.next(false);
                            this.init();
                            this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        }, (error) => {
                            this.loading.state.next(false);
                            this.alertService.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                        });
                    });
                }
            },
            dismiss: (): void => { }
        });
    }

}
