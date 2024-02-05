import { Injectable } from '@angular/core';
import { SDStorageArray, SDStorage } from '../../models';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { OthersService } from '../../others.service';
import { CardUrlArray } from '../../models/cardUrlArray';

@Injectable({
    providedIn: 'root'
})
export class SDStorageService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<SDStorageArray> {
        const sDStorageArray = new SDStorageArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion) || ' ';
                        sDStorageArray.addSDStorage(new SDStorage(manufacturer, firmwareVersion));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => sDStorageArray)
        );
    }

}
