import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs/internal/Observable';
import { IUpdataFileData, ICustError } from './dataType/data.interface';
import { map } from 'rxjs/internal/operators/map';

@Injectable({
    providedIn: 'root'
})

export class SPUpgradeServices {
    constructor(
        private http: HttpService,
        private httpClient: HttpClient,
        private userInfo: UserInfoService
    ) { }

    // 查询升级文件
    public getSPUpdateFileList() {
        return this.http.get('/UI/Rest/BMCSettings/UpdateService/SPUpdateFileList').pipe(
            map(res => {
                const updateFileList: IUpdataFileData[] = res['body'].UpdateFileList;
                return updateFileList;
            })
        );
    }
    // 删除文件列表
    public delSPUpdateFileList() {
        return this.http.post('/UI/Rest/BMCSettings/UpdateService/DeleteSPUpdateFile', {});
    }
    // 上传文件
    public uploadFile(param: { formData: FormData; }): Observable<ICustError> {
        return new Observable((observer) => {
            const url = `/UI/Rest/BMCSettings/UpdateService/SPFWUpdate`;
            let header = {};
            header = {
                headers: {
                    'X-CSRF-Token': this.userInfo.getToken(),
                    From: 'WebUI',
                    Accept: '*/*'
                }
            };
            this.httpClient.post(url, param.formData, header).subscribe({
                next: () => {
                    observer.next({
                        type: 'success',
                        errors: null
                    });
                },
                error: (error) => {
                    observer.error(error);
                }
            });
        });
    }
}
