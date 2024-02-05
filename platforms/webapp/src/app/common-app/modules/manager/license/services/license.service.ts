
import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
@Injectable({
    providedIn: 'root'
})
export class LicenseService {

    constructor(
        private http: HttpService,
        private user: UserInfoService
    ) {

    }
    // 查询license信息
    public findLicenseInfo () {
        return this.http.get(`/UI/Rest/BMCSettings/LicenseService`);
    }

    // 安装
    public uploadLicense (params) {
        return this.http.post(`/UI/Rest/BMCSettings/LicenseService/InstallLicense`, params);
    }

    // 导出
    public download (params) {
        return this.http.post(`/UI/Rest/BMCSettings/LicenseService/ExportLicense`, params);
    }

    // 查询task
    public getTask (url) {
      return this.http.get(url);
    }

    // 失效
    public licenseInvalid (params) {
        return this.http.post(`/UI/Rest/BMCSettings/LicenseService/DisableLicense`, params);
    }

    // 删除
    public licenseDel (params) {
        return this.http.post(`/UI/Rest/BMCSettings/LicenseService/DeleteLicense`, params);
    }
}
