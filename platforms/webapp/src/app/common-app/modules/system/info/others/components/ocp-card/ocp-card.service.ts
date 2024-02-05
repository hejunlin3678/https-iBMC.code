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
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT, PRODUCTTYPE } from 'src/app/common-app/service/product.type';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';

@Injectable({
    providedIn: 'root'
})
export class OCPCardService {

    constructor(
        private othersService: OthersService,
        private globalData: GlobalDataService
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
                            const associatedResource = (this.globalData.productType === PRODUCTTYPE.PANGEA ||
                                                    CommonData.productType === PRODUCT.PANGEA) ?
                                                    this.getPangeaResource(bodyData.OCPFunctionDate.AssociatedResource) :
                                                    (isOtherEmptyProperty(bodyData.OCPFunctionDate.AssociatedResource) || ' ');
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
    // 盘古的资源归属比返回的字符串少1，如返回CPU1页面需显示CPU0
    public getPangeaResource(data: string) {
        let resourceData = '';
        const cpuStr = data.substring(0, 3);
        const indexStr = Number(data.substring(3));
        resourceData = cpuStr + (indexStr - 1);
        return resourceData;
    }
}

