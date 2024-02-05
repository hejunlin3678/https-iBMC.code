import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
@Injectable({
    providedIn: 'root'
})
export class SystemLogService {

    constructor(private http: HttpService) {
    }
    // 获取状态
    public switchState() {
        return this.http.get('/UI/Rest/Maintenance/SystemDiagnostic');
    }
    // 更新状态
    public updateSwitchState(params) {
        return this.http.patch('/UI/Rest/Maintenance/SystemDiagnostic', params);
    }
    // 配置查询下载日志
    public downloadLog(exportUrl: string) {
        return this.http.post(exportUrl, {});
    }
    public downloadNpuLog(exportUrl: string, npuId: number): Observable<any> {
        return this.http.post(exportUrl, {
            NpuId: npuId
        });
    }
    // 获取下载进度
    public getTaskStatus(url: string) {
        return this.http.get(url);
    }
    // 获取处理器集合
    public getAllProcessor(): Observable<any> {
        return this.http.get('/UI/Rest/System/Processor');
    }
}
