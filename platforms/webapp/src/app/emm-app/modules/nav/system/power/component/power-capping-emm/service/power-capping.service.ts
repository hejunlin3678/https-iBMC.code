import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Blade } from '../model/blade';
import { Chassis } from '../model/chassis';

@Injectable({
    providedIn: 'root'
})
export class PowerCappingService {

    constructor(
        private http: HttpService
    ) { }

    public getPower() {
        return this.http.get('/UI/Rest/Chassis/Enclosure/Power');
    }

    public setPower(chassisParam, bladeParam) {
        return this.http.patch('/UI/Rest/Chassis/Enclosure/Power', this.getPowerParamJSON(chassisParam, bladeParam));
    }

    public getPowerParamJSON(chassisParam, bladeParam) {
        let param = {};
        param = Object.assign(param, {...chassisParam}, {...bladeParam});
        return param;
    }

    public getBladePowerInfo(bladeInfo): Blade[] {
        const blades = bladeInfo.map((blade: any, i: number) => {
            const param: Blade = {
                id: blade.Id,
                actualPower: blade.PowerConsumedWatts || '--',
                capValue: blade.LimitInWatts,
                recommendMinCapValue: blade.MinimumValueWatts,
                isManual: blade.IsManual,
                failurePolicy: blade.LimitException
            };
            return new Blade(param);
        });
        return blades;
    }

    public getPowerCappingInfo(resp): Chassis {
        const param: Chassis = {
            enable: resp.PowerLimitEnabled,
            actualPower: resp.PowerConsumedWatts,
            capStatus: resp.PowerLimitActivated,
            capValue: resp.LimitInWatts,
            maxCapValue: resp.MaximumValue,
            minCapValue: resp.MinimumValue,
            recommendMinCapValue: resp.RecommendMinimumValue,
            capThreshold: resp.PowerLimitThreshold,
            capMode: resp.PowerLimitMode,
            bladeList: this.getBladePowerInfo(resp.PowerLimitBladeInfo)
        };
        return param;
    }
}
