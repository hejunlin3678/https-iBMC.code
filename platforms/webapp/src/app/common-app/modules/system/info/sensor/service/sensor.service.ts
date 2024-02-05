import { Injectable } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { HttpService } from 'src/app/common-app/service';
import { ISensorTableRow, sensorUnit, statusList, IStatusObj } from '../sensor.datatype';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';

@Injectable({
    providedIn: 'root'
})
export class SensorService {

    public getData() {
        return this.getSensors();
    }

    private getSensors(): Observable<ISensorTableRow[]> {
        const url = '/UI/Rest/System/ThresholdSensor';
        return this.http.get(url).pipe(
            pluck('body', 'List'),
            map((data) => this.factorySensorData(data))
        );
    }

    // 数组按照某个对象属性排序
    public sortBy(key: string) {
        return (a, b) => {
            if (a[key] == null) {
                return -1;
            } else if (b[key] == null) {
                return 1;
            }
            const numReg = /^[0-9]$/;
            const aValue = a[key].split('');
            const bValue = b[key].split('');
            const len = Math.min(aValue.length, bValue.length);

            for (let i = 0; i < len; i++) {
                // 将遇到的第一个数字开始截取，然后将剩余的字符串转换为数字
                if (numReg.test(aValue[i]) && numReg.test(bValue[i])) {
                    if (parseInt(a[key].substring(i), 10) > parseInt(b[key].substring(i), 10)) {
                        return 1;
                    } else if (parseInt(a[key].substring(i), 10) < parseInt(b[key].substring(i), 10)) {
                        return -1;
                    }
                }

                if (aValue[i] > bValue[i]) {
                    return 1;
                } else if (aValue[i] < bValue[i]) {
                    return -1;
                }
            }
            return aValue.length > bValue.length ? 1 : -1;
        };
    }


    private factorySensorData(resData: any[]): ISensorTableRow[] {
        const items = resData.sort(this.sortBy('Name'));
        const data: ISensorTableRow[] = [];
        const noDataStr = '--';
        items.forEach((item, index) => {
            const unit = sensorUnit[item.Unit] || '';
            const status = item.Status;
            const statusObj = status ? status.toUpperCase() : '--';
            data.push({
                id: index + 1,
                name: item.Name == null ? noDataStr : item.Name + (unit ? ` (${unit})` : ''),
                readingValue: item.ReadingValue == null ? noDataStr : item.ReadingValue,
                status,
                statusText: statusObj,
                statusClassName: statusObj && statusObj.className,
                statusIcon: statusObj && statusObj.icon,
                lowerThresholdCritical: item.LowerThresholdCritical == null ?
                    noDataStr : item.LowerThresholdCritical,
                lowerThresholdFatal: item.LowerThresholdFatal == null ?
                    noDataStr : item.LowerThresholdFatal,
                lowerThresholdNonCritical: item.LowerThresholdNonCritical == null ?
                    noDataStr : item.LowerThresholdNonCritical,
                upperThresholdNonCritical: item.UpperThresholdNonCritical == null ?
                    noDataStr : item.UpperThresholdNonCritical,
                upperThresholdCritical: item.UpperThresholdCritical == null ?
                    noDataStr : item.UpperThresholdCritical,
                upperThresholdFatal: item.UpperThresholdFatal == null ?
                    noDataStr : item.UpperThresholdFatal
            });
        });

        return data;
    }

    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

}
