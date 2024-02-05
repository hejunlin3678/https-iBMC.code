import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class USBService {

    constructor(
        private http: HttpService
        ) { }

    // 获取全部端口和使能
    public getUSB() {
        return this.http.get('/UI/Rest/BMCSettings/USBMgmtService');
    }

    // 保存全部修改
    public setUSB(params) {
        return this.http.patch('/UI/Rest/BMCSettings/USBMgmtService', params);
    }

}
