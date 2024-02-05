import { Injectable } from '@angular/core';
import { FDMNode } from '../../classes';
import { map } from 'rxjs/internal/operators/map';
import { FDMFactory } from '../fdm-factory';
import { mergeMap } from 'rxjs/internal/operators/mergeMap';
import { isEmptyProperty } from 'src/app/common-app/utils/common';
import { TranslateService } from '@ngx-translate/core';
import { of } from 'rxjs';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class DetailsService {

    constructor(
        private http: HttpService,
        private i18n: TranslateService
    ) { }

    factory(node: FDMNode) {
        const deviceUrl = node.getDeviceInfo;
        const componentID = node.getComponentID;
        const onlineTime = node.getOnlineTime;
        return this.getData(deviceUrl).pipe(
            mergeMap(([bodyData]) => {
                const liveTime = isEmptyProperty(onlineTime);
                let result = null;
                switch (componentID) {
                    case 0:
                        result = of('').pipe(map(() => FDMFactory.createCPUData(bodyData, liveTime)));
                        break;
                    case 16:
                        result = of('').pipe(map(() => FDMFactory.createMainboard(bodyData, liveTime)));
                        break;
                    case 3:
                        result = of('').pipe(map(() => FDMFactory.createPSU(bodyData, liveTime, deviceUrl)));
                        break;
                    case 1:
                        result = of('').pipe(map(() => FDMFactory.createDIMM(bodyData, liveTime)));
                        break;
                    case 4:
                        result = of('').pipe(map(() => FDMFactory.createFans(bodyData, liveTime, this.i18n)));
                        break;
                    case 13:
                        result = of('').pipe(map(() => FDMFactory.createLom(bodyData, liveTime)));
                        break;
                    case 15:
                        result = of('').pipe(map(() => FDMFactory.createRiserCard(bodyData, liveTime)));
                        break;
                    case 6:
                        result = of('').pipe(map(() => FDMFactory.createRaid(bodyData, liveTime)));
                        break;
                    case 5:
                        result = of('').pipe(map(() => FDMFactory.createDiskBP(bodyData, liveTime)));
                        break;
                    case 2:
                        result = of('').pipe(map(() => FDMFactory.createDisk(bodyData, liveTime, this.i18n)));
                        break;
                    case 8:
                        result = of('').pipe(map(() => FDMFactory.createPcieCard(bodyData, liveTime)));
                        break;
                    case 18:
                        result = of('').pipe(map(() => FDMFactory.createChassis(liveTime)));
                        break;
                    default:
                        result = of('').pipe(map(() => { }));
                }
                return result;
            })
        );
    }

    /**
     *  发送请求
     *  @param url 请求链接
     */
    private getData(url: string) {
        return this.http.get(url).pipe(
            map(
                (data: any) => {
                    if (!(data.body)) {
                        data.body = {};
                    }
                    return [data.body];
                }
            )
        );
    }
}
