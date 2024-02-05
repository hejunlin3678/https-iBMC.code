import { Injectable } from '@angular/core';
import { UserInfoService, HttpService } from 'src/app/common-app/service';
import { HttpClient } from '@angular/common/http';

@Injectable({
    providedIn: 'root'
})
export class SyslogService {
    constructor(
        private http: HttpService,
        private user: UserInfoService,
        private httpClient: HttpClient
    ) { }

    getSyslogInfo() {
        return this.http.get(`/UI/Rest/Maintenance/SyslogNotification`);
    }
    // syslog保存
    sysLogSave(params) {
        return this.http.patch(`/UI/Rest/Maintenance/SyslogNotification`, params);
    }
    // 导入本地根证书
    imporCustomCertificate(params, headers) {
        return this.httpClient.post(
            `/UI/Rest/Maintenance/SyslogNotification/ImportClientCertificate`,
            params,
            headers
        );
    }
    // 列表测试接口
    sysLogSendTest(params) {
        return this.http.post(`/UI/Rest/Maintenance/TestSyslogNotification`, params);
    }
}
