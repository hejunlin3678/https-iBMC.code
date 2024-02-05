import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { map } from 'rxjs/internal/operators/map';
import { Battery, BatteryItem } from './models';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';
import { CommonData } from 'src/app/common-app/models';
import { WorkingState } from './battery.datatype';

@Injectable({
   providedIn: 'root'
})
export class BatteryService {

    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

    public getBatteryInfos(): Observable<Battery> {
        return this.http.get(`/UI/Rest/System/BatteryInfo`).pipe(
            map((data) => {
                const factoryData = new Battery({
                    ratedCapacityWattHour: data.body['RatedCapacityWattHour'],
                    remainCapacityWattHour: data.body['RemainCapacityWattHour'],
                    batteryArr: this.getBatteryInfo(data.body['BBUInfo'])
                });
                return factoryData;
            })
        );
    }

    // 转换工作状态的i18
    private getWorkingState (type: string) {
        let workingStateName = '';
        switch (type) {
            case WorkingState.Offline:
                workingStateName = this.translate.instant('SYSTEM_BATTERY_STATE_OFFLINE');
                break;
            case WorkingState.Sleep:
                workingStateName = this.translate.instant('SYSTEM_BATTERY_STATE_SLEEP');
                break;
            case WorkingState.Charging:
                workingStateName = this.translate.instant('SYSTEM_BATTERY_STATE_CHARGING');
                break;
            case WorkingState.Discharging:
                workingStateName = this.translate.instant('SYSTEM_BATTERY_STATE_DISCHARGING');
                break;
            case WorkingState.Undefined:
                workingStateName = this.translate.instant('SYSTEM_BATTERY_STATE_UNDEFINED');
                break;
            default:
                workingStateName = this.translate.instant('SYSTEM_BATTERY_STATE_UNDEFINED');
        }
        return workingStateName;
    }

    private getBatteryInfo(items: any): Observable<any> {
        const batteryInfos: any = [];
        items?.forEach((data: any, index: any) => {
            const powerItemData = new BatteryItem({
                id: 'irmBatteryDetailForm' + index,
                // 槽位
                slot: parseInt(data.Id, 10) < 9 ? ('0' + data.Id) : data.Id,
                // 丝印
                deviceLocator: data.DeviceLocator ? data.DeviceLocator : CommonData.isEmpty,
                // 厂商
                manufacturer: data.Manufacturer ? data.Manufacturer : CommonData.isEmpty,
                // 型号
                model: data.Model ? data.Model : CommonData.isEmpty,
                // 固件版本
                firmwareVersion: data.FirmwareVersion ? data.FirmwareVersion : CommonData.isEmpty,
                // 部件编码
                partNumber: data.PartNumber ? data.PartNumber : CommonData.isEmpty,
                // 运行状态
                workingState: this.getWorkingState(data.WorkingState),
                // 额定容量(Wh)
                capacity: data.RatedCapacityWattHour !== null ? data.RatedCapacityWattHour : CommonData.isEmpty,
                // 放电次数
                dischargeTimes: data.DischargeTimes !== null ? data.DischargeTimes : CommonData.isEmpty,
                // 使用时长(h)
                usageDuration: data.WorkingHours !== null ? data.WorkingHours : CommonData.isEmpty,
                // SOC
                SOC: data.RemainingPowerPercent !== null ? data.RemainingPowerPercent + '%' : CommonData.isEmpty,
                // SOH
                SOH: data.RemainingHealthPercent !== null ? data.RemainingHealthPercent + '%' : CommonData.isEmpty,
            });
            batteryInfos.push(powerItemData);
            batteryInfos?.sort((a, b) => a.slot - b.slot);
        });
        return batteryInfos;
    }
}

