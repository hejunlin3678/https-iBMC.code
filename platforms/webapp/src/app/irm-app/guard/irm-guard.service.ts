import { Injectable } from '@angular/core';
import { CommonData, GlobalData, WebStyle } from 'src/app/common-app/models';
import { ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, CanActivate, Router } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';
import { NavMenuService } from '../service/navMenu.service';
import { routePrivilegesConfig } from '../routePrivileges.config';
import { PRODUCT, GlobalDataService, HttpService, CommonService, UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class IRMGuard implements CanActivate {
    constructor(
        private navService: NavMenuService,
        private http: HttpService,
        private router: Router,
        private userInfo: UserInfoService,
        private commonSever: CommonService,
        private globalData: GlobalDataService,
    ) { }

    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return new Observable(observe => {
            GlobalData.getInstance().setWebStyle = WebStyle.Default;
            CommonData.mainMenu = this.navService.mainMenu;
            CommonData.headerLogSrc = `assets/irm-assets/image/home/productLogo.png`;
            CommonData.routeConfig = routePrivilegesConfig;
            CommonData.productType = PRODUCT.IRM;
            observe.next(true);
            observe.complete();
        });
    }
}
