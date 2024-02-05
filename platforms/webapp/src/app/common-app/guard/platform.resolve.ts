import { Injectable } from '@angular/core';
import { ActivatedRouteSnapshot, RouterStateSnapshot } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';
import { UserInfoService, CommonService } from 'src/app/common-app/service';


@Injectable({
    providedIn: 'root'
})


export class PlatFormResolve {
    constructor(
        private userInfo: UserInfoService,
        private commonService: CommonService
    ) {
    }

    resolve(route: ActivatedRouteSnapshot, state: RouterStateSnapshot): Observable<string> | Promise<string> | string {
        let platform = this.userInfo.platform;
        if (platform !== '') {
            return platform;
        } else {
            return new Observable(observe => {
                this.commonService.getGenericInfo().subscribe({
                    next: (res) => {
                        platform = res.body.ArmSupported ? 'Arm' : 'x86';
                        this.userInfo.platform = platform;
                        this.commonService.saveUserToStorage();
                        observe.next(platform);
                        observe.complete();
                    },
                    error: () => {
                        observe.next(platform);
                        observe.complete();
                    }
                });
            });
        }
    }
}

