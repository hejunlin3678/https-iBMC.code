import { Injectable } from '@angular/core';
import { ExpandBoardArray, ExpandBoard } from '../../models';
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
export class ExpandBoardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<ExpandBoardArray> {
        const expandBoardArray = new ExpandBoardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = isOtherEmptyProperty(bodyData.BoardName) || ' ';
                        const boardID = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                        const componentUniqueID =
                            bodyData.ComponentUniqueID === undefined ? 'undefined' : isOtherEmptyProperty(bodyData.ComponentUniqueID);
                        const cPLDVersion = isOtherEmptyProperty(bodyData.CPLDVersion) || ' ';
                        const cardNo = isOtherEmptyProperty(bodyData.CardNo) || 0;
                        const description = isOtherEmptyProperty(bodyData.Description) || ' ';
                        const location = isOtherEmptyProperty(bodyData.Location) || ' ';
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                        const partNumber = isOtherEmptyProperty(bodyData.PartNumber) || ' ';
                        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
                        expandBoardArray.addExpandBoard(
                            new ExpandBoard(
                                boardName,
                                boardID,
                                componentUniqueID,
                                cPLDVersion,
                                cardNo,
                                description,
                                location,
                                manufacturer,
                                pCBVersion,
                                partNumber,
                                serialNumber
                            ));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => expandBoardArray)
        );
    }

}
