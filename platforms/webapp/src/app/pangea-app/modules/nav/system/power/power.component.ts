import { Component, OnInit } from '@angular/core';
import { PowerService } from './service/power.service';
import { Power } from './model/power';
import { PowerCapping } from './model/power-capping';
import { PowerControl } from './model/power-control';
import { PowerCappingHistory } from './model/power-capping-history';
import { TranslateService } from '@ngx-translate/core';
import { ActivatedRoute, Router } from '@angular/router';
import { UserInfoService, LoadingService } from 'src/app/common-app/service';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { Pangen } from 'src/app/pangea-app/models/pangen.model';

@Component({
    selector: 'app-power',
    templateUrl: './power.component.html',
    styleUrls: ['./power.component.scss']
})
export class PowerComponent implements OnInit {

    constructor(
        private route: ActivatedRoute,
        private router: Router,
        private service: PowerService,
        private translate: TranslateService,
        private loading: LoadingService,
        private user: UserInfoService
    ) { }
    public isPrivileges = this.user.hasPrivileges(['OemPowerControl']);
    public isSystemLock = false;
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
    public powerCapping = {
        title: this.translate.instant('POWER_CAPPING'),
        active: false,
        show: true,
        onActiveChange: (isActive: boolean) => {
            if (isActive) {
                this.router.navigate(['/navigate/system/power'], {queryParams: {id: 1}});
            }
        }
    };
    public powerControl = {
        title: this.translate.instant('POWER_CONTORL_PANGEA'),
        active: false,
        onActiveChange: (isActive: boolean) => {
            if (isActive) {
                this.router.navigate(['/navigate/system/power'], {queryParams: {id:  2}});
            }
        }
    };
    public power = new Power();
    public powerCappingInfo: PowerCapping;
    public powerContorlInfo: PowerControl;
    public powerHistory: PowerCappingHistory;

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    init() {
        forkJoin([this.service.getpowerSupply(), this.service.getPower(),
            this.service.getPowerControl()]).subscribe((response) => {
            const powerInfoList = this.service.getPowerInfo(response[0].body);
            this.power.powerInfoList = powerInfoList;
            if (!powerInfoList.length || Pangen.ocean === 'PangeaV6_Atlantic') {
                this.powerInfo.show = false;
            }
            const powerSetting = this.service.getPowerSettingInfo(response[0].body);
            this.power.setIsActiveAndStandby(powerSetting.isActiveAndStandby);
            this.power.setAactivePSU(powerSetting.activePSU);
            this.power.setDeepSleep(powerSetting.deepSleep);
            this.power.normalAndRedundancy = powerSetting.normalAndRedundancy;

            const powerCapping = this.service.getPowerCapping(response[1].body, 'W');
            this.powerCapping.show = powerCapping.powerMetrics;
            this.powerCappingInfo = new PowerCapping(powerCapping);
            // 历史功率charts数据
            const historyInfo = this.service.getPowerHistoryInfo(response[1].body);
            this.powerHistory = new PowerCappingHistory(
                historyInfo.powerPeakWattsList,
                historyInfo.powerAverageWattsList,
                historyInfo.timeList);
            const powerContorlInfo = this.service.getPowerControlInfo(response[2].body);
            this.powerContorlInfo = new PowerControl(powerContorlInfo);
            const chassisType = this.service.getChassisTypeInfo(response[2].body);
            this.powerContorlInfo.setChassisType(chassisType);
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        }, () => {
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
        });
    }
    ngOnInit(): void {
        this.loading.state.next(true);
        this.init();
    }

}
