import { Component, OnInit, ViewChild, ViewContainerRef, ComponentFactory, ComponentFactoryResolver } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiModalRef } from '@cloud/tiny3';
import { AssetService } from './services/asset.service';
import { AssetEditComponent } from './component/asset-edit/asset-edit.component';
import { CustHalfModalComponent } from 'src/app/common-app/components/cust-half-modal/cust-half-modal.component';
import { AssetStaticModel } from './model/asset-static.model';
import { AssetModel } from './model/asset.model';
import { IrmGlobalDataService } from 'src/app/irm-app/service/irm-global-data.service';
import {
    LoadingService,
    SystemLockService,
    UserInfoService
} from 'src/app/common-app/service';

@Component({
    selector: 'app-asset',
    templateUrl: './asset.component.html',
    styleUrls: ['./asset.component.scss']
})
export class AssetComponent implements OnInit {
    @ViewChild('add', { read: ViewContainerRef, static: true }) addContainer: ViewContainerRef;
    public state: boolean = false;
    public modalInstant: TiModalRef;
    constructor(
        private assetService: AssetService,
        private resolver: ComponentFactoryResolver,
        private loading: LoadingService,
        private lockService: SystemLockService,
        private irmGlobalData: IrmGlobalDataService,
        private user: UserInfoService
    ) {
    }
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public colsNumber = 2;
    public colsGap = ['100px'];
    public verticalAlign = 'middle';
    public labelWidth = '175px';
    public isSystemLock: boolean = false;
    public columns: TiTableColumns[] = [
        {
            title: 'ASSET_U_NUMBER',
            width: '6%'
        },
        {
            title: 'IRM_DEVICE_MODEL',
            width: '10%'
        },
        {
            title: 'COMMON_DEVICE_TYPE',
            width: '10%'
        },
        {
            title: 'COMMON_MANUFACTURER',
            width: '8%'
        },
        {
            title: 'IRM_POSITION_USE',
            width: '8%'
        },
        {
            title: 'COMMON_SERIALNUMBER',
            width: '18%'
        },
        {
            title: 'IRM_DISCOVER_TIME',
            width: '20%'
        },
        {
            title: 'ASSET_RATED_POWER_WATTS',
            width: '10%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '10%'
        }
    ];
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };
    public assetDetails = AssetStaticModel.assetDetails;
    public trackByFn(index: string, item: any): string {
        return index;
    }
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.getAssetManagementList();
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.addContainer) {
                this.addContainer.clear();
            }
        });
    }
    // 获取列表数据
    public getAssetManagementList() {
        this.loading.state.next(true);
        this.assetService.getAllAssetData().subscribe((res: AssetModel[]) => {
            this.loading.state.next(false);
            this.srcData.data = res;
            if (this.irmGlobalData.assetUid && this.irmGlobalData.assetUid > -1) {
                res.forEach(item => {
                    if (Number(item.uNum) === this.irmGlobalData.assetUid) {
                        this.editAsset(item, '');
                    }
                });
            }
        }, () => {
            this.loading.state.next(false);
        });
    }
    // 展开用户详情
    public beforeToggle(row: TiTableRowData): void {
        row.showDetails = !row.showDetails;
    }
    // 点击新增或编辑时，创建半屏弹窗组件并绑定相关数据
    public editAsset(row, rWCapability) {
        if (rWCapability === 'ReadOnly' || this.isSystemLock || !this.isConfigureComponentsUser) {
            return;
        }
        if (this.addContainer) {
            this.addContainer.clear();
        }
        const componentFactory: ComponentFactory<any> = this.resolver.resolveComponentFactory(AssetEditComponent);
        const editAsset = this.addContainer.createComponent(componentFactory);
        editAsset.instance.editData = row;
        editAsset.instance.isSystemLock = this.isSystemLock;
        editAsset.instance.halfTitle = 'COMMON_EDIT';
        editAsset.instance.close.subscribe(() => {
            editAsset.destroy();
            this.irmGlobalData.assetUid = -1;
            this.getAssetManagementList();
        });
        editAsset.instance.reInit.subscribe(() => {
            this.irmGlobalData.assetUid = -1;
            this.getAssetManagementList();
        });
    }
}
