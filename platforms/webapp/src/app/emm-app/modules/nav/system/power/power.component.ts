import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { TranslateService } from '@ngx-translate/core';
import { GlobalDataService, LoadingService, UserInfoService } from 'src/app/common-app/service';


@Component({
    selector: 'app-power',
    templateUrl: './power.component.html',
    styleUrls: ['./power.component.scss']
})
export class PowerComponent implements OnInit {

    constructor(
        private route: ActivatedRoute,
        private router: Router,
        private translate: TranslateService,
        private loading: LoadingService,
        private user: UserInfoService,
        private globalData: GlobalDataService,

    ) { }
    public isPrivileges = this.user.hasPrivileges(['OemPowerControl']);
    public isSystemLock = false;

    // 电源信息
    public powerInfo = {
        title: this.translate.instant('POWER_INFO'),
        active: true,
        show: true,
        onActiveChange: (isActive: boolean) => {
            if (isActive) {
                this.router.navigate(['/navigate/system/power'], {queryParams: {id: 0}});
            }
        }
    };

    // 历史功率
    public powerCapping = {
        title: this.translate.instant('POWER_HISTORY'),
        active: false,
        show: true,
        onActiveChange: (isActive: boolean) => {
            if (isActive) {
                this.router.navigate(['/navigate/system/power'], {queryParams: {id: 1}});
            }
        }
    };

    // 功率顶峰设置
    public powerControl = {
        title: this.translate.instant('POWER_CAP_SETTING'),
        active: false,
        show: this.showPowerControl(),
        onActiveChange: (isActive: boolean) => {
            if (isActive) {
                this.router.navigate(['/navigate/system/power'], {queryParams: {id:  2}});
            }
        }
    };

    // 根据接口返回值判断是否显示“功率封顶设置”
    public showPowerControl() {
        this.globalData.powerSleepSupport.then((res: boolean) => {
            return res;
        });
    }

    init() {
        this.route.queryParams.subscribe((data) => {
            if (data.id) {
                switch (Number(data.id)) {
                    case 0:
                        this.powerCapping.active = false;
                        this.powerControl.active = false;
                        this.powerInfo.active = true;
                        break;
                    case 1:
                        this.powerInfo.active = false;
                        this.powerControl.active = false;
                        this.powerCapping.active = true;
                        break;
                    case 2:
                        this.powerInfo.active = false;
                        this.powerCapping.active = false;
                        this.powerControl.active = true;
                        break;
                }
            }
        });
    }
    ngOnInit(): void {
        this.loading.state.next(true);
        this.init();
    }

}
