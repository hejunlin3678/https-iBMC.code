import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';
import { UnitsService } from '../../units.service';
import { BasicComputationArray } from '../../models/basic-computation/basic-computation-array';
import { BasicComputation } from '../../models/basic-computation/basic-computation';
import { ExtendInfo } from '../../models/basic-computation/extendInfo/entend-info';

@Injectable({
    providedIn: 'root'
})
export class BasicComputationService {

    constructor(
        private unitsService: UnitsService
    ) { }

    factory(activeCard: CardUrlArray): Observable<BasicComputationArray> {
        const basicComputationArray = new BasicComputationArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.unitsService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas?.forEach((bodyData) => {
                        const productName = isOtherEmptyProperty(bodyData.Name) || ' ';
                        const position = isOtherEmptyProperty(bodyData.Position) || ' ';
                        const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                        const slotId = isOtherEmptyProperty(bodyData.Id) || 0;
                        const type = isOtherEmptyProperty(bodyData.Description) || ' ';
                        const componentUID =
                                bodyData.ComponentUniqueID === undefined ? 'undefined' : isOtherEmptyProperty(bodyData.ComponentUniqueID);
                        const basicComputation = new BasicComputation(
                            productName,
                            position,
                            manufacturer,
                            slotId,
                            type,
                            componentUID
                        );
                        let showExtendInfo: boolean = false;
                        // 展示的扩展信息字段
                        if (bodyData?.ExtendInfo) {
                            showExtendInfo = true;
                            const serialNumber = isOtherEmptyProperty(bodyData.ExtendInfo.SerialNumber);
                            const pCBVersion = isOtherEmptyProperty(bodyData.ExtendInfo.PCBVersion) || ' ';
                            const cPLDVersion = isOtherEmptyProperty(bodyData.ExtendInfo.CPLDVersion) || ' ';
                            const selfDescriptionVersion = isOtherEmptyProperty(bodyData.ExtendInfo.HWSRVersion) || ' ';
                            const bios1Version = isOtherEmptyProperty(bodyData.ExtendInfo.Bios1Version) || ' ';
                            const partNumber = isOtherEmptyProperty(bodyData.ExtendInfo.PartNumber) || ' ';
                            const bios2Version = isOtherEmptyProperty(bodyData.ExtendInfo.Bios2Version) || ' ';
                            const boardId = isOtherEmptyProperty(bodyData.ExtendInfo.BoardId) || ' ';
                            const mcuVersion = isOtherEmptyProperty(bodyData.ExtendInfo.McuVersion) || ' ';
                            const extendInfo = new ExtendInfo (
                                serialNumber,
                                pCBVersion,
                                cPLDVersion,
                                selfDescriptionVersion,
                                bios1Version,
                                partNumber,
                                bios2Version,
                                boardId,
                                mcuVersion
                            );
                            basicComputation.setExtendInfo = extendInfo;
                        }
                        basicComputationArray.setDropdownIconShow = showExtendInfo;
                        basicComputationArray.addBasicComputation(basicComputation);
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => basicComputationArray)
        );
    }
}
