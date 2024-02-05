import { Injectable } from '@angular/core';
import { CanActivateChild, ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';

import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { CommonData } from '../models';

@Injectable({
    providedIn: 'root'
})
export class AuthGuard implements CanActivateChild {
    constructor(
        private userInfo: UserInfoService,
        private global: GlobalDataService
    ) {

    }
    canActivateChild(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return this.global.webType.then(() => {
            // 路由权限控制,如果权限列表为空，则不跳转
            if (this.userInfo.privileges.length === 0) {
                this.global.routeChagneError.next(true);
                return false;
            }

            // 判断路由是否是属于某产品，不属于则不跳转
            const url = state.url.split('?')[0];
            if (!Object.prototype.hasOwnProperty.call(CommonData.routeConfig, url)) {
                this.global.routeChagneError.next(true);
                return false;
            }

            // 判断用户的权限是否满足该路由的权限设定
            const access = CommonData.routeConfig[url]?.access;
            const userPrivileges = this.userInfo.privileges.filter((role) => {
                return access.indexOf(role) > -1;
            });
            if (access.length !== userPrivileges.length) {
                this.global.routeChagneError.next(true);
                return false;
            }

            return true;
        });
    }
}
