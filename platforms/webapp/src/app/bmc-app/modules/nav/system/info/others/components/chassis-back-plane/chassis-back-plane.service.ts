import { Injectable } from '@angular/core';
import { ChassisBackplaneArray, ChassisBackplane } from '../../models';
import { map } from 'rxjs/internal/operators/map';
import { of } from 'rxjs/internal/observable/of';
import { Observable } from 'rxjs/internal/Observable';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { OthersService } from '../../others.service';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';


@Injectable({
    providedIn: 'root'
})
export class ChassisBackPlaneService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<ChassisBackplaneArray> {
        const chassisBackplaneArray = new ChassisBackplaneArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = isOtherEmptyProperty(bodyData.BoardName) || ' ';
                        const location = isOtherEmptyProperty(bodyData.Location) || ' ';
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                        const boardId = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                        const componentUniqueID =
                            bodyData.ComponentUniqueID === undefined ? 'undefined' : isOtherEmptyProperty(bodyData.ComponentUniqueID);
                        chassisBackplaneArray.addChassisBackplane(
                            new ChassisBackplane(boardName, location, manufacturer, pCBVersion, boardId, componentUniqueID)
                        );
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => chassisBackplaneArray)
        );
    }

}
