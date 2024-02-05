import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class TrapService {
    constructor(
        private http: HttpService
    ) {}
    // Trap端口
    public readonly trapPort = 162;
    // 获取trap信息
    getTrapInfo() {
        return this.http.get(`/UI/Rest/Maintenance/TrapNotification`);
    }
    trapSave(params) {
        return this.http.patch(
            `/UI/Rest/Maintenance/TrapNotification`,
            params
        );
    }
    trapSendTest(params) {
        return this.http.post(
            `/UI/Rest/Maintenance/TestTrapNotification`,
            params
        );
    }
    // 获取是否开启校验
    getPwdCode() {
        return this.http.get('/UI/Rest/AccessMgnt/AdvancedSecurity');
    }
}
