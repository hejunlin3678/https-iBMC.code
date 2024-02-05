import { Injectable } from '@angular/core';
import { CanActivate, ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree } from '@angular/router';
import { Observable } from 'rxjs';
import { CommonService, UserInfoService } from '../service';

@Injectable({
    providedIn: 'root'
})
export class GenericInfoGuard implements CanActivate {
    constructor(private userInfo: UserInfoService, private commonService: CommonService) {}
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot
    ): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        if (this.userInfo.ibmaSupport !== null && this.userInfo.ibmaSupport !== undefined) {
            return true;
        } else {
            return new Promise((resolve) => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res) => {
                        this.userInfo.ibmaSupport = res['body'].SmsSupported;
                        resolve(true);
                    },
                    error: (error) => {
                        resolve(true);
                    }
                });
            });
        }
    }
}
