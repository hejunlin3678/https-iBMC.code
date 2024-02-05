import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { AssetModel } from '../model/asset.model';
import { map } from 'rxjs/internal/operators/map';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { isEmptyProperty } from 'src/app/common-app/utils/common';
import { CommonData } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class AssetService {
    constructor(private http: HttpService) {

    }

    // 资产列表数据
    public getAllAssetData() {
        return this.getAssetData();
    }
    private getAssetData(): Observable<AssetModel[]> {
        return this.http.get('/UI/Rest/Overview').pipe(
            pluck('body'),
            map(data => this.createAssetTable(data.Summary.Comunite))
        );
    }
    // 资产列表数据处理
    private createAssetTable(data): AssetModel[] {
        const resultArr: AssetModel[] = [];
        data.forEach(item => {
            // 在位的情况
            if (item.state !== 'Absent') {
                const extendArr = [];
                if (item.ExtendField) {
                    Object.keys(item.ExtendField).forEach((keyExtend) => {
                        const valueExtend = item.ExtendField[keyExtend];
                        extendArr.push({
                            key: keyExtend,
                            value: valueExtend || CommonData.isEmpty,
                        });
                    });
                }
                // 以下时间处理代码主要是为了解决IE下无法使用 - 的方式初始化时间而做的hack,处理逻辑，去除首尾空格，中间空格以T分隔
                const time = item.DiscoveredTime;
                let shelfTime = '';
                if (time) {
                    const arr = time.split(' ');
                    shelfTime = arr[0] + ' ' + arr[1];
                }
                const noDataStr = CommonData.isEmpty;
                const assetData = new AssetModel({
                    state: isEmptyProperty(item.state) || noDataStr,
                    uNum: isEmptyProperty(item.TopUSlot) || noDataStr,
                    tagUid: isEmptyProperty(item.RFIDTagUID) || noDataStr,
                    shelfTime: isEmptyProperty(shelfTime) || noDataStr,
                    pattern: isEmptyProperty(item.Model) || noDataStr,
                    deviceType: isEmptyProperty(item.DeviceType) || noDataStr,
                    serialNumber: isEmptyProperty(item.SerialNumber) || noDataStr,
                    partNumber: isEmptyProperty(item.PartNumber) || noDataStr,
                    uHeight: item.UHeight ? item.UHeight : 1,
                    factory: isEmptyProperty(item.Manufacturer) || noDataStr,
                    lifeCircle: isEmptyProperty(item.LifeCycleYear) || noDataStr,
                    startTime: isEmptyProperty(item.CheckInTime) || noDataStr,
                    weight: isEmptyProperty(item.WeightKg) || noDataStr,
                    ratedPower: isEmptyProperty(item.RatedPowerWatts) || noDataStr,
                    owner: isEmptyProperty(item.AssetOwner) || noDataStr,
                    extendField: extendArr,
                    // 是否只读  ReadWrite – 用户可读写	ReadOnly – 用户只读
                    rWCapability: item.RWCapability,
                    availableRackSpaceU: item.AvailableRackSpaceU,
                });
                resultArr.push(assetData);
            }
        });
        return resultArr;
    }

    // 保存编辑数据
    public saveAssetTag(uNum, param) {
        const urlNew = '/UI/Rest/System/AssetInfo/U' + uNum;
        return this.http.post(urlNew, param).pipe(
            map((res) => {
                return res;
            })
        );
    }
}
