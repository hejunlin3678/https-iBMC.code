import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class KvmService {

    constructor(
        private http: HttpService
    ) { }

    // 获取虚拟控制台配置信息
    public getKvm() {
        return this.http.get('/UI/Rest/Services/KVM');
    }

    // 获取vmm 加密使能
    public getVmm() {
        return this.http.get('/UI/Rest/Services/VMM');
    }

    // 设置虚拟控制台配置信息
    public setKvmService(params) {
        return this.http.patch('/UI/Rest/Services/KVM', params);
    }

}
