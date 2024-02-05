import { Injectable } from '@angular/core';
import { NtpService } from '../ntp.service';
import { AuthenData } from '../models/authen/authen-data.model';
import { Observable } from 'rxjs/internal/Observable';
import { map } from 'rxjs/internal/operators/map';
import { Authen } from '../models/authen/authen.model';
import { UserInfoService } from 'src/app/common-app/service';
import { NTPStaticModel } from '../models';
import { HttpClient, HttpHeaders } from '@angular/common/http';

@Injectable({
    providedIn: 'root'
})
export class NtpAuthenService {

    constructor(
        private ntpService: NtpService,
        private user: UserInfoService,
        private httpClient: HttpClient
    ) { }

    public factory(): Observable<{ authenData: AuthenData, authen: Authen }> {
        return this.ntpService.data.pipe(
            map((authData: any) => {
                const authenData = new AuthenData();
                const authen = new Authen();

                const authEnabled = authData.AuthEnabled;
                authen.setAuthenEnabled = authEnabled;
                authenData.setAuthEnabled = authEnabled;

                const keyStatus = authData.KeyStatus;
                authenData.setKeyStatus = keyStatus;

                const privilege = this.user.hasPrivileges(NTPStaticModel.getInstance().privileges);
                authenData.setAuthenDisabled(privilege);

                return { authenData, authen };
            })
        );
    }

    public save(ntpCopy: any, authen: Authen): object {
        const obj = {};
        for (const attr in ntpCopy) {
            if (ntpCopy[attr] !== authen[attr]) {
                const key = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                obj[key] = authen[attr];
            }
        }
        return obj;
    }

    public uploadNtpFile(fileName: string, file: any) {
        const url = '/UI/Rest/BMCSettings/NTP/ImportKey';
        const ntpFormData = new FormData();
        ntpFormData.append('Content', file, fileName);
        ntpFormData.append('Type', 'URI');
        const token = this.user.token ? this.user.token : '';
        const headers: HttpHeaders = new HttpHeaders({
            'X-CSRF-Token': token
        });
        const httpHeaders = { headers };
        return this.httpClient.post(url, ntpFormData, httpHeaders);
    }
}

