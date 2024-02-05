import { Component, OnInit, Input } from '@angular/core';
import { HeaderService } from '../../header/header.service';
import { Router } from '@angular/router';
import { CommonService } from 'src/app/common-app/service/common.service';
import * as utils from 'src/app/common-app/utils';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-login-info',
    templateUrl: './login-info.component.html',
    styleUrls: ['./login-info.component.scss']
})
export class LoginInfoComponent implements OnInit {
    @Input() id;
    public mouseState: string = 'leave';
    public showDropLang: string = 'none';

    constructor(
        private headerService: HeaderService,
        private router: Router,
        private common: CommonService,
        private userInfo: UserInfoService,
        private translate: TranslateService
    ) { }

    public list = [
        {
            label: 'COMMON_USERNAME',
            value: '',
            id: 'navUserName'
        },
        {
            label: 'COMMON_ROLE',
            value: '',
            id: 'navRole'
        },
        {
            label: 'COMMON_IPADRESS',
            value: '',
            id: 'navIpAddress'
        },
        {
            label: 'COMMON_TIME',
            value: '',
            id: 'navTime'
        }
    ];

    ngOnInit() {
        const roles: string = this.userInfo.userRole.map(role => this.translate.instant(`USERINFO_ROLE_${role.toUpperCase()}`)).join();

        this.list = [
            {
                label: 'COMMON_USERNAME',
                value: this.userInfo.userName,
                id: 'navUserName'
            },
            {
                label: 'COMMON_ROLE',
                value: roles,
                id: 'navRole'
            },
            {
                label: 'COMMON_IPADRESS',
                value: this.userInfo.userIp,
                id: 'navIpAddress'
            },
            {
                label: 'COMMON_TIME',
                value: this.userInfo.loginTime,
                id: 'navTime'
            }
        ];
    }

    public showDropMenu() {
        this.mouseState = 'enter';
        this.showDropLang = 'block';
    }

    public hideDropMenu(ev) {
        this.mouseState = 'leave';
        if (utils.getMouseOutPosition(ev) === 'top') {
            this.showDropLang = 'none';
        }
    }

    public hideMenu(ev) {
        this.mouseState = 'leave';
        if (utils.getMouseOutPosition(ev) === 'bottom') {
            this.showDropLang = 'none';
        }
    }

    public loginOut(ev: MouseEvent) {
        this.headerService.loginOut().subscribe((res) => {
            this.userInfo.isExited = true;
            this.common.clear();
            this.router.navigate(['login']);
        });
        // 阻止冒泡，避免触发 document的点击事件而再次发送keepAlive请求
        ev.stopPropagation();
    }

}
