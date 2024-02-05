import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { ISecurityNoticeData, ISaveParam } from '../security-notice.datatype';
import { map } from 'rxjs/internal/operators/map';
import { pluck } from 'rxjs/internal/operators/pluck';

@Injectable({
    providedIn: 'root'
})
export class SecurityNoticeService {

    constructor(
        private http: HttpService
    ) { }

    public getData() {
        return this.getUserInfo();
    }

    private getUserInfo(): Observable<ISecurityNoticeData> {
        const url = '/UI/Rest/AccessMgnt/SecurityBanner';
        return this.http.get(url).pipe(
            pluck('body'),
            map(data => this.factoryData(data))
        );
    }

    private factoryData(data: any): ISecurityNoticeData {
        const {
            Enabled: enabled,
            SecurityBanner: securityBanner,
            DefaultSecurityBanner: defaultValue,
        } = data;
        return {
            value: securityBanner,
            status: enabled,
            defaultValue,
        };
    }

    private updateUser(params: any) {
        const url = '/UI/Rest/AccessMgnt/SecurityBanner';
        return this.http.patch(url, params);
    }

    public saveParams(obj: ISecurityNoticeData) {
        const saveParm: ISaveParam = {};
        if (typeof (obj.status) === 'boolean') {
            saveParm.Enabled = obj.status;
        }
        saveParm.SecurityBanner = obj.value;
        return this.updateUser(saveParm).pipe(
            pluck('body'),
            map(data => this.factoryData(data))
        );
    }

}
