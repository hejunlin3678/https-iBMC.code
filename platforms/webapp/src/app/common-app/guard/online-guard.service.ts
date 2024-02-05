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

@Injectable({
    providedIn: 'root'
})
export class OnlineGuard implements CanActivate {
    constructor(
        private userInfo: UserInfoService,
        private globalData: GlobalDataService
    ) {

    }
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        const hasPrivil = this.userInfo.hasPrivileges(['ConfigureUsers']);
        if (!hasPrivil) {
            // 触发路由跳转错误
            this.globalData.routeChagneError.next(true);
        }
        return hasPrivil;
    }
}
