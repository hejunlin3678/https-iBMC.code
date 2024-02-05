import { Injectable } from '@angular/core';

import { CommonData, GlobalData, WebStyle } from 'src/app/common-app/models';
import { ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, CanActivate } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';
import { routePrivilegesConfig } from '../routePrivileges.config';
import { NavMenuService } from '../services/navMenu.service';
import { PRODUCT } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class EMMGuard implements CanActivate {
    constructor(
        private navService: NavMenuService
    ) { }

    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return new Observable(observe => {
            GlobalData.getInstance().setWebStyle = WebStyle.Default;
            CommonData.headerLogSrc = `assets/emm-assets/image/home/default/header_logo.png`;
            CommonData.productType = PRODUCT.EMM;
            CommonData.routeConfig = routePrivilegesConfig;
            CommonData.mainMenu = this.navService.mainMenu;
            observe.next(true);
            observe.complete();
        });
    }
}
