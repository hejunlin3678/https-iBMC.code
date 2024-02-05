import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { UserInfoService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
/**
 * 如果采用 `providedIn` ，
 * 这个形式是 Angular v6 之后引入的
 * 这种写法和传统的在 Module 中设置 providers 数组的写法的区别在于
 * 可以让服务在真正被其它组件或服务注入时才编译到最后的 js 中
 * 对于引入第三方类库较多的应用可以有效减小 js 大小
 */
@Injectable({
    providedIn: 'root'
})
export class AlarmEventService {

    constructor(private http: HttpService, private user: UserInfoService) {
    }
    // 清空日志
    clearData() {
        return this.http.post('/UI/Rest/Maintenance/ClearEvent', {});
    }
    // 配置查询下载日志
    downloadLog() {
        return this.http.post('/UI/Rest/Maintenance/DownloadEvent', {});
    }
    // 获取下载进度
    getTaskStatus(url: string) {
        return this.http.get(url);
    }
    // 当前日志ok
    logServices() {
        return this.http.get(`/UI/Rest/Maintenance/Alarm`, {});
    }
    // 系统事件
    sysLogServices(data: object) {
        return this.http.post('/UI/Rest/Maintenance/QueryEvent', data);
    }
    // 查询系统事件主体类型
    getSubjectType() {
        return this.http.get('/UI/Rest/Maintenance/EventObjectType').pipe(
            map(res => {
                const body = res['body'];
                return body.List;
            })
        );
    }
}
