import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class HttpWebService {

    constructor(
        private http: HttpService,
    ) { }

    // 获取WEB服务基本配置
    public getWeb() {
        return this.http.get('/UI/Rest/Services/WEBService');
    }

    // 设置WEB服务基本配置
    public setWebService(params) {
        return this.http.patch('/UI/Rest/Services/WEBService', params);
    }

    // 生成并导出CSR文件
    public exportCSR(params) {
        return this.http.post('/UI/Rest/Services/WEBService/ExportCSR', params);
    }
    // 查询证书生成进度
    public task(url) {
        return this.http.get(url);
    }
}
