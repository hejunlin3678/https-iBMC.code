import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { map } from 'rxjs/internal/operators/map';
import { Observable } from 'rxjs/internal/Observable';
import { Power, PowerItem } from './models';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';
import { ChannelText, PowerState } from './power.datatype';

@Injectable({
    providedIn: 'root'
})
export class PowerService {

    constructor(
        private http: HttpService,
        private translate: TranslateService,
        private user: UserInfoService
    ) { }

    public remainCapacityWattHour: any = 0;
    public powerConsumedWatts: any;
    public powerInfos: any = [];

    // 转换工作状态的i18
    public getSupplyChannelText (name: any) {
        let channelText = '';
        switch (name) {
            case ChannelText.MainCircuit:
                channelText = this.translate.instant('IRM_SYSTEM_POWER_MAIN_CIRCUIT');
                break;
            case ChannelText.BackupCircuit:
                channelText = this.translate.instant('IRM_SYSTEM_POWER_BACKUP_CIRCUIT');
                break;
            default :
                channelText = CommonData.isEmpty;
        }
        return channelText;
    }

    // 转换状态的i18
    public getAbsentText (name: any) {
        let stateName = '';
        switch (name) {
            case PowerState.Enabled:
                stateName = this.translate.instant('IRM_SYSTEM_POWER_LOAD_BALANCING');
                break;
            case PowerState.Absent:
                stateName = this.translate.instant('IRM_SYSTEM_POWER_UNDEFINED_POWER_LABEL');
                break;
            default :
                stateName = CommonData.isEmpty;
        }
        return stateName;
    }

    public getpowerSupply(): Observable<Power> {
        return this.http.get('/UI/Rest/System/PowerSupply').pipe(
            map((res) => {
                const powerSupplies = res.body.SupplyList;
                this.powerConsumedWatts = res.body.PowerConsumedWatts !== null ? res.body.PowerConsumedWatts : CommonData.isEmpty;
                this.remainCapacityWattHour = 0;
                this.powerInfos = [];
                powerSupplies.forEach((item: any, index: any) => {
                    this.remainCapacityWattHour += item.PowerCapacityWatts;
                    let slots = index;
                    if (parseInt(slots, 10) < 9) {
                        slots = '0' + (parseInt(index, 10) + 1);
                    } else {
                        slots = parseInt(index, 10) + 1;
                    }
                    const powerItemInfo = new PowerItem({
                        // id
                        id: 'irmPowerDetailForm' + index,
                        // 输入功率
                        powerInputWatts: item.InputWatts != null ? item.InputWatts : CommonData.isEmpty,
                        // 负载均衡、未检测到
                        isAbsent: item.State,
                        // 负载均衡、未检测到名称
                        isAbsentText: this.getAbsentText(item.State),
                        // 槽位
                        slot: slots ? slots : CommonData.isEmpty,
                        // 厂商
                        manufacturer: item.Manufacturer ? item.Manufacturer : CommonData.isEmpty,
                        // 类型
                        model: item.Model ? item.Model : CommonData.isEmpty,
                        // 序列号
                        serialNumber: item.SerialNumber ? item.SerialNumber : CommonData.isEmpty,
                        // 固件版本
                        firmwareVersion: item.FirmwareVersion ? item.FirmwareVersion : CommonData.isEmpty,
                        // 部件编码
                        partNumber: item.PartNumber ? item.PartNumber : CommonData.isEmpty,
                        // 额定功率
                        powerCapacityWatts: item.PowerCapacityWatts != null ? item.PowerCapacityWatts + ' W' : CommonData.isEmpty,
                        // 输入模式
                        powerSupplyType: item.PowerSupplyType ? item.PowerSupplyType : CommonData.isEmpty,
                        // 输入电压
                        lineInputVoltage: item.LineInputVoltage != null ? parseInt(item.LineInputVoltage, 10) + ' V' : CommonData.isEmpty,
                        // 输出电压
                        outputVoltage: item.OutputVoltage != null ? (item.OutputVoltage > 0 ? item.OutputVoltage.toFixed(3) : 0) + ' V'
                            : CommonData.isEmpty,
                        // 供电线路
                        powerSupplyChannel: this.getSupplyChannelText(item.PowerSupplyChannel)
                    });
                    this.powerInfos.push(powerItemInfo);
                });
                const factoryData = new Power({
                    powerConsumedWatts:  this.powerConsumedWatts,
                    remainCapacityWattHour: this.remainCapacityWattHour,
                    powerArr: this.powerInfos
                });
                return factoryData;
            })
        );
    }
}
