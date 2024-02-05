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

@Injectable({
    providedIn: 'root'
})
export class PartityGuard implements CanActivate {
    constructor(
        private userInfo: UserInfoService,
        private globalData: GlobalDataService,
        private commonService: CommonService
    ) {

    }
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        if (this.userInfo.partitySupport !== null && this.userInfo.partitySupport !== undefined) {
            return this.userInfo.partitySupport;
        } else {
            return new Promise((resolve, reject) => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res) => {
                        // 非9008V5和双系统中的从系统，取值为null，可用来确定硬分区快捷入口是否显示（也就是环境是否支持硬分区）。
                        const paritySupport = res.body.FusionPartSupported;

                        if (!paritySupport) {
                            reject();
                            this.globalData.routeChagneError.next(true);
                        } else {
                            resolve(res);
                        }
                    },
                    error: () => {
                        reject();
                        this.globalData.routeChagneError.next(true);
                    }
                });
            });
        }
    }
}
