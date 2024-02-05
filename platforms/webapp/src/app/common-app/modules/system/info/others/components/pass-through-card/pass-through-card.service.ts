import { Injectable } from '@angular/core';
import { PassThroughCardArray, PassThroughCard } from '../../models';
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
export class PassThroughCardService {

    constructor(
        private othersService: OthersService
    ) { }
    factory(activeCard: CardUrlArray): Observable<PassThroughCardArray> {
        const passThroughCardArray = new PassThroughCardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                    const productName = isOtherEmptyProperty(bodyData.ProductName) || ' ';
                    const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                    const description = isOtherEmptyProperty(bodyData.Description) || ' ';
                    const cardNo = isOtherEmptyProperty(bodyData.CardNo) || 0;
                    const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                    const boardId = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                    const partNumber = isOtherEmptyProperty(bodyData.PartNumber) || ' ';
                    const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
                    passThroughCardArray.addPassThroughCard(
                        new PassThroughCard(
                            productName,
                            manufacturer,
                            description,
                            cardNo,
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
            map(() => passThroughCardArray)
        );
    }

}
