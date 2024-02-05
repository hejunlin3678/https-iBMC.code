import { Injectable } from '@angular/core';
import { FDMNode } from '../../classes';
import { HttpService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { FDMFactory } from '../fdm-factory';
import { displayDateTime } from '../../utils/fdm.utils';
import { TranslateService } from '@ngx-translate/core';

@Injectable({
    providedIn: 'root'
})
export class DeviceCaseService {

    constructor(
        private http: HttpService,
        private i18n: TranslateService
    ) { }

    factory(node: FDMNode) {
        const eventUrl = node.getEventReports;
        return this.getData(eventUrl).pipe(
            map((eventData) => {
                const eventRecords = eventData[0].EventRecords;
                const caseArr = this.getCases(eventRecords);
                const result = FDMFactory.createDeviceCase(this.i18n, caseArr);
                return result;
            })
        );
    }

    private getCases(arr) {
        const caseArr = [];
        arr.forEach((item, index) => {
            const no = index + 1;
            const createTime = displayDateTime(item.ReportTimeStamp, item.ReportTimeZone);
            const type = item.Type;
            const level = item.Level;
            const info = item.Information;
            const temp: CaseArrItem = {
                no,
                type,
                level,
                createTime,
                info
            };
            caseArr.push(FDMFactory.createCaseItem(temp));

        });
        return caseArr;
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
interface CaseArrItem {
    no: number;
    type: string;
    level: string;
    createTime: string;
    info: string;
}
