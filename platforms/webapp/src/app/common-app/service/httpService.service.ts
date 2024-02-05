import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable, } from 'rxjs';
import { tap } from 'rxjs/operators';
import { UserInfoService } from './user-info.service';

// 服务导入
import { TimeoutService } from 'src/app/common-app/service/timeout.service';
import { ErrortipService } from 'src/app/common-app/service/errortip.service';
import { LoadingService } from './loading.service';

import * as utils from 'src/app/common-app/utils';
import { GlobalDataService } from './global-data.service';
import { Router } from '@angular/router';

@Injectable()

export class HttpService {
    constructor(
        private httpClient: HttpClient,
        private user: UserInfoService,
        private globalData: GlobalDataService,
        private timeout: TimeoutService,
        private errorService: ErrortipService,
        private loading: LoadingService,
        private router: Router,
    ) {

    }
    private options: object = {};

    get(url: string, options?: object): Observable<any> {
        this.keepAlive('Activate', options);
        url = this.urlReductive(url);
        const headers = this.requestHeaderHandle(options, 'get');
        return this.httpClient.get(url, headers).pipe(
            tap(() => { this.globalData.reset(); }, (error) => { this.handleError(error, 'get', options); })
        );
    }

    post(url: string, data: object, options: object = {}, notSendActive?: boolean): Observable<any> {
        if (!notSendActive) {
            this.keepAlive('Activate', options);
        }

        this.options = options;
        url = this.urlReductive(url);
        options = this.requestHeaderHandle(options, 'post');
        return this.httpClient.post(url, data, options).pipe(
            tap(() => { this.globalData.reset(); }, (error) => { this.handleError(error, 'post', options); })
        );
    }

    patch(url: string, data: object, options?: object): Observable<any> {
        this.keepAlive('Activate', options);
        options = options || {};
        url = this.urlReductive(url);
        const headers = this.requestHeaderHandle(options, 'patch');
        return this.httpClient.patch(url, data, headers).pipe(
            tap(() => { this.globalData.reset(); }, (error) => { this.handleError(error, 'patch', options); })
        );
    }

    put(url: string, data: object, options?: object): Observable<any> {
        this.keepAlive('Activate', options);
        options = options || {};
        url = this.urlReductive(url);
        const headers = this.requestHeaderHandle(options, 'put');
        return this.httpClient.put(url, data, headers).pipe(
            tap(() => { this.globalData.reset(); }, (error) => { this.handleError(error, 'put', options); })
        );
    }

    delete(url: string, options?: object, data?: object): Observable<any> {
        this.keepAlive('Activate', options);
        options = options || {};
        url = this.urlReductive(url);
        const header = this.requestHeaderHandle(options, 'delete');
        if (data) {
            header['body'] = data;
        }
        return this.httpClient.delete(url, header).pipe(
            tap(() => { this.globalData.reset(); }, (error) => { this.handleError(error, 'delete', options); })
        );
    }

    public keepAlive(type: 'Activate' | 'Deactivate', options?) {
        const localUserInfo = JSON.parse(localStorage.getItem('userInfo'));
        const resourceId = this.user['resourceId'] || (localUserInfo && localUserInfo.resourceId);
        if (!resourceId) {
            return;
        }
        const url = `/UI/Rest/KeepAlive`;
        if (type === 'Activate') {
            if (options && options.ignoreKeepAlive) {
                return;
            }

            if (this.router.url.indexOf('login') > -1) {
                return;
            }

            const dateNow = Date.now();
            if (resourceId && dateNow - this.globalData.keepAliveInterval > 10000) {
                this.globalData.keepAliveInterval = dateNow;
                this.post(url, { 'Mode': type }).subscribe();
            }
        } else {
            // 如果已经发生了401错误了，则不发送该请求
            if (this.globalData.sessionTimeOut) {
                return;
            }
            this.post(url, { 'Mode': type }, { 'ignoredErrorTip': true }, true).subscribe();
        }
    }

    // 请求头头处理（新增，修改）
    private requestHeaderHandle(options, method?: string): object {
        // 请求头添加 observe: 'response' 是为了能在请求成功后，通过res.headers.get()来获取响应头
        let headers: HttpHeaders = new HttpHeaders({ 'Content-Type': 'application/json', 'From': 'WebUI', 'Accept': '*/*' });
        if (this.user.token !== '' && method !== 'get') {
            if (options.type === 'redfish') {
                headers = headers.append('Token', this.user.token || '');
            } else {
                headers = headers.append('X-CSRF-Token', this.user.token || '');
            }
        }
        const httpOptions = {
            ...options,
            headers,
            observe: 'response'
        };
        // 循环添加传入的自定义请求头
        if (options && options.headers && Object.prototype.toString.call(options.headers) === '[object Object]') {
            Object.keys(options.headers).forEach((key) => {

                if (httpOptions.headers.has(key)) {
                    httpOptions.headers = httpOptions.headers.set(key, options.headers[key]);
                } else {
                    httpOptions.headers = httpOptions.headers.append(key, options.headers[key]);
                }
            });
        }
        return httpOptions;
    }

    // URL地址还原，将{managerid},{systemid},{chassisid}替换成正确的ID
    private urlReductive(url: string): string {
        let _url: string = url;
        const subRegRex: RegExp = /(managersId)|(systemsId)|(chassisId)/;
        _url = _url.
            replace(subRegRex, (match: string) => {
                return this.user[match];
            })
            .replace(/\{|\}/g, '');
        return _url;
    }

    private extractData(res: Response): object {
        return res || {};
    }

    // 这里采用箭头函数的写法，如果使用函数定义的方式，则在函数体内 this 会指向到错误对象，从而无法调用该服务内的方法
    private handleError = (error: HttpErrorResponse, method: string, options: object) => {
        this.loading.state.next(false);
        // 如果超过3次错误，则弹出超时框
        if (!error.ok && error.status === 0) {
            this.globalData.failedCount++;
        }

        if (this.globalData.failedCount >= 3) {
            this.timeout.error401.next('SessionTimeout');
            return;
        }

        // 排除某些错误ID
        const outRangeErrorIdArr: string[] = [
            'ReauthFailed'
        ];

        if (typeof error === 'object' && error.error) {
            const errorArr = utils.getMessageId(error.error);
            const errorId = errorArr && errorArr.length > 0 ? errorArr[0].errorId : null;

            /**
             * 如果是二次认证导致的401错误，则不处理
             * 正常401错误，触发超时提醒。
             * 其他类型的错误，使用错误处理来提示报错信息
             * 如果接口中带有ignoredErrorTip，则不使用错误处理服务来报错。
             */
            if (error.status === 401 && errorId === outRangeErrorIdArr[0]) {
                return;
            }

            if (error.status === 401) {
                // 为了防止频繁触发401错误导致不可预估的问题，加入了是否已经触发401错误的判断
                if (!this.globalData.sessionTimeOut) {
                    this.timeout.error401.next(errorId);
                }
                return;
            }

            if (options && !options['ignoredErrorTip'] && method !== 'get') {
                this.errorService.errorTip(error.error);
            }
        }
    }
}
