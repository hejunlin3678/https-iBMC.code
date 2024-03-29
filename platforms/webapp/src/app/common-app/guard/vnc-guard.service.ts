import { Injectable } from '@angular/core';
import {
    CanActivate,
    ActivatedRouteSnapshot,
    RouterStateSnapshot,
    UrlTree
} from '@angular/router';
import { Observable } from 'rxjs';
import { CommonService, GlobalDataService, UserInfoService } from '../service';

@Injectable({
    providedIn: 'root'
})
export class VNCGuard implements CanActivate {
    constructor(
        private globalData: GlobalDataService,
        private userInfo: UserInfoService,
        private commonService: CommonService

    ) {}
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        if (this.userInfo.vncSupport !== null && this.userInfo.vncSupport !== undefined) {
            return this.userInfo.vncSupport;
        } else {
            return new Promise((resolve) => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res) => {
                        const support = res['body'].VNCSupported;
                        if (support) {
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
