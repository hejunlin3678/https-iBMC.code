import { Injectable } from '@angular/core';
import {
    CanActivate,
    ActivatedRouteSnapshot,
    RouterStateSnapshot,
    UrlTree
} from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';
import { ProductService } from 'src/app/bmc-app/modules/nav/system/info/product/service';
import { GlobalDataService } from '../service';
import { PRODUCTTYPE } from 'src/app/common-app/service/product.type';


@Injectable({
    providedIn: 'root'
})
export class InfoGuard implements CanActivate {
    constructor(
        private productService: ProductService,
        private globalData: GlobalDataService,
    ) {}
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
            return new Observable((observer) => {
                this.productService.getData().subscribe((data) => {
                    let isProductInfo = null;
                    if (data.boardId === '0x0009' && data.productId === 7) {
                        isProductInfo = PRODUCTTYPE.WUHAN;
                    } else if (data.boardId === '0x00ff') {
                        isProductInfo = PRODUCTTYPE.TIANCHI;
                    } else if ((data.boardId === '0x0001' || data.boardId === '0x0003') && data.productId === 2) {
                        isProductInfo = PRODUCTTYPE.ATLAS;
                    } else {
                        isProductInfo = PRODUCTTYPE.COMMON;
                    }
                    const pangea: string[] = new Array('0x0030', '0x0017', '0x0090', '0x0093', '0x001b', '0x00f3' );
                    for ( const contract of pangea ) {
                        if ( data.boardId === contract && data.productId === 13 ) {
                            isProductInfo = PRODUCTTYPE.PANGEA;
                        }
                    }
                    this.globalData.productType = isProductInfo;
                    observer.next(true);
                    observer.complete();
                    return ;
                });
            });
        }
}
