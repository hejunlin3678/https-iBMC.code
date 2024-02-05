import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { IFansInfo } from '../../model';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { Observable } from 'rxjs/internal/Observable';
import { forkJoin } from 'rxjs';
import { CommonData } from 'src/app/common-app/models';
@Injectable({
    providedIn: 'root'
})
export class FanInfoService {
    constructor(
        private http: HttpService
    ) { }

    factory(): Observable<IFansInfo[]> {
        const getFansInfo$ = this.http.get('/UI/Rest/Chassis/Enclosure/Thermal').pipe(pluck('body'));
        return forkJoin([getFansInfo$]).pipe(
            map(
                ([fansInfo]) => {
                    return this.factoryFansInfo(fansInfo?.Fan);
                }
            )
        );
    }
    private factoryFansInfo(lists: any[]): IFansInfo[] {
        const resultData: IFansInfo[] = [];
        if (lists && lists.length > 0) {
            lists.forEach(item => {
                const fanDetailInfo: IFansInfo = {
                    slot: item?.Name || CommonData.isEmpty,
                    boardType: item?.Fru?.Board?.ProductName || CommonData.isEmpty,
                    HardwareVersion: item?.PcbVersion || CommonData.isEmpty,
                    softwareVersion: item?.SoftwareVersion || CommonData.isEmpty,
                };
                resultData.push(fanDetailInfo);
            });
        }
        return resultData;
    }
}
