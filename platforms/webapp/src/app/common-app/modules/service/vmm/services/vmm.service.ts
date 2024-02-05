import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class VmmService {

    constructor(
        private http: HttpService
    ) { }

    // 获取虚拟媒体配置信息
    public getVmm() {
        return this.http.get('/UI/Rest/Services/VMM');
    }

    // 获取kvm 加密使能
    public getKvm() {
        return this.http.get('/UI/Rest/Services/KVM');
    }

    // 设置虚拟媒体配置信
    public setVmmService(params) {
        return this.http.patch('/UI/Rest/Services/VMM', params);
    }

    // 断开活跃会话
    public getVmmActiveSession() {
        const queryParams = {
            VmmControlType: 'Disconnect'
        };
        return this.http.post('/UI/Rest/Services/VMM/Control', queryParams);
    }
    // 活跃会话进度查询
    public queryTaskActiveSession(url) {
        return this.http.get(url);
    }

}
