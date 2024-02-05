import { ChangeDetectorRef, Component } from '@angular/core';
import { Router } from '@angular/router';
import { CommonService } from './common-app/service';
import { GlobalDataService } from './common-app/service/global-data.service';
import { HttpService } from './common-app/service/httpService.service';
import { UserInfoService } from './common-app/service/user-info.service';
import { getBrowserType } from './common-app/utils/common';

@Component({
    selector: 'app-root',
    templateUrl: './app.component.html'
})
export class AppComponent {
    constructor(
        private http: HttpService,
        private userInfo: UserInfoService,
        private globalData: GlobalDataService,
        private change: ChangeDetectorRef,
        private commonSever: CommonService,
        private router: Router
    ) {
        window.addEventListener('beforeunload', () => {
            const tabIsValid = JSON.parse(sessionStorage.getItem('tabIsValid'));
            if (!tabIsValid) {
                return;
            }

            // IE11直接注销会话，不判断有几个Tab页
            if (this.isIE11() || this.isSafari()) {
                this.sendDeactive();
            } else {
                const tabs = JSON.parse(localStorage.getItem('tabs'));
                if (tabs !== null && tabs <= 0) {
                    this.sendDeactive();
                }
            }

        });

        // 监听tab页的切换事件
        this.listenResource();

        // 当无权限或其他原因导致的路由跳转错误时
        this.globalData.routeChagneError.subscribe(res => {
            // 参数Res为true时，代表应该要跳转到登录页
            if (res) {
                const resourceId = this.userInfo?.resourceId;
                if (resourceId) {
                    this.http.delete(`/UI/Rest/Sessions/${resourceId}`, { ignoreKeepAlive: true }).subscribe();
                }
                this.commonSever.clear();
                this.router.navigate(['login']);
                location.hash = '';
            } else {
                this.router.navigate(['home']);
            }
        });

        // 触发全局的数据变化检测，解决tip信息文字不能动态更新的问题
        this.globalData.appDetectChagne.subscribe(() => {
            this.change.detectChanges();
        });
    }

    // 切换tab页时，监听到 resourceId不一致时，刷新页面
    private listenResource() {
        document.addEventListener('visibilitychange', () => {
            const userInfo = JSON.parse(localStorage.getItem('userInfo'));
            if (userInfo) {
                const resourceId = this.userInfo.resourceId;
                if (resourceId !== userInfo.resourceId) {
                    location.reload();
                }
            }
        });
    }

    private isIE11(): boolean {
        const userAgent = navigator.userAgent;
        const ie11Rv = /rv:([\d.]+)\) like gecko/;
        const isIE11 = userAgent.indexOf('Trident') > -1 && ie11Rv.test(userAgent.toLocaleLowerCase());
        const isSafari = /Safari/.test(userAgent) && !/Chrome/.test(userAgent);
        return isIE11 || isSafari;
    }

    private isSafari(): boolean {
        const userAgent = navigator.userAgent;
        const isSafari = /Safari/.test(userAgent) && !/Chrome/.test(userAgent);
        return isSafari;
    }

    private sendDeactive(): void {
        const browser = getBrowserType();
        // IE11,火狐需发同步请求
        if (this.isIE11() || browser.browser === 'firefox') {
            const xml = new XMLHttpRequest();
            const data = JSON.stringify({ Mode: 'Deactivate' });
            xml.open('POST', '/UI/Rest/KeepAlive', false);
            xml.withCredentials = true;
            xml.setRequestHeader('From', 'WebUI');
            xml.setRequestHeader('Content-Type', 'application/json');
            xml.setRequestHeader('X-CSRF-Token', this.userInfo.token);
            xml.send(data);
        } else {
            this.http.keepAlive('Deactivate');
        }
    }
}
