import { Injectable } from '@angular/core';
import {
    CanActivate,
    ActivatedRouteSnapshot,
    RouterStateSnapshot,
    UrlTree
} from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';

import { GlobalDataService } from 'src/app/common-app/service/global-data.service';

@Injectable({
    providedIn: 'root'
})
export class WebGuard implements CanActivate {
    constructor(
        private globalData: GlobalDataService
    ) {

    }
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        this.globalData.closePresetCert.next(true);
        return true;
    }
}
