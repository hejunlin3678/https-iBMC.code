import { Component, OnInit, OnDestroy, Renderer2 } from '@angular/core';
import { Router } from '@angular/router';
import { TranslateService } from '@ngx-translate/core';
import { Subscription } from 'rxjs/internal/Subscription';

import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { HomeService } from './services';
import {
    IAccess, IResource, IOverview, IDeviceInfo, IMonitorInfo, IResourceInfo,
    IAccessInfo
} from './model/home.datatype';
import { TickMark, Baffle } from './model';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { htmlEncode } from 'src/app/common-app/utils/common';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { ErrortipService } from 'src/app/common-app/service/errortip.service';
import { TiModalService } from '@cloud/tiny3';
import { HomeIrmQuickConfigComponent } from './component/home-irm-quick-config/home-irm-quick-config.component';

@Component({
    selector: 'app-home',
    templateUrl: './home.component.html',
    styleUrls: ['./home.component.scss'],
})
export class HomeComponent implements OnInit, OnDestroy {
    constructor(
        private homeService: HomeService,
        private rd2: Renderer2,
        private userService: UserInfoService,
        private loadingService: LoadingService,
        private translate: TranslateService,
        private globalData: GlobalDataService,
        private router: Router,
        private alert: AlertMessageService,
        private lockService: SystemLockService,
        private tiModal: TiModalService
    ) { }

    // irm现在不包含RM110系列产品，注释掉该处以免判断错误
    public isRM110: boolean = false ;
    public hasConfigureComponentsRight = this.userService.hasPrivileges(['ConfigureComponents']);

    public loading = true;
    public showmointer = true;
    public overViewSubscription: Subscription;
    public accesses: any = [];
    public resources: IResource[] = [];
    public systemLocked: boolean = true;
    public serverDetail: IDeviceInfo = {
        productName: '',
        position: '',
        capacity: '',
        serialNumber: '',
        firmwareVersion: ''
    };
    public monitorInfo: IMonitorInfo = {
        totalU: 0,
        usedU: 0,
        deviceTotalNumber: 0,
        serverNumber: 0,
        networkNumber: 0,
        storeNumber: 0,
        otherNumber: 0,
        maxRatedPower: 0,
        currentPower: 0,
    };

    public quickConfigModal;
    public timerRefresh: any;
    public ntpInfo: any = {};
    public ipInfo: any = {};
    public overViewCabinetData: any;

    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
        // 设置背景色
        this.rd2.setStyle(document['body'], 'background', 'rgb(232, 232, 232)');

        this.queryOverviewInfo('init');
        this.timedRefresh();
    }

    ngOnDestroy() {
        this.cancelAutoRefresh();
        this.overViewSubscription.unsubscribe();

        // 移除背景色
        this.rd2.setStyle(document['body'], 'background', '');
    }

    private timedRefresh(): void {
        // 跟资产标签相关的数据，表格、机柜标签图、当前选中的标签数据
        this.queryOverviewInfo('refresh');

        this.timerRefresh = setTimeout(() => {
            this.timedRefresh();
        }, 6000);
    }

    private cancelAutoRefresh(): void {
        clearTimeout(this.timerRefresh);
    }

    private queryOverviewInfo(type: string): void {
        this.overViewSubscription = this.homeService.getData().subscribe((overViewData: IOverview) => {
            if (overViewData) {
                this.initServerDetail(overViewData.deviceInfo);
                this.monitorInfo = overViewData.monitorInfo;
                this.overViewCabinetData = overViewData;
                if (type === 'init') {
                    this.initAccess(overViewData.accessInfo);
                }
                if (!this.isRM110) {
                    this.initResource(overViewData.resourceInfo);
                }
            }
        }, () => {
            this.loading = false;
            this.loadingService.state.next(false);
        }, () => {
            this.loading = false;
            this.loadingService.state.next(false);
        });
    }

    private initServerDetail(deviceInfo: IDeviceInfo): void {
        this.serverDetail = {
            productName: deviceInfo.productName,
            position:  deviceInfo.position,
            capacity:  deviceInfo.capacity,
            serialNumber:  deviceInfo.serialNumber,
            firmwareVersion:  deviceInfo.firmwareVersion,
        };
    }

    private initResource(resourceInfo: IResourceInfo): void {
        const {
            totalPower,
            currentPower,
            totalBattary,
            currentBattary,
            totalFun,
            currentFun
        } = resourceInfo;
        this.resources = [
            {
                title: 'HOME_POWER',
                backgroundClassName: 'powerImg',
                id: 'Power',
                targetNav: ['/navigate/system/power'],
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalPower
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: currentPower
                }]
            },
            {
                title: 'IRM_BATTARY',
                backgroundClassName: 'batteryImg',
                id: 'Battary',
                targetNav: ['/navigate/system/battery'],
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalBattary
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: currentBattary
                }]
            },
            {
                title: 'HOME_FAN',
                backgroundClassName: 'funImg',
                id: 'Fan',
                targetNav: ['/navigate/system/fans'],
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalFun
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: currentFun
                }]
            }
        ];
    }

    private initAccess(accessInfo: IAccessInfo): void {
        const hasConfigureUserRight = this.userService.hasPrivileges(['ConfigureUsers']);
        const hasConfigureComponentsRight = this.userService.hasPrivileges(['ConfigureComponents']);
        const hasOemSecurityMgmtRight = this.userService.hasPrivileges(['OemSecurityMgmt']);
        const hasLoginRight = this.userService.hasPrivileges(['Login']);
        this.accesses = [
            // 资产管理
            {
                show: hasLoginRight,
                title: 'IRM_ASSET_MANAGEMENT',
                id: 'AssetManager',
                icon: 'dash-quick-access-system-icon',
                state: '/navigate/system/asset',
                lock: false
            },
            // 本地用户
            {
                show: hasLoginRight,
                title: 'HOME_LOCALUSER',
                id: 'LocalUser',
                icon: 'dash-quick-access-user-icon',
                state: '/navigate/user/localuser',
                disable: !hasLoginRight,
                lock: false
            },
            // 网络配置
            {
                show: hasLoginRight,
                title: 'HOME_NETWORK_CONFIGURATION',
                id: 'Network',
                icon: 'dash-quick-access-netw-icon',
                state: '/navigate/manager/network',
                lock: false
            },
            // 一键收集
            {
                show: hasOemSecurityMgmtRight,
                title: 'HOME_ONE_CLICK_COLLECTION',
                id: 'Collection',
                icon: 'dash-quick-access-conl-icon',
                onKeySelect: true,
                lock: false
            },
            // 固件升级
            {
                show: hasConfigureComponentsRight,
                title: 'HOME_FIRMWARE_UPGRADES',
                id: 'Upgrade',
                icon: 'dash-quick-access-hotw-icon',
                state: '/navigate/manager/upgrade',
                lock: false
            },
            // 恢复配置
            {
                show: hasConfigureUserRight,
                title: 'HOME_RESTORE_CONFIGURATION',
                id: 'ConfigureUser',
                icon: 'dash-quick-access-rest-icon',
                restoreSet: true,
                lock: this.systemLocked
            }
        ];

        this.accesses = this.accesses.filter((access: IAccess) => access.show);
    }
}
