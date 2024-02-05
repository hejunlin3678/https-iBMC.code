import { Injectable } from '@angular/core';
import { CanActivateChild, ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, Router } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';

import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { CommonService } from 'src/app/common-app/service/common.service';
import { GlobalDataService } from '../service';

@Injectable({
    providedIn: 'root'
})
export class KvmLoginGuard implements CanActivateChild {
    constructor(
        private userInfo: UserInfoService,
        private commonService: CommonService,
        private router: Router,
        private global: GlobalDataService
    ) {

    }
    canActivateChild(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return new Promise((resolve, reject) => {
            this.global.webType.then(() => {
                if (this.userInfo.resourceId) {
                    this.commonService.keepAlive().subscribe({
                        next: (res) => {
                            if (sessionStorage.getItem('kvmConnect')) {
                                localStorage.setItem('H5ClientModel', 'Private');
                                this.router.navigateByUrl('/kvm_h5');
                                sessionStorage.removeItem('kvmConnect');
                                sessionStorage.removeItem('kvmSearch');
                            } else {
                                this.router.navigate(['/navigate']);
                            }
                            resolve(false);
                        },
                        error: () => {
                            if (sessionStorage.getItem('kvmHtml5Info') || localStorage.getItem('kvmHtml5Info')) {
                                this.router.navigateByUrl('/kvm_h5');
                                resolve(false);
                            }
                            resolve(true);
                        }
                    });
                } else {
                    if (sessionStorage.getItem('kvmHtml5Info') || localStorage.getItem('kvmHtml5Info')) {
                        this.router.navigateByUrl('/kvm_h5');
                        resolve(false);
                    } else {
                        resolve(true);
                    }
                }
            });
        });
    }
}
