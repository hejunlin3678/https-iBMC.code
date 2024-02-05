import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { FanService } from './services/fan.service';
import { LoadingService, UserInfoService, SystemLockService } from 'src/app/common-app/service';
import { FanInfo } from 'src/app/common-app/models/fanInfo';
import { getFanInfo } from 'src/app/common-app/utils/fan.util';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-fans',
    templateUrl: './fans.component.html',
    styleUrls: ['./fans.component.scss'],
    encapsulation: ViewEncapsulation.None
})
export class FansComponent implements OnInit {

    constructor(
        private service: FanService,
        private loading: LoadingService,
        private user: UserInfoService,
        private lockService: SystemLockService,
        private translate: TranslateService
    ) { }

    public fanInfoList: FanInfo[] = [];
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock: boolean = false;
    public fanState: boolean = false;

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    public init() {
        this.service.getThermals().subscribe((response) => {
            this.fanInfoList = getFanInfo(response.body, this.translate);
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }
    ngOnInit(): void {
        this.loading.state.next(true);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
        });
        this.init();
    }
}
