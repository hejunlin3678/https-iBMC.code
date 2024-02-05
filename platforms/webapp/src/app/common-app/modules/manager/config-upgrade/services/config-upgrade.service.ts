import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { HttpService, UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})

export class ConfigUpgradeService {
    constructor(
        private http: HttpService,
        private user: UserInfoService,
        private httpClient: HttpClient
    ) {

    }

    // 导入
    public importConfig(params) {
        return this.http.post('/UI/Rest/BMCSettings/Configuration/ImportConfig', params);
    }

    // 上传文件
    public uploadFile(formData: FormData): Observable<any> {
        return new Observable((observer) => {
            const url = `/UI/Rest/BMCSettings/Configuration/ImportConfig`;
            const header = {
                headers: {
                    'X-CSRF-Token': this.user.getToken(),
                    From: 'WebUI',
                    Accept: '*/*'
                }
            };
            this.httpClient.post(url, formData, header).subscribe({
                next: () => {
                    observer.next(true);
                },
                error: (error) => {
                    observer.error(error);
                }
            });
        });
    }

    // 任务
    public task() {
        return this.http.get('/UI/Rest/BMCSettings/Configuration/ImportExportProgress');
    }

    // 查询task
    public getTask(url) {
        return this.http.get(url);
    }

    // 导出配置
    public configExport() {
        return this.http.post(`/UI/Rest/BMCSettings/Configuration/ExportConfig`, {});
    }
}
