import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { Router } from '@angular/router';
import { UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-report',
    templateUrl: './report.component.html',
    styleUrls: ['./report.component.scss']
})
export class ReportComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private router: Router,
        private user: UserInfoService
    ) { }
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public options = [{
        active: true,
        title: this.translate.instant('ALARM_REPORT_SYSLOG'),
        id: 'syslogId',
        route: 'syslog',
        show: true
    }, {
        active: false,
        title: this.translate.instant('ALARM_REPORT_EMAIL'),
        id: 'emailId',
        route: 'email',
        show: true
    }, {
        active: false,
        title: this.translate.instant('ALARM_REPORT_TRAP'),
        id: 'trapId',
        route: 'trap',
        show: false
    }];
    public tabChange(isActive: boolean, id: string) {
        if (isActive) {
            switch (id) {
                case 'syslogId':
                    this.router.navigate(['/navigate/maintance/report/syslog']);
                    break;
                case 'emailId':
                    this.router.navigate(['/navigate/maintance/report/email']);
                    break;
                case 'trapId':
                    this.router.navigate(['/navigate/maintance/report/trap']);
                    break;
                default:
            }
        }
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        // Trap菜单依赖是否支持SNMP
        this.options[2].show = this.user.snmpSupport;
        const url = this.router.url.split('/');
        const currentRoute = url[url.length - 1];
        this.options.forEach((item) => {
            item.active = false;
            if (item.route === currentRoute) {
                item.active = true;
            }
        });
    }
}
