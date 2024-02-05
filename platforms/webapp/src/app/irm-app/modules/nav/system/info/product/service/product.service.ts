import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { IInfo, IUpdateBody } from '../product.datatype';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { CommonData } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class ProductService {

    constructor(
        private http: HttpService,
    ) { }

    public getData() {
        return this.getProductInfo();
    }
    private factoryProductInfo(productData): IInfo {
        const noDataStr = CommonData.isEmpty;
        const {
            BasicInfo: {
                Model: productModel,
                AssetTag: productTag,
                SerialNumber: serialNumberBase,
                PartNumber: partNumberBase,
                Manufacturer: vendor,
                FirmwareVersion: firmwareVersion,
                BoardID: veneerID,
            },
            RackInfo: {
                Name: cabinetName,
                Rack: cabinetRack,
                Row: cabinetRow,
                RatedPowerWatts: ratedPower,
                LoadCapacityKg: loadCapacityKg,
                TotalUCount: totalUCount,
                UcountUsed: cabinetUse,
            },
        } = productData;
        let countUsed;
        if (cabinetUse && cabinetUse > 0) {
            countUsed = Math.ceil(cabinetUse / totalUCount * 100);
        } else {
            countUsed = 0;
        }
        const resultData: IInfo = {
            Name: cabinetName || noDataStr,
            Rack: cabinetRack || noDataStr,
            Row: cabinetRow || noDataStr,
            cabinetPosition: cabinetName ? cabinetName + '(' + (cabinetRow || '') + ',' + (cabinetRack || '') + ')' : noDataStr,
            RatedPowerWatts: ratedPower || noDataStr,
            LoadCapacityKg: loadCapacityKg || loadCapacityKg === 0 ? loadCapacityKg : noDataStr,
            TotalUCount: totalUCount || noDataStr,
            UcountUsed: countUsed || countUsed === 0 ? countUsed : noDataStr,
            productModel: productModel || noDataStr,
            productTag: productTag || '',
            serialNumberBase: serialNumberBase || noDataStr,
            partNumberBase: partNumberBase || noDataStr,
            vendor: vendor || noDataStr,
            firmwareVersion: firmwareVersion || noDataStr,
            veneerID: veneerID || noDataStr,
        };
        return resultData;
    }

    public updateAssetTag(value: string) {
        return this.updateProductInfo({
            AssetTag: value
        });
    }
    // 机柜信息保存
    public cabinetSave(param: any) {
        return this.updateProductInfo({
            RackInfo: param
        });
    }

    private getProductInfo(): Observable<IInfo> {
        const url = '/UI/Rest/System/ProductInfo';
        return this.http.get(url).pipe(
            pluck('body'),
            map((data) => this.factoryProductInfo(data))
        );
    }

    private updateProductInfo(value: IUpdateBody) {
        const url = '/UI/Rest/System/ProductInfo';
        return this.http.patch(url, value);
    }

}
