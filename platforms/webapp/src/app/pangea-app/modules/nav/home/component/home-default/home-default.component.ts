import { Component, OnInit, Renderer2 } from '@angular/core';
import { HomeService } from '../../services';
import { LoadingService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { Subscription } from 'rxjs/internal/Subscription';
import { IAccess, IResource, IEChartOption, IServerDetail } from '../../home.datatype';
import { fromEvent } from 'rxjs/internal/observable/fromEvent';
import { getPowerMonitorOption } from '../../utils/monitor.util';
import { IOverview } from '../../models/overview.datatype';
import { Pangen } from '../../../../../models/pangen.model';

@Component({
    selector: 'app-home-default',
    templateUrl: './home-default.component.html',
    styleUrls: ['./home-default.component.scss']
})
export class HomeDefaultComponent implements OnInit {

    constructor(
        private homeService: HomeService,
        private rd2: Renderer2,
        private loadingService: LoadingService,
        private translate: TranslateService
    ) { }

    public loading = true;
    public overViewSubscription: Subscription;
    public accesses: IAccess[] = [];
    public resources: IResource[] = [];
    public echartOptions: IEChartOption[] = [];
    public isHideVirtual: boolean = false;
    public serverDetail: IServerDetail = {
        productName: '',
        aliasName: '',
        productImg: '',
        errorImg: '',
        deviceInfo: null,
    };

    ngOnInit(): void {
        this.loadingService.state.next(true);

        // 设置背景色
        this.rd2.setStyle(document['body'], 'background', 'rgb(232, 232, 232)');

        this.queryOverviewInfo();

        // 页面监听
        fromEvent(window, 'resize')
            .subscribe((event) => {
                // 这里处理页面变化时的操作
                if (this.homeService.monitorInfo && this.echartOptions.length > 0) {
                    this.echartOptions.forEach((chart, index) => {
                        if (chart.id === 'homePowerMonitor') {
                            this.echartOptions[index] = {
                                id: 'homePowerMonitor',
                                option: getPowerMonitorOption(this.homeService.monitorInfo, this.translate),
                                navigatorUrl: ['/navigate/system/power'],
                                routerParam: '1'
                            };
                        }
                    });
                }
            });
        if (Pangen.ocean === 'PangeaV6_Atlantic_Smm') {
            this.isHideVirtual = true;
        }
    }

    ngOnDestroy() {
        this.overViewSubscription.unsubscribe();

        // 移除背景色
        this.rd2.setStyle(document['body'], 'background', '');
    }

    private queryOverviewInfo(): void {
        this.overViewSubscription = this.homeService.getData().subscribe((overViewData: IOverview) => {
            if (overViewData) {
                this.serverDetail = overViewData.deviceInfo.getServerDetail;
                this.resources = overViewData.resourceInfo.getResources;
                this.echartOptions = overViewData.monitorInfo.getEchartOption;
                this.accesses = overViewData.accessInfo.getAccesses;
            }
        }, () => {
            this.loading = false;
            this.loadingService.state.next(false);
        }, () => {
            this.loading = false;
            this.loadingService.state.next(false);
        });
    }
}
