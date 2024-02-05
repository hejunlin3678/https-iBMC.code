import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { UpgradeService } from '../../../services';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class UpgradeManualService {

    constructor(
        private http: HttpService,
        private upgradeService: UpgradeService
    ) {

    }
    public upgradeManual = this.upgradeService.upgradeManual;

    // 下发升级请求
    public upgradeAdd(params: {PackageList: [], ActiveMode: string}): Observable<any> {
        return this.http.post('/UI/Rest/BMCSettings/UpdateService/UpdateQueue', params);
    }

    // 手动远程升级进度查询
    public getUpdate(): Observable<any> {
        return this.http.get('/UI/Rest/BMCSettings/UpdateService/Updateprogress');
    }

}
