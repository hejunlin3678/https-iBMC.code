import { Component, Input, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-upgrade-manual',
    templateUrl: './upgrade-manual.component.html',
    styleUrls: ['./upgrade-manual.component.scss']
})
export class UpgradeManualComponent implements OnInit {
    constructor(
        private user: UserInfoService,
        private translate: TranslateService
    ) { }
    @Input() isSystemLock;

    public tabIn: any = {
        title: this.translate.instant('IBMC_UPGRADE_INTERNAL_FIRMWARE'),
        active: false,
        hide: !this.user.spSupport
    };
    public tabOut: any = {
        title: this.translate.instant('IBMC_UPGRADE_OUT_FIRMWARE'),
        active: true
    };
    ngOnInit(): void {

    }
}
