import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { FansInfo } from '../model/fans.model';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin, Observable } from 'rxjs';
import { Fans } from '../model/fans.interface';
import { DetailInfo } from '../model/detailInfo.model';
import { DetailInfoProduct } from '../model/detailInfoProduct.model';
import { pluck } from 'rxjs/internal/operators/pluck';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class FansService {
    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

    factory(): Observable<FansInfo> {
        const getFansInfo$ = this.http.get('/UI/Rest/Chassis/Enclosure/Thermal').pipe(pluck('body'));
        return forkJoin([getFansInfo$]).pipe(
            map(
                ([fansInfo]) => {
                    return this.factoryFansInfo(fansInfo);
                }
            )
        );
    }
    public factoryFansInfo(data: any): FansInfo {
        const smartCoolingMode = data.FanSmartCoolingMode;
        const lists = data.Fan;
        const smartCoolingModeText = this.intelligentInit(smartCoolingMode);
        return new FansInfo(
            smartCoolingMode,
            smartCoolingModeText,
            lists.length,
            this.transformTableData(lists)
        );
    }
    private intelligentInit(fanSmartCoolingMode: string) {
        let fanSmartCoolingModeText = '';
        if (fanSmartCoolingMode === 'EnergySaving') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE1');
        } else if (fanSmartCoolingMode === 'LowNoise') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE2');
        } else if (fanSmartCoolingMode === 'HighPerformance') {
            fanSmartCoolingModeText = this.translate.instant('FAN_MODEL_TITLE3');
        } else {
            fanSmartCoolingModeText = '';
        }
        return fanSmartCoolingModeText;
    }

    private transformTableData(lists: any[]): Fans[] {
        return lists.map((list) => {
           const status = list.State === 'Enabled' ? (list.Health === 'OK' ? 0 : 1) : 2;
            const detailInfo = new DetailInfo(
                list.Fru?.Board?.FruFileId || CommonData.isEmpty,
                list.Fru?.Board?.Manufacturer || CommonData.isEmpty,
                list.Fru?.Board?.ManufacturingDate || CommonData.isEmpty,
                list.Fru?.Board?.PartNumber || CommonData.isEmpty,
                list.Fru?.Board?.ProductName || CommonData.isEmpty,
                list.Fru?.Board?.SerialNumber || CommonData.isEmpty,
            );
            const detailInfoProduct = new DetailInfoProduct(
                list.Fru?.Product?.Manufacturer || CommonData.isEmpty,
                list.Fru?.Product?.PartNumber || CommonData.isEmpty,
                list.Fru?.Product?.ProductName || CommonData.isEmpty,
                list.Fru?.Product?.SerialNumber || CommonData.isEmpty,
                list.Fru?.Product?.Version || CommonData.isEmpty,
            );
            const fanDetailInfo = new Fans(
                list.Name || CommonData.isEmpty,
                list.Speed || CommonData.isEmpty,
                list.SoftwareVersion || CommonData.isEmpty,
                list.PcbVersion || CommonData.isEmpty,
                list.Health || CommonData.isEmpty,
                status,
                detailInfo,
                detailInfoProduct,
            );
            return fanDetailInfo;
        });
    }
    public intelligentUpdate(param) {
        return this.http.patch('/UI/Rest/Chassis/Enclosure/Thermal', param);
    }
}
