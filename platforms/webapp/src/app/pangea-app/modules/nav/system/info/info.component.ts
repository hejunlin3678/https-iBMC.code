import { Component, OnInit, OnDestroy } from '@angular/core';
import { Router, NavigationEnd } from '@angular/router';
import { UserInfoService } from 'src/app/common-app/service';
import { Subscription } from 'rxjs/internal/Subscription';
import { ITabTitle } from './info.interface';
import { Info } from './info.model';

@Component({
    selector: 'app-info',
    templateUrl: './info.component.html',
    styleUrls: ['./info.component.scss']
})
export class InfoComponent implements OnInit, OnDestroy {

    public sysInfoTitle: ITabTitle[] = [];
    private routerEvent: Subscription;

    public isSystemLock = this.userServ.systemLockDownEnabled;

    constructor(
        private router: Router,
        private userServ: UserInfoService
    ) {
        this.sysInfoTitle = Info.getInfoTitle();
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

