import { Injectable } from '@angular/core';

import { CommonData, GlobalData, WebStyle } from 'src/app/common-app/models';
import { ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, CanActivate } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';
import { Pangen } from '../models/pangen.model';
import { OceanType } from '../models/common.datatype';
import { ArcticNavMenuService, AtlanticNavMenuService, AtlanticSMMNavMenuService, PacificNavMenuService } from '../services';
import { arcticRoutePrivilegesConfig, pacificRoutePrivilegesConfig, atlanticSMMRoutePrivilegesConfig, atlanticRoutePrivilegesConfig } from '../routes';
import { PRODUCT } from 'src/app/common-app/models/product.type';

@Injectable({
    providedIn: 'root'
})
export class PangeaGuard implements CanActivate {
    constructor(
        private arcticNavService: ArcticNavMenuService,
        private atlanticNavService: AtlanticNavMenuService,
        private atlanticSMMNavService: AtlanticSMMNavMenuService,
        private pacificNavService: PacificNavMenuService,
    ) {}

    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return new Observable(observe => {
            GlobalData.getInstance().setWebStyle = WebStyle.Default;
            CommonData.headerLogSrc = `/assets/pangea-assets/image/home/default/header_logo.png`;
            CommonData.productType = PRODUCT.PANGEA;
            // 构建文件pack_TinyUI用作替换四大洋的代码
            Pangen.ocean = 'PangeaV6_Arctic';
            switch (Pangen.ocean) {
                case OceanType.Arctic:
                    CommonData.mainMenu = this.arcticNavService.mainMenu;
                    CommonData.routeConfig = arcticRoutePrivilegesConfig;
                    break;
                case OceanType.Atlantic:
                    CommonData.mainMenu = this.atlanticNavService.mainMenu;
                    CommonData.routeConfig = atlanticRoutePrivilegesConfig;
                    break;
                case OceanType.AtlanticSMM:
                    CommonData.mainMenu = this.atlanticSMMNavService.mainMenu;
                    CommonData.routeConfig = atlanticSMMRoutePrivilegesConfig;
                    break;
                case OceanType.Pacific:
                    CommonData.mainMenu = this.pacificNavService.mainMenu;
                    CommonData.routeConfig = pacificRoutePrivilegesConfig;
                    break;
                default:
                    CommonData.mainMenu = this.arcticNavService.mainMenu;
                    CommonData.routeConfig = arcticRoutePrivilegesConfig;
                    break;
            }
            observe.next(true);
            observe.complete();
        });
    }
}
