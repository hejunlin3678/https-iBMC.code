import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { HttpClient } from '@angular/common/http';

@Injectable({
    providedIn: 'root'
})
export class CertUpdateService {
    constructor(
        private http: HttpService,
        private httpClient: HttpClient
    ) {}

    // 获取所有数据
    public getData() {
        return this.http.get('/UI/Rest/AccessMgnt/CertUpdateService');
    }

    // 更新使能状态 和 保存
    certUpdateSaveData(params) {
        return this.http.patch(`/UI/Rest/AccessMgnt/CertUpdateService`, params);
    }

    // 更新证书
    updateCertFromCA() {
        return this.http.post(`UI/Rest/AccessMgnt/CertUpdateService/UpdateCertFromCA`, {});
    }

    // 导入客户端证书
    imporCustomCertificate(params, headers) {
        return this.httpClient.post(
            `/UI/Rest/AccessMgnt/CertUpdateService/ImportClientCert`,
            params,
            headers
        );
    }

    public getBootOptionsServer() {
        return this.http.get('/UI/Rest/System/BootOptions');
    }

    public getBootCertificates() {
        return this.http.get('/UI/Rest/AccessMgnt/BootCertificates');
    }

    public getSecureBootState() {
        return this.http.get('/UI/Rest/AccessMgnt/BootCertificates/SecureBoot');
    }

    public setSecureBootState(params) {
        return this.http.patch('/UI/Rest/AccessMgnt/BootCertificates/SecureBoot', params);
    }
}
