import { Injectable } from '@angular/core';
import { IOBoardArray, IOBoard } from '../../models';
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
export class IOBoardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<IOBoardArray> {
        const iOBoardArray = new IOBoardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = bodyData?.ProductName || isOtherEmptyProperty(bodyData.BoardName) || ' ';
                        const location = isOtherEmptyProperty(bodyData.Location) || ' ';
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const cardNo = isOtherEmptyProperty(bodyData.CardNo) || 0;
                        const description = isOtherEmptyProperty(bodyData.DeviceType) || ' ';
                        const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                        const boardId = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                        const partNumber = isOtherEmptyProperty(bodyData.PartNumber) || ' ';
                        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
                        iOBoardArray.addIOBoard(
                            new IOBoard(
                                boardName,
                                location,
                                manufacturer,
                                cardNo,
                                description,
                                pCBVersion,
                                boardId,
                                partNumber,
                                serialNumber
                            ));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => iOBoardArray)
        );
    }
}
