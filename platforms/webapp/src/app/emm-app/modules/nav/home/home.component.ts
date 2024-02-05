import { Component, OnInit, Renderer2 } from '@angular/core';
import { Subscription } from 'rxjs/internal/Subscription';

import { EChartsCoreOption } from 'echarts';

import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { HomeService } from './services';
import {
    IAccess, IServerDetail, IEChartOption, IMonitorParams,
} from './home.datatype';
import { Monitor } from './model';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { CommonData } from 'src/app/common-app/models';
import { ProductName } from 'src/app/emm-app/models/common.datatype';

@Component({
    selector: 'app-home',
    templateUrl: './home.component.html',
    styleUrls: ['./home.component.scss'],
})
export class HomeComponent implements OnInit {
    constructor(
        private homeService: HomeService,
        private rd2: Renderer2,
        private userService: UserInfoService,
        private loadingService: LoadingService,
        private lockService: SystemLockService,
    ) { }

    public loading = true;
    public overViewSubscription: Subscription;
    public accesses: IAccess[] = [];
    public resources = [];
    public echartOptions: IEChartOption[] = [];
    public systemLocked: boolean = true;
    public serverDetail: IServerDetail = {
        options: []
    };
    // 刷新按钮状态标志位
    public refreshing: boolean = false;
    public resourceRefreshing: boolean = false;
    public chassisModel = '';

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

