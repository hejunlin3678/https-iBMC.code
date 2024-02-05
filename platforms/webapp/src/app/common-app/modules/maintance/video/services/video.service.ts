import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
@Injectable({
    providedIn: 'root'
})
export class VideoService {

    constructor(
        private http: HttpService,
        private user: UserInfoService,
        private translate: TranslateService
    ) { }

    // 查询
    public videoScreenCapture() {
        return this.http.get('/UI/Rest/Maintenance/SystemDiagnostic');
    }

    // 录像功能使能 最后一屏使能
    public setOnOffVal(params) {
        return this.http.patch('/UI/Rest/Maintenance/SystemDiagnostic', params);
    }

    // 进度查询
    public queryTask(url) {
        return this.http.get(url);
    }

    // 手动截屏
    public capturescreenshot(params) {
        return this.http.post('/UI/Rest/Maintenance/SystemDiagnostic/CaptureScreenshot', params);
    }

    // 删除截屏
    public deletescreenshot(params) {
        return this.http.delete('/UI/Rest/Maintenance/SystemDiagnostic/DeleteScreenshot', params);
    }

    // 停止播放
    public stopvideoplayback(params) {
        return this.http.post('/UI/Rest/Maintenance/SystemDiagnostic/StopVideo', params);
    }

    // 导出录像播放启动文件
    public exportJavaVideoStartupFile(params) {
        const url = '/UI/Rest/Maintenance/SystemDiagnostic/ExportVideoStartupFile';
        const httpOptions = {};
        httpOptions['responseType'] = 'blob';
        return this.http.post(url, params, httpOptions);
    }

    // 导出录像播放启动文件
    public exportJavaVideoStartupFail(params) {
        const url = '/UI/Rest/Maintenance/SystemDiagnostic/ExportVideoStartupFile';
        return this.http.post(url, params);
    }

    // 导出录像播放启动文件
    public exportHtml5VideoStartupFile(params) {
        const httpOptions = {};
        httpOptions['responseType'] = 'text';
        return this.http.post('/UI/Rest/Maintenance/SystemDiagnostic/ExportVideoStartupFile', params, httpOptions);
    }

    // 导出录像 下载  /UI/Rest/Maintenance/SystemDiagnostic/DownloadVideo
    public exportvideo(params) {
        return this.http.post('/UI/Rest/Maintenance/SystemDiagnostic/DownloadVideo', params);
    }

}
