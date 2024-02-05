import { Injectable } from '@angular/core';
import { RaidCardArray, RaidCard } from '../../models';
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
export class RaidCardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<RaidCardArray> {
        const raidCardArray = new RaidCardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = isOtherEmptyProperty(bodyData.ProductName) || ' ';
                        const location = isOtherEmptyProperty(bodyData.Location) || ' ';
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const cardNo = isOtherEmptyProperty(bodyData.CardNo) || 0;
                        const description = isOtherEmptyProperty(bodyData.Description) || ' ';
                        const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                        const cPLDVersion = isOtherEmptyProperty(bodyData.CPLDVersion) || ' ';
                        const partNumber = isOtherEmptyProperty(bodyData.PartNumber) || ' ';
                        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
                        const boardId = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                        const associatedResource = isOtherEmptyProperty(bodyData.AssociatedResource) || ' ';
                        raidCardArray.addRaidCard(
                            new RaidCard(
                                boardName,
                                location,
                                manufacturer,
                                cardNo,
                                description,
                                pCBVersion,
                                cPLDVersion,
                                partNumber,
                                serialNumber,
                                boardId,
                                associatedResource
                            )
                        );
                    });

                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => raidCardArray)
        );
    }

}
