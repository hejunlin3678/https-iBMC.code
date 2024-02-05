import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class SnmpService {

    constructor(private http: HttpService) { }

    // 获取SNMP配置信息
    public getSnmp() {
        return this.http.get('/UI/Rest/Services/SNMP');
    }
    // 设置SNMP配置信息
    public setSnmpService(params) {
        return this.http.patch('/UI/Rest/Services/SNMP', params);
    }
}
