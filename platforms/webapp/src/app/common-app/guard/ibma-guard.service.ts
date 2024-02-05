import { Injectable } from '@angular/core';
import {
    CanActivate,
    ActivatedRouteSnapshot,
    RouterStateSnapshot,
    UrlTree
} from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';

import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { CommonService } from 'src/app/common-app/service/common.service';
import { HttpResponse } from '@angular/common/http';

@Injectable({
    providedIn: 'root'
})
export class IbmaGuard implements CanActivate {
    constructor(
        private userInfo: UserInfoService,
        private globalData: GlobalDataService,
        private commonService: CommonService
    ) {

    }
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        if (this.userInfo.ibmaSupport !== null && this.userInfo.ibmaSupport !== undefined) {
            return this.userInfo.ibmaSupport;
        } else {
            return new Promise((resolve) => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res: HttpResponse<any>) => {
                        // 查询IBMA是否支持，影响菜单：IBMA管理，性能监控
                        const ibmaSupport = res.body.SmsSupported;
                        if (ibmaSupport) {
                            resolve(true);
                        } else {
                            resolve(false);
                            this.globalData.routeChagneError.next(true);
                        }
                    },
                    error: (error) => {
                        resolve(false);
                        this.globalData.routeChagneError.next(true);
                    }
                });
            });
        }
    }
}
