import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { Observable } from 'rxjs';

@Injectable({
    providedIn: 'root'
})
export class TpcmService {
    constructor(private http: HttpService, private user: UserInfoService) {}

    public getTPCMBasicInfo(): Observable<any> {
        return this.http.get(`/UI/Rest/AccessMgnt/TPCMBasicInfo`);
    }

    public getTPCMDetailedInfo(): Observable<any> {
        return this.http.get(`/UI/Rest/AccessMgnt/TPCMDetailedInfo`);
    }

    public getTpcmService(): Observable<any> {
        return this.http.get(`/UI/Rest/AccessMgnt/TpcmService`);
    }

    public setTpcmService(param) {
        return this.http.patch(`/UI/Rest/AccessMgnt/TpcmService`, param);
    }
    public updateService(param) {
        return this.http.patch(`/UI/Rest/AccessMgnt/TpcmService/SetStandardDigest`, param);
    }
}
