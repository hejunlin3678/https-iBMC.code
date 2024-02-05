import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class CertificateService {
    constructor(
        private http: HttpService,
    ) { }

    // 删除吊销证书
    delSyslogCrlCertificate(url: string) {
        return this.http.post(url, {});
    }
}
