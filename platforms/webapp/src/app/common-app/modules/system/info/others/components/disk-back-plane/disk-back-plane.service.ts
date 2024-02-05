import { Injectable } from '@angular/core';
import { DiskBackPlane, DiskBackPlaneArray } from '../../models';
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
export class DiskBackPlaneService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<DiskBackPlaneArray> {
        const diskBackPlaneArray = new DiskBackPlaneArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                        const boardName = isOtherEmptyProperty(bodyData.BoardName);
                        const location = isOtherEmptyProperty(bodyData.Location);
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer);
                        const cardNo = isOtherEmptyProperty(bodyData.CardNo);
                        const description = isOtherEmptyProperty(bodyData.Description);
                        const pCBVersion = isOtherEmptyProperty(bodyData.PCBVersion);
                        const cPLDVersion = isOtherEmptyProperty(bodyData.CPLDVersion);
                        const boardId = isOtherEmptyProperty(bodyData.BoardID);
                        const partNumber = isOtherEmptyProperty(bodyData.PartNumber);
                        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber);
                        const diskBackPlane = new DiskBackPlane(
                            boardName,
                            location,
                            manufacturer,
                            cardNo,
                            description,
                            pCBVersion,
                            cPLDVersion,
                            boardId,
                            partNumber,
                            serialNumber
                        );
                        diskBackPlaneArray.addDiskBackPlanes(diskBackPlane);
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => diskBackPlaneArray)
        );
    }
}
