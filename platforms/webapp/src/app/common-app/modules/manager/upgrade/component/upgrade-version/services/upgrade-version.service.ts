import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class UpgradeVersionService {

    constructor(
        private http: HttpService
    ) {

    }

    // 固件升级重启
    public resetFirmware(params) {
        return this.http.post('/UI/Rest/BMCSettings/UpdateService/Reset', params);
    }

    // 切换iBMC镜像
    public rollBackFirmware(params) {
        return this.http.post('/UI/Rest/BMCSettings/UpdateService/SwitchImage', params);
    }

}
