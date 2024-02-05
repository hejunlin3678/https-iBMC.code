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
export class VideoGuard implements CanActivate {
    constructor(
        private globalData: GlobalDataService,
        private userInfo: UserInfoService,
        private commonService: CommonService
    ) {}
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        if (this.userInfo.videoScreenshotSupport !== null && this.userInfo.videoScreenshotSupport !== undefined) {
            return this.userInfo.videoScreenshotSupport;
        } else {
            return new Promise((resolve) => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res) => {
                        const support = res['body'].VideoScreenshotSupported;
                        if (support) {
                            resolve(true);
                        } else {
                            resolve(false);
                            this.globalData.routeChagneError.next(false);
                        }
                    },
                    error: (error) => {
                        resolve(false);
                        this.globalData.routeChagneError.next(false);
                    }
                });
            });
        }
    }
}
