import { Injectable } from '@angular/core';
import { GPUBoardArray, GPUBoard } from '../../models';
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
export class GPUBoardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<GPUBoardArray> {
        const gPUBoardArray = new GPUBoardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = isOtherEmptyProperty(bodyData.BoardName) || ' ';
                        const slot = isOtherEmptyProperty(bodyData.CardNo) || ' ';
                        const boardId = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                        const componentUniqueID =
                            bodyData.ComponentUniqueID === undefined ? 'undefined' : isOtherEmptyProperty(bodyData.ComponentUniqueID);
                        const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                        const cPLDVersion = bodyData.CPLDVersion == null ? '--' : '(' + bodyData.PositionId + ')' + bodyData.CPLDVersion;
                        gPUBoardArray.addGPUBoard(new GPUBoard(boardName, slot, boardId, componentUniqueID, pCBVersion, cPLDVersion));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => gPUBoardArray)
        );
    }

}
