import { Injectable } from '@angular/core';
import { PeripheralFirmwareArray, PeripheralFirmware } from '../../models';
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
export class PeripheralService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<PeripheralFirmwareArray> {
        const peripheralFirmwareArray = new PeripheralFirmwareArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const name = isOtherEmptyProperty(bodyData.Name) || ' ';
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const version = isOtherEmptyProperty(bodyData.Version) || ' ';
                        peripheralFirmwareArray.addPeripheralFirmware(new PeripheralFirmware(name, manufacturer, version));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => peripheralFirmwareArray)
        );
    }

}
