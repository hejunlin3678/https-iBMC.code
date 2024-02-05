import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { UpgradeModel } from '../location-upgrade/upgrade-model';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class LocationUpgradeService {

    constructor(
        private http: HttpService
    ) {

    }

    // 进度查询
    public queryTaskProgress() {
        return this.http.get('/UI/Rest/BMCSettings/UpdateService/UpdateProgress');
    }

    public getPowerControl() {
        return this.http.get('/UI/Rest/System/PowerControl');
    }


    public upgradeList = () => {
        return new Observable((observer) => {
            this.http.get('/UI/Rest/BMCSettings/UpdateService').subscribe((response) => {
                const upgrade = new UpgradeModel();
                const resData = response['body'];
                upgrade.setActiveModeSupported(resData.ActiveModeSupported);
                upgrade.setActiveMode(resData.ActiveMode);
                upgrade.setUpdatingFlag(resData.UpdatingFlag);
                observer.next([upgrade]);
            });
        });
    }
}
