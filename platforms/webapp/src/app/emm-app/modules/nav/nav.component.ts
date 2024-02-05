import { Component, OnInit } from '@angular/core';
import { fromEvent } from 'rxjs/internal/observable/fromEvent';
import { debounceTime } from 'rxjs/internal/operators/debounceTime';
import { Router } from '@angular/router';
import { AlertMessageService, CommonService, HttpService } from 'src/app/common-app/service';
import { NavMenuService } from '../../services/navMenu.service';
import { Subscription } from 'rxjs/internal/Subscription';

@Component({
    selector: 'app-nav',
    templateUrl: './nav.component.html',
    styleUrls: ['./nav.component.scss']
})
export class NavComponent implements OnInit {

    constructor(
        private alertMessage: AlertMessageService,
        private navService: NavMenuService,
        private http: HttpService,
        private route: Router,
        private commonService: CommonService
    ) {
        // 每次登录创建nav组件后，重新刷新菜单项
        this.navService.refreshMenu();
    }

    public type: string;
    public label: string;
    public open: boolean;
    public time: number;
    public subscription: Subscription;
    public errorKey: string;
    private click$: Subscription;

    ngOnInit(): void {
        // 当tabIsValid 不存在时，代表是新开的Tab标签
        const tabIsValid = sessionStorage.getItem('tabIsValid');
        if (!tabIsValid) {
            this.commonService.updateLocalLoginCount('add');
        }

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
