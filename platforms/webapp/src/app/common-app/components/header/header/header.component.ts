import { Component, OnInit, QueryList, ViewChildren } from '@angular/core';
import { DropDownComponent } from '../drop-down/drop-down.component';
import { HeaderService } from '../header.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { TimeoutService } from 'src/app/common-app/service/timeout.service';
import { Subscription } from 'rxjs/internal/Subscription';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { timer } from 'rxjs';
import { CommonService } from 'src/app/common-app/service/common.service';
import { CommonData, GlobalData } from 'src/app/common-app/models';
import { AlertMessageService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { hideFansAndPower } from 'src/app/common-app/utils/fan.util';

@Component({
    selector: 'app-header',
    templateUrl: './header.component.html',
    styleUrls: ['./header.component.scss']
})
export class HeaderComponent implements OnInit {
    @ViewChildren('drop', { read: DropDownComponent }) dropMenus: QueryList<DropDownComponent>;
    constructor(
        private headerService: HeaderService,
        private global: GlobalDataService,
        private timeout: TimeoutService,
        private user: UserInfoService,
        private commonService: CommonService,
        private alert: AlertMessageService,
        private router: Router
    ) { }
    public sysOverView;
    public name: string;
    public route: string;
    public subscription: Subscription;
    private timer = null;
    private lockTimer: Subscription | null = null;
    public webStyle: string;
    public headerLogSrc: string = '/extern/custom/header_logo.png';

    // 系统管理菜单
    public listSystem = CommonData.mainMenu[1].children;

    // 维护诊断菜单
    public listMaintance = CommonData.mainMenu[2].children;

    // 用户与安全菜单
    public listUsers = CommonData.mainMenu[3].children;

    // 服务管理菜单
    public listService = CommonData.mainMenu[4].children;

    // 网络配置菜单
    public listConfig = CommonData.mainMenu[5].children;

    // 一级导航菜单
    public navData = CommonData.mainMenu;

    // 显示二级菜单
    showLevel2(ev, index) {
        if (index === 0) { return; }
        this.dropMenus['_results'][index - 1].show(ev.target.offsetWidth);
    }

    // 隐藏二级菜单
    hideLevel2(index) {
        if (index === 0) { return; }
        this.dropMenus['_results'][index - 1].hide();
    }

    ngOnInit() {
        this.webStyle = GlobalData.getInstance().getWebStyle;
        // 查询iBMC时间和告警信息
        this.getOverview();
        this.global.resendOverview.subscribe((value) => {
            if (value) {
                this.getOverview();
            }
        });

        // 监听401错误，当触发时，停止相关的自动监听请求
        this.timeout.error401.subscribe((value) => {
            clearInterval(this.timer);
            this.lockTimer.unsubscribe();
        });

        // 定时10秒查询系统锁定状态
        this.lockTimer = timer(0, 10000).subscribe({
            next: () => {
                this.getStystemLockState();
            }
        });
        this.listSystem.forEach(item => {
            if (item.id === 'leftSysPower') {
                if (hideFansAndPower()) {
                    item.lable = 'HOME_POWER';
                } else {
                    item.lable = 'COMMON_SYSTEM_POWER';
                }
            }
        });
    }
    // 资源路径请求错误后获取本地图片
    public getHeaderLogo(obj) {
        obj.headerLogSrc = CommonData.headerLogSrc;
    }

    // 查询IBMA,license,语言，platform,硬分区等功能是否支持。
    private getOverview() {
        this.headerService.getGenericInfo().subscribe((res) => {
            this.global.ibmcTime.next(res);
            // 是否显示FDM菜单
            this.global.fdmResolve(res.fdmSupport);
            this.global.ibmaResolve(res.ibmaSupport);
            this.global.ibmaRunningResolve(res.ibmaRunning);
            this.global.partityResolve(res.partitySupport);
            this.global.licenseResolve(res.licenseSupport);
            this.global.fanResolve(res.fanSupport);
            this.global.snmpResolve(res.snmpSupport);
            // tce电源模块数据的显示隐藏
            this.global.powerCappingResolve(res.powerCappingSupport);
            this.global.powerSleepResolve(res.powerSleepSupport);
            this.global.swiSupportResolve(res.swiSupport);

            // 保存到本地数据
            this.user.ibmaSupport = res.ibmaSupport;
            this.user.fdmSupport = res.fdmSupport;
            this.user.licenseSupport = res.licenseSupport;
            this.user.partitySupport = res.partitySupport;
            this.user.fanSupport = res.fanSupport;
            this.user.snmpSupport = res.snmpSupport;
            this.user.platform = res.platform;
            this.user.iBMARunning = res.ibmaRunning;
            this.user.customizedId = res.customizedId;

            if (this.user.customeCertFlag !== res.customeCertFlag) {
                this.user.customeCertFlag = res.customeCertFlag;
                // 当前为预置证书告警提示
                if (this.user.customeCertFlag === false) {
                    this.alert.eventEmit.emit({type: 'warn', label: 'PRESET_CERT_TIP'});
                }
            }
            this.commonService.saveUserToStorage();
            // 是否显示SP菜单
            this.global.spResolve(res.spSupport);
            this.user.spSupport = res.spSupport;

            // 是否显示TPCM菜单
            this.global.tpcmResolve(res.tpcmSupport);
            this.user.tpcmSupport = res.tpcmSupport;

            // 是否显示USB菜单
            this.global.usbResolve(res.usbSupport);
            this.user.usbSupport = res.usbSupport;
            this.user.modifyVlanIdUnsupported = res.modifyVlanIdUnsupported;

            // 是否显示虚拟控制台
            this.global.kvmResolve(res.kvmSupport);
            this.user.kvmSupport = res.kvmSupport;

            // 是否显示vnc
            this.global.vncResolve(res.vncSupport);
            this.user.vncSupport = res.vncSupport;

            // 是否显示虚拟媒体
            this.global.vmmResolve(res.vmmSupport);
            this.user.vmmSupport = res.vmmSupport;

            // 是否显示性能监控
            this.global.perfSupResolve(res.perfSupSupport);
            this.user.perfSupSupport = res.perfSupSupport;

            // 是否显示录像截屏
            this.global.videoScreenshotResolve(res.videoScreenshotSupport);
            this.user.videoScreenshotSupport = res.videoScreenshotSupport;

            // 头部导航的电源，UID，告警信息
            this.sysOverView = res;

            if (this.timer) {
                clearInterval(this.timer);
            }
            this.timer = setInterval(() => {
                this.getOverview();
            }, 30000);
        });
    }

    private getStystemLockState() {
        this.headerService.getSystemLockState().subscribe((state: boolean) => {
            // 当前锁定状态放在user中，监控锁定状态得subject放在global中，遵循得原则是 userInfo记录基本值
            this.user.systemLockDownEnabled = state;
            this.commonService.saveUserToStorage();
            this.global.systemLockState.next(state);
        });
    }

    ngOnDestroy(): void {
        this.lockTimer.unsubscribe();
        clearInterval(this.timer);
    }
}
