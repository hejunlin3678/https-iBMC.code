import { Injectable } from '@angular/core';
import { SecurityArray, Security } from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { OthersService } from '../../others.service';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';

@Injectable({
    providedIn: 'root'
})
export class SecurityService {

    constructor(
        private othersService: OthersService,
        private i18n: TranslateService
    ) { }

    factory(activeCard: CardUrlArray): Observable<SecurityArray> {
        const securityArray = new SecurityArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                    const protocolType = isOtherEmptyProperty(bodyData.ProtocolType) || ' ';
                    const protocolVersion = isOtherEmptyProperty(bodyData.ProtocolVersion) || ' ';
                    const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                    const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion) || ' ';
                    let selfTestResult = '';
                    if (bodyData.SelfTestResult == null) {
                        selfTestResult = '--';
                    } else {
                        selfTestResult = bodyData.SelfTestResult === 'OK' ? 'IBMC_AUTOMATIC_SUCCESS' : 'IBMC_AUTOMATIC_FAILED';
                    }
                    securityArray.addSecurity(new Security(protocolType, protocolVersion, manufacturer, firmwareVersion, selfTestResult));
                });
            })));
        });
        }
        return forkJoin(post$).pipe(
            map(() => securityArray)
        );
    }
}
