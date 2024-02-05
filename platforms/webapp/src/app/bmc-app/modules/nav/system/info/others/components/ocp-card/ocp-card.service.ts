import { Injectable } from '@angular/core';
import { OCPCardArray, OCPCard } from '../../models';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { OthersService } from '../../others.service';
import { OCPExtendAttr } from '../../models/ocp-card/ocp-extend-attr';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';

@Injectable({
    providedIn: 'root'
})
export class OCPCardService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<OCPCardArray> {
        const oCPCardArray = new OCPCardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                            const id = bodyData.Id.substring(8);
                            const description = isOtherEmptyProperty(bodyData.Description) || ' ';
                            const position = isOtherEmptyProperty(bodyData.Position) || ' ';
                            const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                            const vendorId = isOtherEmptyProperty(bodyData.OCPFunctionDate.VendorId) || ' ';
                            const deviceId = isOtherEmptyProperty(bodyData.OCPFunctionDate.DeviceId) || ' ';
                            const subsystemVendorId = isOtherEmptyProperty(bodyData.OCPFunctionDate.SubsystemVendorId) || ' ';
                            const subsystemId = isOtherEmptyProperty(bodyData.OCPFunctionDate.SubsystemId) || ' ';
                            const associatedResource = isOtherEmptyProperty(bodyData.OCPFunctionDate.AssociatedResource) || ' ';
                            const extendAttr = new OCPExtendAttr(
                                bodyData.ProductName,
                                bodyData.OCPFunctionDate.SubsystemVendorId,
                                bodyData.OCPFunctionDate.SubsystemId,
                            );
                            oCPCardArray.addOCPCard(
                                new OCPCard(
                                    description,
                                    position,
                                    manufacturer,
                                    id,
                                    vendorId,
                                    deviceId,
                                    subsystemVendorId,
                                    subsystemId,
                                    associatedResource,
                                    extendAttr
                                )
                            );
                        });
                        return bodyDatas;
                        }),
                    ));
            });
        }
        return forkJoin(post$).pipe(
            map(() => oCPCardArray)
        );
    }
}

