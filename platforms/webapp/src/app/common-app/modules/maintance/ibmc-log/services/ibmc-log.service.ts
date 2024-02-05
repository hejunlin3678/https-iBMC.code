import { Injectable } from '@angular/core';
import { UserInfoService, HttpService } from 'src/app/common-app/service';
@Injectable({
    providedIn: 'root'
})
export class IbmcLogService {

    constructor(private http: HttpService, private user: UserInfoService) {
    }
    // 下载日志
    downloadLog(exportUrl: string) {
        return this.http.post(exportUrl, {});
    }
    // 获取下载进度
    getTaskStatus(url: string) {
        return this.http.get(url);
    }
    // 当前日志
    getLog(type, params) {
        let url = '';
        if (type === 'operate') {
            url = `/UI/Rest/Maintenance/OperationLog?Skip=${params.Skip}&Top=${params.Top}`;
        } else if (type === 'running') {
            url = `/UI/Rest/Maintenance/RunLog?Skip=${params.Skip}&Top=${params.Top}`;
        } else if (type === 'security') {
            url = `/UI/Rest/Maintenance/SecurityLog?Skip=${params.Skip}&Top=${params.Top}`;
        }
        return this.http.get(url, params);
    }
}
