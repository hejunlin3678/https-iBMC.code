import { Injectable } from '@angular/core';
import { SDCardArray, SDCard } from '../../models';
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
export class SDCardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<SDCardArray> {
        const sDCardArray = new SDCardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const id = isOtherEmptyProperty(bodyData.Id) ? bodyData.Id.substring(15) : 0;
                        let capacityBytes = null;
                        if (bodyData.CapacityBytes === 0 || bodyData.CapacityBytes) {
                            capacityBytes = bodyData.CapacityBytes;
                            capacityBytes = capacityBytes / 1024 / 1024;
                            capacityBytes = capacityBytes.toFixed(0);
                        } else {
                            capacityBytes = null;
                        }
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
                        sDCardArray.addSDCard(new SDCard(id, manufacturer, serialNumber, capacityBytes));
                    });

                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => sDCardArray)
        );
    }

}
