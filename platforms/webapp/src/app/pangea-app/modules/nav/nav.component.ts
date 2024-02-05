import { Component, OnInit } from '@angular/core';
import { fromEvent } from 'rxjs/internal/observable/fromEvent';
import { debounceTime } from 'rxjs/internal/operators/debounceTime';
import { Router } from '@angular/router';
import { AlertMessageService, HttpService } from 'src/app/common-app/service';
import { Subscription } from 'rxjs/internal/Subscription';
import { ArcticNavMenuService, AtlanticNavMenuService, AtlanticSMMNavMenuService, PacificNavMenuService } from '../../services';
import { Pangen } from '../../models/pangen.model';
import { OceanType } from '../../models/common.datatype';

@Component({
    selector: 'app-nav',
    templateUrl: './nav.component.html',
    styleUrls: ['./nav.component.scss']
})
export class NavComponent implements OnInit {

    constructor(
        private alertMessage: AlertMessageService,
        private arcticNavService: ArcticNavMenuService,
        private atlanticNavService: AtlanticNavMenuService,
        private atlanticSMMNavService: AtlanticSMMNavMenuService,
        private pacificNavService: PacificNavMenuService,
        private http: HttpService,
        private route: Router
    ) {
        // 每次登录创建nav组件后，重新刷新菜单项
        switch (Pangen.ocean) {
            case OceanType.Arctic:
                this.arcticNavService.refreshMenu();
                break;
            case OceanType.Atlantic:
                this.atlanticNavService.refreshMenu();
                break;
            case OceanType.AtlanticSMM:
                this.atlanticSMMNavService.refreshMenu();
                break;
            case OceanType.Pacific:
                this.pacificNavService.refreshMenu();
                break;
            default:
                this.arcticNavService.refreshMenu();
                break;
        }
    }

    public type: string;
    public label: string;
    public open: boolean;
    public time: number;
    public subscription: Subscription;
    public errorKey: string;
    private click$: Subscription;

    ngOnInit(): void {
        this.subscription = this.alertMessage.eventEmit.subscribe((data) => {
            // 首先关闭上一次的弹出提示
            this.open = false;

            if (data.type === 'success') {
                this.time = 5000;
            } else {
                this.time = 10000;
            }
            this.type = data.type;
            this.label = data.label;
            this.open = data.open === undefined ? true : data.open;
        });

        // 绑定鼠标单击事件，发送KeepAlive请求
        this.click$ = fromEvent(document, 'click').pipe(
            debounceTime(200)
        ).subscribe(() => {
            // 全局click事件放置在navComponent里面执行，解决KVM页面也绑定了click事件的 bug，同时为了预防录像播放再次出现类似问题，加入路由判断
            const url = this.route.url;
            if (url.indexOf('navigate') > -1) {
                this.http.keepAlive('Activate');
            }
        });
    }

    ngOnDestroy(): void {
        this.subscription.unsubscribe();
        this.click$.unsubscribe();
    }

}
