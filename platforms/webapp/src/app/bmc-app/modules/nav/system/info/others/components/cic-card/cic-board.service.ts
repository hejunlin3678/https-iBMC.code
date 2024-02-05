import { Injectable } from '@angular/core';
import { CicBoardArray, CicBoard } from '../../models';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { OthersService } from '../../others.service';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';
import { CICExtendAttr } from '../../models/cic-card/cic-extend-attr';

@Injectable({
    providedIn: 'root'
})
export class CicBoardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<CicBoardArray> {
        const cicBoardArray = new CicBoardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = isOtherEmptyProperty(bodyData.BoardName);
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer);
                        const pcbVer = isOtherEmptyProperty(bodyData.PcbVer);
                        const boardId = isOtherEmptyProperty(bodyData.BoardId) || 0;
                        const componentUniqueID =
                            bodyData.ComponentUniqueID === undefined ? 'undefined' : isOtherEmptyProperty(bodyData.ComponentUniqueID);
                        const description = isOtherEmptyProperty(bodyData.Description);
                        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber);
                        const partNum = isOtherEmptyProperty(bodyData.PartNum);
                        const extendAttr = new CICExtendAttr(
                            bodyData.M2Device1Presence,
                            bodyData.M2Device2Presence,
                        );
                        cicBoardArray.addExpandBoard(
                            new CicBoard(
                                boardName,
                                manufacturer,
                                pcbVer,
                                boardId,
                                componentUniqueID,
                                description,
                                serialNumber,
                                partNum,
                                extendAttr
                            ));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => cicBoardArray)
        );
    }

}
