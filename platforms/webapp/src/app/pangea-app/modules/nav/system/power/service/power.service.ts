import { Injectable } from '@angular/core';
import { PowerInfoCard } from '../model/power-info-card';
import { TranslateService } from '@ngx-translate/core';
import { W_TO_BTU_COEFF } from '../interface';
import Date from 'src/app/common-app/utils/date';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class PowerService {

    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

    public getpowerSupply() {
        return this.http.get('/UI/Rest/System/PowerSupply');
    }

    public setPowerSupply(param) {
        return this.http.patch('/UI/Rest/System/PowerSupply', param);
    }

    public getPower() {
        return this.http.get('/UI/Rest/System/Power');
    }

    public setPower(param) {
        return this.http.patch('/UI/Rest/System/Power', param);
    }

    public getPowerControl() {
        return this.http.get('/UI/Rest/System/PowerControl');
    }

    public setPowerControl(param) {
        return this.http.patch('/UI/Rest/System/PowerControl', param);
    }

    public resetPowerCapping() {
        return this.http.post('/UI/Rest/System/Power/ResetStatistics', {});
    }

    public clearHistoryData() {
        return this.http.delete('/UI/Rest/System/Power/History');
    }

    public queryTask(url) {
        return this.http.get(url);
    }

    public setPowerOnOff(param) {
        return this.http.post('/UI/Rest/System/PowerControl', param);
    }

    public downLoadHistory() {
        return this.http.get('/UI/Rest/System/Power/History');
    }

    public getPowerType(activeStandby) {
        let type = null;
        if (activeStandby === 'Active') {
            type = this.translate.instant('POWER_MAIN');
        } else if (activeStandby === 'Standby') {
            type = this.translate.instant('POWER_STANDBY');
        } else if (activeStandby === 'Shared') {
            type = this.translate.instant('POWER_LOAD_BALANCING');
        } else {
            type = this.translate.instant('POWER_NOT_DETECTED');
        }
        return type;
    }

    public getPowerInfo(data) {
        const supplyList = data.SupplyList;
        const powerInfoList = [];
        supplyList.forEach((powerInfo, index) => {
            const sharModelLabel = this.getPowerType(powerInfo.Mode);
            let powerName = '--';
            if (powerInfo.Name) {
                powerName = 'PSU' + powerInfo.Name.substr(3);
            }
            const absent = powerInfo.State === 'Absent';
            const info = {
                name: powerName,
                powerInputWatts: (powerInfo.InputWatts === null || absent) ? '--' : powerInfo.InputWatts,
                activeStandby: absent ? this.translate.instant('POWER_NOT_DETECTED') : sharModelLabel,
                slotId: absent ? '--' : index,
                manufacturer: (powerInfo.Manufacturer === null || absent) ? '--' : powerInfo.Manufacturer,
                model: (powerInfo.Model === null || absent) ? '--' : powerInfo.Model,
                serialNumber: (powerInfo.SerialNumber === null || absent) ? '--' : powerInfo.SerialNumber,
                firmwareVersion: (powerInfo.FirmwareVersion === null || absent) ? '--' : powerInfo.FirmwareVersion,
                partNumber: (powerInfo.PartNumber === null || absent) ? '--' : powerInfo.PartNumber,
                powerCapacityWatts: (powerInfo.PowerCapacityWatts === null || absent) ? '--' : powerInfo.PowerCapacityWatts + ' W',
                powerSupplyType: (powerInfo.PowerSupplyType === null || absent) ? '--' : powerInfo.PowerSupplyType,
                lineInputVoltage: (powerInfo.LineInputVoltage === null || absent) ? '--' : powerInfo.LineInputVoltage + ' V',
                outputVoltage: (powerInfo.OutputVoltage === null || absent) ? '--' : powerInfo.OutputVoltage + ' V',
                isAbsent: absent
            };
            const powerInfoCard = new PowerInfoCard(info);
            powerInfoList.push(powerInfoCard);
        });
        return powerInfoList;
    }

    public getPowerSettingInfo(data) {
        const supplyList = data.ExpectedSupplyList;
        const list = [];
        let disabledState = true;
        if (data.PowerMode === 'Active/Standby') {
            disabledState = false;
        }
        supplyList.forEach((powerInfo, index) => {
            if (powerInfo.Mode === 'Active') {
                list.push({
                    label: powerInfo.Name.substr(0, 3) + index,
                    name: powerInfo.Name,
                    disabled: disabledState,
                    checked: true
                });
            } else {
                list.push({
                    label: powerInfo.Name.substr(0, 3) + index,
                    name: powerInfo.Name,
                    disabled: disabledState,
                    checked: false
                });
            }
        });
        const sleep = data.DeepSleep ? data.DeepSleep === 'On' : null;
        const powerSetting = {
            isActiveAndStandby: data.PowerMode || null,
            activePSU: list,
            deepSleep: sleep,
            normalAndRedundancy: data.NormalAndRedundancy === 'Enabled'
        };
        return powerSetting;
    }

    public wattstoBTUs(value) {
        if (!value) {
            return value;
        }
        const ret = parseFloat(Number(value / W_TO_BTU_COEFF).toFixed(1));
        return ret === 0.0 ? 0 : ret;
    }

    public getPowerCapping(data, utils?) {
        const metrics = data.Metrics ? data.Metrics[0] : false;
        const powerLimit = data.PowerLimit || false;
        let tmpDate = metrics.StatisticsCollected;
        if (tmpDate !== '' && tmpDate !== null && tmpDate !== undefined) {
            tmpDate = tmpDate.substr(0, 19);
            tmpDate = tmpDate.replace('T', ' ');
            tmpDate = tmpDate.replace(/-/g, '/');
        }
        let tmpPro = metrics.MaxConsumedOccurred;
        if (tmpPro !== '' && tmpPro !== null && tmpPro !== undefined) {
            tmpPro = tmpPro.substr(0, 19);
            tmpPro = tmpPro.replace('T', ' ');
            tmpPro = tmpPro.replace(/-/g, '/');
        }
        let limitState = null;
        if (powerLimit && powerLimit.LimitState === 'On') {
            limitState = true;
        } else if (powerLimit && powerLimit.LimitState === 'Off') {
            limitState = false;
        }
        let limitExceptionState = null;
        if (powerLimit && powerLimit.LimitException === 'HardPowerOff') {
            limitExceptionState = true;
        } else if (powerLimit && powerLimit.LimitException === 'NoAction') {
            limitExceptionState = false;
        }
        const targetData = {
            powerMetrics: metrics,
            powerConsumedWatts: metrics ? metrics.PowerConsumedWatts : null,
            averageConsumedWatts: metrics ? metrics.AverageConsumedWatts : null,
            currentCPUPowerWatts: metrics ? metrics.CurrentCPUPowerWatts : null,
            currentMemoryPowerWatts: metrics ? metrics.CurrentMemoryPowerWatts : null,
            totalConsumedPowerkWh: metrics ? metrics.TotalConsumedPowerkWh.toFixed(2) : null,
            maxConsumedWatts: metrics ? metrics.MaxConsumedWatts : null,
            powerLimitState: limitState,
            isPowerCapSet: powerLimit,
            isLimitState: powerLimit ? powerLimit.PowerLimitExceptionSupported : false,
            limitInWatts: powerLimit ? powerLimit.LimitInWatts : null,
            minPowerLimitInWatts: powerLimit ? powerLimit.MinLimitInWatts : null,
            maxPowerLimitInWatts: powerLimit ? powerLimit.MaxLimitInWatts : null,
            limitException: limitExceptionState,
            statisticsCollected: tmpDate,
            maxConsumedOccurred: tmpPro,
            highPowerThresholdWatts: data.HighPowerThresholdWatts,
            powerCapMode: powerLimit ? powerLimit.MSPPEnabled : null
        };
        return targetData;
    }

    public getPowerHistoryInfo(data) {
        const historyData = data.History;
        const powerPeakWatts = [];
        const powerAverageWatts = [];
        const times = [];
        historyData.forEach(element => {
            const time = new Date(element.Time).pattern('yyyy/MM/dd HH:mm');
            powerPeakWatts.push([time, element.PowerPeakWatts]);
            powerAverageWatts.push([time, element.PowerAverageWatts]);
            times.push(time);
        });
        const targetData = {
            powerPeakWattsList: powerPeakWatts,
            powerAverageWattsList: powerAverageWatts,
            timeList: times
        };
        return targetData;
    }

    public getPowerControlInfo(data) {
        const powerRestoreDelay = data.PowerRestoreDelay;
        const powerOffAfterTimeout = data.PowerOffAfterTimeout;
        const targetData = {
            powerState: data.PowerState,
            powerOnStrategy: data.PowerOnStrategy,
            powerRestoreDelayMode: powerRestoreDelay.Mode || 'DefaultDelay',
            powerRestoreDelaySeconds: powerRestoreDelay.Seconds || 0,
            panelPowerButtonEnabled: data.PanelPowerButtonEnabled === 'On' ? true : false,
            powerOffDefaultSeconds: powerOffAfterTimeout.DefaultSeconds,
            safePowerOffMaxTimeoutSeconds: powerOffAfterTimeout.MaxSeconds,
            safePowerOffMinTimeoutSeconds: powerOffAfterTimeout.MinSeconds,
            safePowerOffDefaultTimeoutSeconds: powerOffAfterTimeout.DefaultSeconds,
            safePowerOffTimoutSeconds: powerOffAfterTimeout.Seconds || (powerOffAfterTimeout.DefaultSeconds || 600),
            safePowerOffTimeoutEnabled: powerOffAfterTimeout.Enabled === 'On' ? true : false,
            leakDetectionSupport: data.LeakDetectionSupport,
            leakStrategy: data.LeakStrategy,
            // 该字段暂未提供
            wakeOnLANStatus: powerRestoreDelay.wakeOnLANStatus
        };
        return targetData;
    }

    public getChassisTypeInfo(data) {
        return data.ChassisType;
    }

}
