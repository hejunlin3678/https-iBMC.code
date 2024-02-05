import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class SpService {

    constructor(
        private http: HttpService
        ) { }

    // 获取全部端口和使能
    public getSp() {
        return this.http.get('/UI/Rest/BMCSettings/SP');
    }

    // 保存全部修改
    public setSp(params) {
        return this.http.patch('/UI/Rest/BMCSettings/SP', params);
    }

}
