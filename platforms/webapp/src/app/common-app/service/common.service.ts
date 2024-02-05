import { Injectable } from '@angular/core';
import { UserInfoService } from './user-info.service';
import { getBrowserType } from 'src/app/common-app/utils';
import { HttpClient } from '@angular/common/http';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { Router } from '@angular/router';
import { Subject, Observable } from 'rxjs';

@Injectable({
    providedIn: 'root'
})
export class CommonService {
    private msgSubject = new Subject();

    constructor(
        private httpService: HttpClient,
        private userInfo: UserInfoService,
        private http: HttpService,
        private router: Router
    ) { }

    // 退出登录时，删除信息, 包括服务和localstorage里面的
    clear() {
        // 服务信息删除
        this.userInfo.reset();
        localStorage.removeItem('userInfo');
        sessionStorage.clear();
        this.updateLocalLoginCount('delete');
    }

    public saveUserToStorage() {
        localStorage.setItem('userInfo', JSON.stringify(this.userInfo));
    }

    // 记录当前已经登录了的tab页
    public updateLocalLoginCount(type: 'add' | 'delete') {
        let tabs = JSON.parse(localStorage.getItem('tabs'));
        const tabIsValid = JSON.parse(sessionStorage.getItem('tabIsValid'));
        if (!tabIsValid) {
            if (type === 'add') {
                tabs++;
                sessionStorage.setItem('tabIsValid', JSON.stringify(true));
                localStorage.setItem('tabs', JSON.stringify(tabs));
            } else {
                // deletge表示是自然超时或者是手动点击退出会话按钮
                localStorage.setItem('tabs', JSON.stringify(0));
                sessionStorage.removeItem('tabIsValid');
            }

        }
    }

    // 跳转到登录规则页面
    public jumpToSec(): void {
        if (this.userInfo.hasPrivileges(['ConfigureUsers'])) {
            sessionStorage.setItem('securityTab', 'loginRule');
            this.router.navigate(['/navigate/user/security']);
        }
    }

    // 发送消息
    public sendMessage(type: any) {
        this.msgSubject.next(type);
    }

    // 获取信息，返回Observable
    public getMessage(): Observable<any> {
        return this.msgSubject.asObservable();
    }

    public downloadFileWithResponse(res, fileName: string) {
        const downloadUrl = window.URL.createObjectURL(res);
        const anchor = document.createElement('a');
        const body = document.getElementsByTagName('body')[0];
        body.appendChild(anchor);
        anchor.setAttribute('style', 'display:none');
        anchor.href = downloadUrl;
        anchor.download = fileName;
        anchor.click();
        window.URL.revokeObjectURL(downloadUrl);
    }

    public restDownloadFile(url: string) {
        return new Promise((resolve, reject) => {
            this.http.get(url, { responseType: 'blob' }).subscribe((response) => {
                const fileName = response.headers.get('content-disposition').split('=')[1];
                this.downloadFileWithResponse(response['body'], fileName);
                resolve(response);
            }, () => {
                reject();
            });
        });
    }

    public oneKeyDownloadFile(url: string, fileName: string) {
        return new Promise((resolve, reject) => {
            this.http.get(url, { responseType: 'blob' }).subscribe((response) => {
                this.downloadFileWithResponse(response['body'], fileName);
                resolve(response);
            }, () => {
                reject();
            });
        });
    }

    public transformSpecialChar(str) {
        return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }
    public loadXML(xmlString) {
        const titleReg = /<title>\s*([\s\S]*?)\s*<\/title>/;
        const titleSNReg = /<title>[\s\S]*?SN\s*:\s*(\S*)\s*[\s\S]*<\/title>/;
        const paramReg = /<param[\s\S]*?\/>/g;
        const obj = {
            title: '',
            SN: ''
        };
        if (titleReg.test(xmlString) && RegExp.$1) {
            obj.title = RegExp.$1;
        }
        if (titleSNReg.test(xmlString) && RegExp.$1) {
            obj.SN = this.transformSpecialChar(RegExp.$1);
        }
        const paramArr = xmlString.match(paramReg);
        if (paramArr && paramArr.length) {
            const paramNameReg = /[\s\S]*name\s*=\s*"(\S*)\s*"[\s\S]*\/>/;
            const paramValueReg = /[\s\S]*value\s*=\s*"([\S\s]*)"[\s\S]*\/>/;
            let key;
            let value;
            paramArr.forEach((param) => {
                if (paramNameReg.test(param)) {
                    key = RegExp.$1;
                }
                if (paramValueReg.test(param)) {
                    value = RegExp.$1;
                }
                if (key && !obj[key] && value) {
                    if (key === 'vmm_compress' || key === 'compress') {
                        value = parseInt(value, 10);
                    }
                    obj[key] = value;
                }
            });
        }
        return obj;
    }

    // 查询Rest接口的通用支持性判断
    public getGenericInfo() {
        return this.http.get('/UI/Rest/GenericInfo');
    }

    // 查询sysOverView接口数据
    public getSysOverView() {
        return this.http.get('/UI/Rest/Overview');
    }

    // 判断当前会话是否有效
    public keepAlive() {
        return this.http.post(`/UI/Rest/KeepAlive`, { 'Mode': 'Activate' });
    }
}
