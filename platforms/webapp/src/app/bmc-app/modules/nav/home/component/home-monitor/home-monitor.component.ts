import { Component, Input, OnInit } from '@angular/core';
import { IEChartOption } from '../../home.datatype';
import { CommonService, SystemLockService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { GlobalData } from 'src/app/common-app/models';
import { HomeService } from '../../services';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { hideFansAndPower } from 'src/app/common-app/utils/fan.util';

@Component({
    selector: 'app-home-monitor',
    templateUrl: './home-monitor.component.html',
    styleUrls: ['./home-monitor.component.scss']
})

export class HomeMonitorComponent implements OnInit {

    @Input() loading = true;

    public refreshTime = '--';
    public refresh2 = false;
    public webStyle: string;
    public systemLocked: boolean = true;
    public chartOptions: IEChartOption[];
    public isLowBrowserVersion: boolean = GlobalData.getInstance().getIsLowBrowserVer;

    constructor(
        private router: Router,
        private commonService: CommonService,
        private lockService: SystemLockService,
        private homeService: HomeService,

    ) {
        this.webStyle = GlobalData.getInstance().getWebStyle;
    }

    ngOnInit(): void {
        this.getRefreshTime();
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
    }

    public navigatorTo(url: string[], param?: string) {
        if (url[0].indexOf('/fans') > -1 && hideFansAndPower()) {
            return;
        }
        if (param) {
            this.router.navigate(url, {queryParams: {id: param}});
        } else {
            this.router.navigate(url);
        }
    }

    public navigatorToMore() {
        const moreLinkUrl = ['/navigate/system/info'];
        this.navigatorTo(moreLinkUrl);
    }

    public getRefreshTime(str: string = 'init') {
        if (str === 'click' && this.systemLocked) {
            return;
        }
        this.refresh2 = true;
        forkJoin([this.homeService.overViewData, this.homeService.genericInfo]).subscribe(([monitorData, timeInfo]) => {
            this.chartOptions = monitorData.monitorInfo.getEchartOption;
            const refreshTimeData = timeInfo.body.CurrentTime;
            this.refreshTime = refreshTimeData.substring(0, 19);
            this.refresh2 = false;
        });
    }
}
