import { Component, OnInit } from '@angular/core';
import { CommonService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-security',
    templateUrl: './security.component.html',
    styleUrls: ['./security.component.scss']
})
export class SecurityComponent implements OnInit {

    public systemLockDownEnabled = this.userService.systemLockDownEnabled;

    public tabs = [
        {
            id: 'securityEnhance',
            title: 'SECURITY_ENHANCE',
            active: true,
        },
        {
            id: 'loginRule',
            title: 'SECURITY_LOGIN_RULES',
            active: false,
        },
        {
            id: 'authorityManagement',
            title: 'SECURITY_AUTHORITY_MANAGEMENT',
            active: false,
            hide: this.userService.privileges.indexOf('ConfigureUsers') < 0
        },
        {
            id: 'securityNotice',
            title: 'SECURITY_LOGIN_SECURITY',
            active: false,
        },
    ];

    constructor(
        private userService: UserInfoService,
        private commonServ: CommonService,
    ) {
        const securityTab = sessionStorage.getItem('securityTab');
        if (securityTab) {
            this.tabs.forEach((tab) => {
                tab.active = tab.id === securityTab;
            });
            sessionStorage.removeItem('securityTab');
        }
    }

    ngOnInit(): void {
    }

    public changeSystemLocked(value) {
        this.userService.systemLockDownEnabled = value;
        // 更新userService的值之后，需要同步到localStorage中
        this.commonServ.saveUserToStorage();
        this.systemLockDownEnabled = value;
    }

}
