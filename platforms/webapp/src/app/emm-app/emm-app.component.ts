import { Component } from '@angular/core';
import {
    TimeoutService,
    GlobalDataService,
    UserInfoService,
    CommonService
} from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { TiModalRef, TiMessageService } from '@cloud/tiny3';
import { Router } from '@angular/router';
import { DomSanitizer } from '@angular/platform-browser';

@Component({
    selector: 'app-emm-app',
    templateUrl: './emm-app.component.html'
})
export class EMMAppComponent {
    private modalInstance: TiModalRef = null;
    constructor(
        private commonSever: CommonService,
        private translate: TranslateService,
        private timeoutService: TimeoutService,
        private tiMessage: TiMessageService,
        private globalData: GlobalDataService,
        private userInfo: UserInfoService,
        private domSanitizer: DomSanitizer,
        private router: Router
    ) {
        // 查询 resourceId，当不存在时，直接跳转到登录页
        if (!this.userInfo.resourceId) {
            this.router.navigate(['/login']);
        }
    }
    ngOnInit(): void {
        // 订阅连接超时提醒的服务
        this.timeoutService.error401.subscribe((errorId) => {
            // 如果弹出框已经出现，则不再弹出
            if (this.modalInstance) {
                return;
            }

            // 如果路由是在登录页或者路由为空的情况，不弹出框
            if (this.router.url.indexOf('login') > -1 || this.router.url === '' || this.router.url === '/') {
                return;
            }
            this.globalData.sessionTimeOut = true;

            let errorMsg = this.translate.instant(errorId);
            if (typeof errorMsg === 'object') {
                errorMsg = errorMsg.errorMessage || errorMsg.attrMessage;
            }

            this.commonSever.clear();
            const msg = `<span id="timeoutContent">${errorMsg}</span>`;
            this.modalInstance = this.tiMessage.open({
                id: 'timeoutModal',
                type: 'error',
                content: this.domSanitizer.bypassSecurityTrustHtml(msg),
                cancelButton: {
                    show: false
                },
                closeIcon: false,
                close: () => {
                    this.modalInstance = null;
                    if (window.parent.frames.frameElement === null) {
                        window.location.hash = '';
                        // 重刷新主要是解决SSL证书更新后，其他浏览器不刷新，登录会失败
                        setTimeout(() => {
                            // tslint:disable-next-line: deprecation
                            location.reload();
                        }, 1000);
                    } else {
                        // 解决csp刷新页面导致ifram及页面排版错乱问题
                        this.router.navigate(['login']);
                    }
                }
            });
        });
    }
}
