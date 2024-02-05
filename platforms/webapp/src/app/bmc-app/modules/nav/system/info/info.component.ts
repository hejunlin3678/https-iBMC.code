import { Component, OnInit, OnDestroy } from '@angular/core';
import { Router, NavigationEnd } from '@angular/router';
import { GlobalDataService, PRODUCTTYPE, UserInfoService } from 'src/app/common-app/service';
import { Subscription } from 'rxjs/internal/Subscription';

@Component({
    selector: 'app-info',
    templateUrl: './info.component.html',
    styleUrls: ['./info.component.scss']
})
export class InfoComponent implements OnInit, OnDestroy {

    public sysInfoTitle: ITabTitle[];
    private routerEvent: Subscription;

    public isSystemLock = this.userServ.systemLockDownEnabled;
    public showAssembly: boolean =
        this.globalData.productType === PRODUCTTYPE.ATLAS;

    constructor(
        private router: Router,
        private userServ: UserInfoService,
        private globalData: GlobalDataService,
    ) {
        this.sysInfoTitle = [
            {
                title: 'SYSINFO_PRODUCTINFO',
                isActived: true,
                path: '/navigate/system/info/product',
                id: 'systemInfoProduct',
                show: true
            },
            {
                title: 'HOME_PROCESSOR',
                isActived: false,
                path: '/navigate/system/info/processor',
                id: 'systemInfoProcessor',
                show: true
            },
            {
                title: 'SYSINFO_MEMORY',
                isActived: false,
                path: '/navigate/system/info/memory',
                id: 'systemInfoMemory',
                show: true
            },
            {
                title: 'COMMON_NETWORK',
                isActived: false,
                path: '/navigate/system/info/net',
                id: 'systemInfoNic',
                show: true
            },
            {
                title: 'SYSINFO_SENSOR',
                isActived: false,
                path: '/navigate/system/info/sensor',
                id: 'systemInfoSensor',
                show: true
            },
            {
                title: 'SYSINFO_ASSEMBLY',
                isActived: false,
                path: '/navigate/system/info/units',
                id: 'systemInfoUnits',
                show: this.showAssembly
            },
            {
                title: 'SYSINFO_ONTERS',
                isActived: false,
                path: '/navigate/system/info/others',
                id: 'systemInfoOthers',
                show: true
            }
        ];
    }

    changeTitle(index: number): void {
        this.sysInfoTitle.forEach((val) => val.isActived = false);
        this.sysInfoTitle[index].isActived = true;
        this.router.navigateByUrl(this.sysInfoTitle[index].path);
    }

    // 刷新页面时，tab底部基线与路由匹配
    private changeTabBaseline() {
        const currentIndex = this.sysInfoTitle.findIndex((tab) => this.router.url.includes(tab.path));
        this.changeTitle(currentIndex);
    }

    ngOnInit(): void {
        this.changeTabBaseline();
        this.routerEvent = this.router.events.subscribe((data) => {
            if (data instanceof NavigationEnd) {
                this.changeTabBaseline();
            }
        });
    }

    ngOnDestroy(): void {
        this.routerEvent.unsubscribe();
    }

}

interface ITabTitle {
    title: string;
    isActived: boolean;
    path: string;
    id: string;
    show: boolean;
}

