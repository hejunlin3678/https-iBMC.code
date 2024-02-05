import { Injectable } from '@angular/core';
import { CommonData, GlobalData } from 'src/app/common-app/models';
import { ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, CanActivate, Router } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';
import { BMCAppService } from '../bmc-app.service';
import { routePrivilegesConfig } from '../routePrivileges.config';
import { NavMenuService } from '../services/navMenu.service';
import { PRODUCT, GlobalDataService, HttpService, CommonService, UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class BMCGuard implements CanActivate {
    constructor(
        private appservice: BMCAppService,
        private http: HttpService,
        private router: Router,
        private userInfo: UserInfoService,
        private commonSever: CommonService,
        private globalData: GlobalDataService,
        private navService: NavMenuService
    ) { }

    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return new Observable(observe => {
            this.appservice.getStyle().subscribe((res) => {
                GlobalData.getInstance().setWebStyle = res;
                CommonData.headerLogSrc = `assets/bmc-assets/image/home/default/header_logo.png`;
                CommonData.productType = PRODUCT.IBMC;
                CommonData.routeConfig = routePrivilegesConfig;
                CommonData.mainMenu = this.navService.mainMenu;
                observe.next(true);
                observe.complete();
            });
        });
    }
}
