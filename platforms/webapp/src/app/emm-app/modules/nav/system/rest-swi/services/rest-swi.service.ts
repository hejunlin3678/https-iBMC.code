import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { SwapInfo } from '../model/rest-swi.model';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin, Observable } from 'rxjs';
import { pluck } from 'rxjs/internal/operators/pluck';
import { CommonData } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class RestSwiService {
    constructor(
        private http: HttpService,
    ) { }
    // 列表数据
    getSwapInfo(): Observable<SwapInfo[]> {
        const getSwap$ = this.http.get('/UI/Rest/Chassis/Enclosure/SwitchProfile').pipe(pluck('body'));
        return forkJoin([getSwap$]).pipe(
            map(
                ([swapInfo]) => {
                    return this.factorySwapInfo(swapInfo);
                }
            )
        );
    }
    private factorySwapInfo(data): SwapInfo[] {
        const resultArr: SwapInfo[] = [];
        if (data && data.length > 0) {
            const _temp = {};
            data.forEach(item => {
                const attr = item.Slot + item.PlaneType;
                if (!_temp[attr]) {
                    _temp[attr] = {};
                    _temp[attr]['slotNo'] = item.Slot || CommonData.isEmpty;
                    _temp[attr]['slotSwi'] = 'Swi' + item.Slot || CommonData.isEmpty;
                    _temp[attr]['boardType'] = item.BoardType || CommonData.isEmpty;
                    _temp[attr]['planeType'] = item.PlaneType || CommonData.isEmpty;
                    _temp[attr]['flieName'] = item.FlieName || CommonData.isEmpty;
                    _temp[attr]['backCount'] = 0;
                    _temp[attr]['detailList'] = [];
                    _temp[attr]['radioId'] = '';
                }
                if (item.BackupTime !== 'current') {
                    _temp[attr]['backCount']++;
                    _temp[attr]['detailList'].push({
                        id: _temp[attr]['backCount'],
                        flieName: item.FlieName || CommonData.isEmpty,
                        backupTime: item.BackupTime || CommonData.isEmpty
                    });
                }
            });
            Object.keys(_temp).forEach((key) => {
                resultArr.push(_temp[key]);
            });
        }
        return resultArr;
    }

    // 获取详情
    public getDetailInfo(param) {
        return this.http.post(`/UI/Rest/Chassis/Enclosure/ParseSwitchProfile`, param);
    }
    // 交换配置
    recoverySetting(param) {
        return this.http.post(`/UI/Rest/Chassis/Enclosure/RestoreSwitchProfile`, param);
    }
}
