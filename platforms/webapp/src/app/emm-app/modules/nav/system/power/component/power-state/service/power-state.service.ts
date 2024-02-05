import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { HttpService } from 'src/app/common-app/service';
import { BoardDomain } from '../model/board-domain';
import { PowerInfo } from '../model/power-info';
import { PowerSleep } from '../model/power-sleep';
import { ProductDomain } from '../model/product-domain';

@Injectable({
    providedIn: 'root'
})
export class PowerStateService {
    constructor(
        private http: HttpService
    ) { }

    public isPowerHealth: boolean = true;

    public getPower() {
        return this.http.get('/UI/Rest/Chassis/Enclosure/PowerSupply');
    }

    public setPowerSleep(param: any): Observable<any> {
        return this.http.patch('/UI/Rest/Chassis/Enclosure/PowerSupply', param);
    }
    public isShowedValue(value: any) {
        let val;
        if (value === null) {
            val = '--';
        } else {
            val = value;
        }
        return val;
    }
    private getBoardInfo(board: any): BoardDomain {
        const boardInfo = {
            fileId: this.isShowedValue(board.FruFileId),
            manufacturer: this.isShowedValue(board.Manufacturer),
            manufacturingDate: this.isShowedValue(board.ManufacturingDate),
            partNumber: this.isShowedValue(board.PartNumber),
            productName: this.isShowedValue(board.ProductName),
            serialNum: this.isShowedValue(board.SerialNumber)
        };
        return new BoardDomain(boardInfo);
    }

    private getProductInfo(product: any): ProductDomain {
        const productInfo: ProductDomain = {
            manufacturer: this.isShowedValue(product.Manufacturer),
            partNumber: this.isShowedValue(product.PartNumber),
            productName: this.isShowedValue(product.ProductName),
            serialNum: this.isShowedValue(product.SerialNumber),
            version: this.isShowedValue(product.Version)
        };
        return new ProductDomain(productInfo);
    }

    private getPowerSupplyType(powerType: string): string {
        let powerSupplyType = '--';
        if (powerType) {
            if (powerType.lastIndexOf('ACorDC') >= 0) {
                powerSupplyType = 'POWER_AC_OR_DC';
            } else if (powerType.lastIndexOf('HVDC') >= 0) {
                powerSupplyType = 'POWER_HVDC';
            } else if (powerType.lastIndexOf('AC') >= 0) {
                powerSupplyType = 'POWER_AC';
            } else if (powerType.lastIndexOf('DC') >= 0) {
                powerSupplyType = 'POWER_DC';
            } else {
                powerSupplyType = '--';
            }
        }
        return powerSupplyType;
    }

    public getPowerInfo(powerResp: any): PowerInfo[] {
        const powerInfos: PowerInfo[] = [];
        const powerSupplies = powerResp.SupplyList;
        powerSupplies.map((powerSupplie) => {
            const softHard = `${this.isShowedValue(powerSupplie.FirmwareVersion)} / ${this.isShowedValue(powerSupplie.HardwareVersion)}`;
            let powerHealthState = '';
            let imgType = '';
            // 1.判断是否在位；2.判断在位是否健康；3.判断在位健康，是否休眠
            if (powerSupplie.State !== 'Enabled') {
                powerHealthState = 'POWER_NOT_POSITION';
                imgType = 'notPosition';
            } else {
                if (powerSupplie.Health === 'Critical') {
                    powerHealthState = 'POWER_ABNORMAL';
                    imgType = 'abnormal';
                    this.isPowerHealth = false;
                } else if (powerSupplie.Health === 'OK') {
                    if (powerSupplie.SleepState) {
                        powerHealthState = 'POWER_SLEEPING';
                        imgType = 'sleeping';
                    } else {
                        powerHealthState = 'POWER_HEALTH';
                        imgType = 'health';
                    }
                }
            }
            const info: PowerInfo = {
                slot: powerSupplie.Name,
                powerType: this.getPowerSupplyType(powerSupplie.SupplyType),
                powerSoftHard: softHard,
                currentPower: powerSupplie.PowerOutputWatts || 0,
                ratedPower: powerSupplie.PowerCapacityWatts || 0,
                powerState: powerHealthState,
                powerStateImg: imgType,
                boardInfo: powerSupplie?.Fru?.Board === undefined ?  null : this.getBoardInfo(powerSupplie?.Fru?.Board),
                productInfo: powerSupplie?.Fru?.Product === undefined ? null : this.getProductInfo(powerSupplie?.Fru?.Product)
            };
            powerInfos.push(new PowerInfo(info));
        });
        return  powerInfos;
    }

    public getPowerSleep(powerResp: any): PowerSleep {
        const powerSleep: PowerSleep = {
            isPowerHealth: this.isPowerHealth,
            isPowerSleep: powerResp.SleepModeEnabled && powerResp.SleepMode,
            sleepEnable: powerResp.SleepModeEnabled,
            sleepMode: powerResp.SleepMode
        };
        return powerSleep;
    }
}
