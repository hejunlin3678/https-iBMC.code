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
export class USBGuard implements CanActivate {
    constructor(
        private userInfo: UserInfoService,
        private globalData: GlobalDataService,
        private commonService: CommonService
    ) {

    }
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        if (this.userInfo.usbSupport !== null && this.userInfo.usbSupport !== undefined) {
            return this.userInfo.usbSupport;
        } else {
            return new Promise((resolve) => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res) => {
                        const usbSupport = true;
                        if (usbSupport) {
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
