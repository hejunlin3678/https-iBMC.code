import { Component, OnInit } from '@angular/core';
import { UserInfoService } from 'src/app/common-app/service';
import { ILoginInfor } from '../../home.datatype';

@Component({
    selector: 'app-home-login-infor',
    templateUrl: './home-login-infor.component.html',
    styleUrls: ['./home-login-infor.component.scss']
})
export class HomeLoginInforComponent implements OnInit {

    public showLoginInfor = false;

    public lastLoginInfor: ILoginInfor[];

    constructor(
        private userService: UserInfoService,
    ) { }

    ngOnInit() {
        this.initLoginInfor();
    }

    public close = () => {
        this.showLoginInfor = false;
        this.userService.lastLoginIp = '';
    }

    private initLoginInfor(): void {
        const { lastLoginIp, lastLoginTime, userName } = this.userService;
        this.showLoginInfor = Boolean(lastLoginIp && lastLoginTime);
        if (this.showLoginInfor) {
            this.lastLoginInfor = [
                {
                    label: 'COMMON_USERNAME',
                    value: userName
                }, {
                    label: 'COMMON_LOGIN_IP',
                    value: lastLoginIp
                }, {
                    label: 'COMMON_LOGIN_TIME',
                    value: lastLoginTime
                }
            ];

            // 10秒后关闭
            setTimeout(this.close, 10000);
        }
    }

}
