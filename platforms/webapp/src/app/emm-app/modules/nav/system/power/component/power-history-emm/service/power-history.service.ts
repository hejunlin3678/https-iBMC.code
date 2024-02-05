import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import Date from 'src/app/common-app/utils/date';

@Injectable({
    providedIn: 'root'
})
export class PowerHistoryService {

    constructor(
        private http: HttpService
    ) { }

    public getPowerHistory() {
        return this.http.get('/UI/Rest/System/Power');
    }

    public getPowerHistoryInfo(data, day: number) {
        const historyData = data;
        const currentTime = sessionStorage.getItem('DateTime').slice(0, 16);
        const spentedTime = 1000 * 60 * 60 * 24 * day;
        const startTime = new Date(currentTime).getTime() - spentedTime;
        const chartDate = {
            // 实际功率
            realData: [],
            // 平均功率
            avgData: [],
            // 时间
            xData: [],
        };
        const realPowerList = [];
        const historyInfo = {
            maximumPower: 0,
            minimumPower: 0,
            averagePower: 0
        };
        if (historyData?.length > 0) {
            let sumPower = 0;
            historyData.forEach(element => {
                const time = new Date(element.Time).pattern('yyyy/MM/dd HH:mm');
                if (new Date(time).getTime() >= startTime) {
                    chartDate.realData.push([time, element.PowerWatts]);
                    chartDate.avgData.push([time, element.PowerAverageWatts]);
                    chartDate.xData.push(time);
                    realPowerList.push(element.PowerWatts);
                    sumPower += (element.PowerAverageWatts * 1);
                }
            });
            if (chartDate.realData.length > 0) {
                historyInfo.maximumPower = Math.max.apply(null, realPowerList);
                historyInfo.minimumPower = Math.min.apply(null, realPowerList);
                historyInfo.averagePower = Math.round(sumPower / chartDate.avgData.length);
            }
        }
        const targetData = {
            chartDate,
            historyInfo
        };
        return targetData;
    }
    public setPower(param) {
        return this.http.patch('/UI/Rest/System/Power', param);
    }
    public clearHistoryData() {
        return this.http.delete('/UI/Rest/System/Power/History');
    }

    public downLoadHistory() {
        return this.http.get('/UI/Rest/System/Power/History');
    }

    public queryTask(url) {
        return this.http.get(url);
    }
}
