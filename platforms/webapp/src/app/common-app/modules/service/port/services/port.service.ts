import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class PortService {

    constructor(
        private http: HttpService
        ) { }

    // 获取全部端口和使能
    public getNetworkPort() {
        return this.http.get('/UI/Rest/Services/PortConfig');
    }

    // 保存全部修改
    public setPortService(params) {
        return this.http.patch('/UI/Rest/Services/PortConfig', params);
    }

}