        // 获取数据
        this.queryOverviewInfo();
    }

    ngOnDestroy() {
        this.overViewSubscription.unsubscribe();

        // 移除背景色
        this.rd2.setStyle(document['body'], 'background', '');

        // 删除存储的productName和productSN数据
        localStorage.removeItem('productName');
        localStorage.removeItem('productSN');
    }

    // 请求数据
    public queryOverviewInfo(action: string = 'init'): void {
        this.overViewSubscription = this.homeService.getData().subscribe((overViewData) => {
            switch (action) {
                case 'refreshResource':
                    this.initResource(overViewData.resourceInfo);
                    break;
                case 'refreshMonitor':
                    this.initMonitor(overViewData.monitorInfo);
                    break;
                default:
                    this.initServerDetail(overViewData.chassisInfo);
                    this.initResource(overViewData.resourceInfo);
                    this.initMonitor(overViewData.monitorInfo);
                    this.initAccess();
                    this.chassisModel = overViewData.chassisInfo.chassisModel;
                    // 如果有前视图和后视图数据 那么进行消息发送
                    if (overViewData.frontViewAndRearViewInfo) {
                        this.homeService.subjectObj.next({
                            data: overViewData.frontViewAndRearViewInfo
                        });
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

    // 加工机箱消息的数据
    private initServerDetail(deviceInfo): void {
        this.serverDetail = {
            options: [
                {
                    id: 'chassisName',
                    label: 'HOME_CHASSIS_NAME',
                    detail: deviceInfo.chassisName || CommonData.isEmpty,
                    value: deviceInfo.chassisName,
                    isEdit: false,
                    validation: null,
                    formControl: null
                },
                {
                    id: 'backPlanePcbVersion',
                    label: 'HOME_BACKPLANE_TYPE',
                    detail: deviceInfo.backPlanePcbVersion || CommonData.isEmpty,
                    isEdit: false,
                },
                {
                    id: 'chassisLocation',
                    label: 'HOME_CHASSIS_LOCATION',
                    detail: deviceInfo.chassisLocation || CommonData.isEmpty,
                    value: deviceInfo.chassisLocation,
                    isEdit: false,
                    validation: null,
                    formControl: null
                },
                {
                    id: 'chassisModel',
                    label: 'HOME_CHASSIS_MODEL',
                    detail: deviceInfo.chassisType || CommonData.isEmpty,
                    isEdit: false
                },
                {
                    id: 'chassisID',
                    label: 'HOME_CHASSIS_ID',
                    detail: deviceInfo.chassisID !== null ? deviceInfo.chassisID : CommonData.isEmpty,
                    value: deviceInfo.chassisID,
                    isEdit: false,
                    validation: null,
                    formControl: null
                },
                {
                    id: 'chassisPartNumber',
                    label: 'HOME_CHASSIS_PART_NUMBER',
                    detail: deviceInfo.chassisPartNumber || CommonData.isEmpty,
                    isEdit: false
                },
                {
                    id: 'chassisHeight',
                    label: 'HOME_CHASSIS_HEIGHT',
                    detail: deviceInfo.chassisHeight || CommonData.isEmpty,
                    isEdit: false
                },
                {
                    id: 'chassisSerialNumber',
                    label: 'HOME_CHASSIS_SERIAL_NUMBER',
                    detail: deviceInfo.chassisSerialNumber || CommonData.isEmpty,
                    isEdit: false
                },
            ],
        };
    }

    // 加工健康状态的数据
    private initResource(resourceInfo): void {
        const {
            bladeHealth,
            bladeStatus,
            installedBlade,
            totalBlade,
            showBladeHealth,
            powerHealth,
            powerStatus,
            installedPower,
            totalPower,
            showPowerHealth,
            fanHealth,
            fanStatus,
            installedFan,
            totalFan,
            showFanHealth,
            swiHealth,
            swiStatus,
            installedSwi,
            totalSwi,
            showSwiHealth,
            hmmHealth,
            hmmStatus,
            installedHmm,
            totalHmm,
            showHmmHealth
        } = resourceInfo;
        const resources = [
            {
                title: 'HOME_INDEX_BLADES',
                status: bladeStatus,
                backgroundClassName: 'bladesImg',
                id: 'bladesId',
                targetNav: ['/navigate/system/info/memory'],
                hide: !showBladeHealth,
                healthStatus: bladeHealth,
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalBlade
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: installedBlade
                }]
            },
            {
                title: 'HOME_POWER',
                status: powerStatus,
                backgroundClassName: 'powerImg',
                id: 'psusId',
                targetNav: ['/navigate/system/power'],
                hide: !showPowerHealth,
                healthStatus: powerHealth,
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalPower
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: installedPower
                }]
            },
            {
                title: 'HOME_FAN',
                status: fanStatus,
                backgroundClassName: 'fanImg',
                id: 'fansId',
                targetNav: ['/navigate/system/fans'],
                hide: !showFanHealth,
                healthStatus: fanHealth,
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalFan
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: installedFan
                }]
            },
            {
                title: 'HOME_INDEX_SWIS',
                status: swiStatus,
                backgroundClassName: 'swisImg',
                id: 'swiId',
                targetNav: ['/navigate/system/storage'],
                hide: !showSwiHealth,
                healthStatus: swiHealth,
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalSwi
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: installedSwi
                }]
            },
            {
                title: this.chassisModel === ProductName.Emm12U ? 'HOME_INDEX_MMS' : 'HOME_INDEX_SMMS',
                status: hmmStatus,
                backgroundClassName: 'mmsImg',
                id: 'mmsId',
                targetNav: ['/navigate/system/info/net'],
                hide: !showHmmHealth,
                healthStatus: hmmHealth,
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: totalHmm
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: installedHmm
                }]
            },
        ];
        this.resources = resources.filter(resource => !resource.hide);
        this.resourceRefreshing = false;
    }

    private initMonitor(monitorInfo): void {
        this.echartOptions = [];
        if (monitorInfo.powerUsage !== undefined) {
            this.echartOptions.push({
                id: 'homepowerMonitor',
                option: this.getpowerMonitorOption(monitorInfo),
                title: 'HOME_POWER_USAGE',
                navigatorUrl: ['/navigate/system/monitor']
            });
        }
        if (monitorInfo.fanAverageSpeed !== undefined) {
            this.echartOptions.push({
                id: 'homefanMonitor',
                option: this.getFanMonitorOption(monitorInfo),
                title: 'HOME_AVERAGE_FAN_SPEED',
                navigatorUrl: ['/navigate/system/monitor']
            });
        }
        this.refreshing = false;
    }

    private initAccess(): void {
        let showLocalUser: boolean;
        const hasConfigureUserRight = this.userService.hasPrivileges(['ConfigureUsers']);
        const hasConfigureComponentsRight = this.userService.hasPrivileges(['ConfigureComponents']);
        const hasOemSecurityMgmtRight = this.userService.hasPrivileges(['OemSecurityMgmt']);
        const hasLoginRight = this.userService.hasPrivileges(['Login']);
        if (this.userService.userId) {
            showLocalUser = hasLoginRight;
        } else {
            showLocalUser = hasConfigureUserRight;
        }
        this.accesses = [
            // 本地用户
            {
                show: showLocalUser,
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
            // 电源控制
            {
                show: hasLoginRight,
                title: 'HOME_POWER_CONTROL',
                id: 'PowerControl',
                icon: 'dash-quick-access-powe-icon',
                state: '/navigate/system/power',
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
            // 一键收集
            {
                show: hasOemSecurityMgmtRight,
                title: 'HOME_ONE_CLICK_COLLECTION',
                id: 'Collection',
                icon: 'dash-quick-access-conl-icon',
                onKeySelect: true,
                lock: false
            }
        ];

        this.accesses = this.accesses.filter((access: IAccess) => access.show);
    }
    public refreshChart(): void {
        this.refreshing = true;
        this.queryOverviewInfo('refreshMonitor');
    }

    public refreshResoure(): void {
        this.resourceRefreshing = true;
        this.queryOverviewInfo('refreshResource');
    }

    private getFanMonitorOption(monitorInfo): EChartsCoreOption {
        const limitFan = 100;
        const fanData = monitorInfo.fanAverageSpeed || 0;
        const formatter = `{div|\n${fanData}}{p| %\n}`;
        const params: IMonitorParams = {
            computedValue: fanData,
            value: fanData,
            formatter,
            minorValue: limitFan
        };
        return new Monitor(params).pieOptions;
    }

    private getpowerMonitorOption(monitorInfo): EChartsCoreOption {
        const limitPower = 100;
        const powerData = monitorInfo.powerUsage || 0;
        const formatter = `{div|\n${powerData}}{p| %\n}`;
        const params: IMonitorParams = {
            computedValue: powerData,
            value: powerData,
            formatter,
            minorValue: limitPower,
        };
        return new Monitor(params).pieOptions;
    }

}
