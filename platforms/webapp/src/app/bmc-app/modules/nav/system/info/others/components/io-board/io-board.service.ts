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
                    const boardName = isOtherEmptyProperty(bodyData.BoardName) || ' ';
                    const location = isOtherEmptyProperty(bodyData.Location) || ' ';
                    const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                    const description = isOtherEmptyProperty(bodyData.Description) || ' ';
                    const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion);
                    const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion) || ' ';
                    const cPLDVersion = isOtherEmptyProperty(bodyData.CPLDVersion) || ' ';
                    const boardId = isOtherEmptyProperty(bodyData.BoardID) || ' ';
                    const componentUniqueID =
                            bodyData.ComponentUniqueID === undefined ? 'undefined' : isOtherEmptyProperty(bodyData.ComponentUniqueID);
                    const powerWatts = isOtherEmptyProperty(bodyData.PowerWatts) || ' ';
                    const partNumber = isOtherEmptyProperty(bodyData.PartNumber) || ' ';
                    const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
                    iOBoardArray.addIOBoard(
                        new IOBoard(
                            boardName,
                            location,
                            manufacturer,
                            description,
                            firmwareVersion,
                            pCBVersion,
                            cPLDVersion,
                            boardId,
                            componentUniqueID,
                            powerWatts,
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
