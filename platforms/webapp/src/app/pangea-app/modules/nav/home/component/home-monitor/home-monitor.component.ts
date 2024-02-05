import { Component, OnInit, Input } from '@angular/core';
import { IEChartOption } from '../../home.datatype';
import { CommonService, SystemLockService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { GlobalData } from 'src/app/common-app/models';
import { Pangen } from 'src/app/pangea-app/models/pangen.model';

@Component({
    selector: 'app-home-monitor',
    templateUrl: './home-monitor.component.html',
    styleUrls: ['./home-monitor.component.scss']
})
export class HomeMonitorComponent implements OnInit {

    @Input() echartOptions: IEChartOption[] = [];
    @Input() loading = true;

    @Input() refreshTime = '--';

    @Input() refresh2 = false;

    public systemLocked: boolean = true;

    public isLowBrowserVersion: boolean = GlobalData.getInstance().getIsLowBrowserVer;
    public isHideSystemMonitor: boolean = false;

    constructor(
        private router: Router,
        private commonService: CommonService,
        private lockService: SystemLockService
    ) { }

    ngOnInit(): void {

        this.getRefreshTime('init');

        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
        // 大西洋控制器不显示“系统监控”
        if (Pangen.ocean === 'PangeaV6_Atlantic') {
            this.isHideSystemMonitor = true;
        }
    }


    public navigatorTo(url: string[], param?: string) {
        if (param) {
            this.router.navigate(url, {queryParams: {id: param}});
        } else {
            this.router.navigate(url);
        }
        this.getRefreshTime('init');
    }

    public navigatorToMore() {
        const moreLinkUrl = ['/navigate/system/info'];
        this.navigatorTo(moreLinkUrl);
        this.getRefreshTime('init');
    }

    public getRefreshTime(str: string) {
        if (str === 'click' && this.systemLocked) {
            return;
        }
        this.refresh2 = true;
        this.commonService.getGenericInfo().subscribe({
            next: (res) => {
                const refreshTimeData = res.body.CurrentTime;
                this.refreshTime = refreshTimeData.substring(0, 19);
                this.refresh2 = false;
            },
        });
    }

}
