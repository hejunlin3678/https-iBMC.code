import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class UpgradeService {

    constructor(
        private http: HttpService,
        private userInfo: UserInfoService,
        private user: UserInfoService
    ) {

    }

    // 获取产品型号
    public queryTCEType() {
        return this.http.get('/redfish/v1/chassis/enclosure');
    }

    // 用户查询固件升级
    public getUpdateInfo() {
        return this.http.get('/redfish/v1/UpdateService');
    }

    // 固件升级
    public upgradeHmm(fileUrl) {
        const header = {
            headers: {
                'Token': this.userInfo.getToken(),
                From: 'WebUI',
                Accept: '*/*'
            }
        };
        return this.http.post('/redfish/v1/UpdateService/Actions/UpdateService.SimpleUpdate',
            {
                ImageURI: fileUrl
            },
            header
        );
    }

    // 进度查询
    public queryTaskProgress(tasksId) {
        return this.http.get('/redfish/v1/TaskService/Tasks/' + tasksId);
    }

    // 获取Oem下数据
    public getOemData(object) {
        let data = null;
        if (object && object.Oem) {
            data = Object.keys(object.Oem)[0];
            return object.Oem[data];
        }
        return null;
    }
}
