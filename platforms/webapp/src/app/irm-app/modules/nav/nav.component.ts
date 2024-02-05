import { Component, OnInit } from '@angular/core';
import { AlertMessageService, CommonService } from 'src/app/common-app/service';
import { Subscription } from 'rxjs';
import { NavMenuService } from '../../service/navMenu.service';

@Component({
    selector: 'app-nav',
    templateUrl: './nav.component.html',
    styleUrls: ['./nav.component.scss']
})
export class NavComponent implements OnInit {

    constructor(
        private alertMessage: AlertMessageService,
        private navService: NavMenuService,
        private commonService: CommonService
    ) {
         // 当tabIsValid 不存在时，代表是新开的Tab标签
         const tabIsValid = sessionStorage.getItem('tabIsValid');
         if (!tabIsValid) {
             this.commonService.updateLocalLoginCount('add');
         }
        // 每次登录创建nav组件后，重新刷新菜单项
        this.navService.refreshMenu();
    }

    public type: string;
    public label: string;
    public open: boolean;
    public time: number;
    public subscription: Subscription;
    public errorKey: string;

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
    }

    ngOnDestroy(): void {
        this.subscription.unsubscribe();
    }

}
