import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { map, share } from 'rxjs/operators';
import { Observable } from 'rxjs/internal/Observable';
import { TimeNTP } from './models/common/time-ntp.model';
import { BehaviorSubject } from 'rxjs/internal/BehaviorSubject';
import { INTPLoad, IVerifica, IComponents } from './models/common/ntp.interface';
import { of } from 'rxjs/internal/observable/of';
import { Authen } from './models/authen/authen.model';

@Injectable({
    providedIn: 'root'
})
export class NtpService {

    // 用来存放接口返回值
    public data: Observable<any>;
    public timeNTP: TimeNTP;
    public ntpLoad: INTPLoad;
    public verifica: IVerifica;
    public authen: Authen;
    public loading$: BehaviorSubject<INTPLoad>;
    public buttonState$: BehaviorSubject<[TimeNTP, IVerifica]>;

    constructor(
        private http: HttpService,
    ) {
        this.ntpLoad = {
            timeArea: false,
            ntpFunction: false,
            polling: false,
            authen: false,
        };
        this.verifica = {
            servesPref: true,
            servesAlte: true,
            polling: true
        };
        this.loading$ = new BehaviorSubject<INTPLoad>(this.ntpLoad);
        this.data = this.init().pipe(share());
    }

    public init() {
        const url = '/UI/Rest/BMCSettings/NTP';
        return this.getData(url).pipe(
            map((results) => {
                return results.body;
            })
        );
    }

    /**
     * 保存下发
     */
    public ntpSaveButton(ntpCopy: any, ntpOrigin: TimeNTP, components: IComponents) {

        const url = '/UI/Rest/BMCSettings/NTP';
        let param = {};
        let post$  = null;

        if (JSON.stringify(ntpCopy.timeArea) !== JSON.stringify(ntpOrigin.getTimeArea)) {
            param = Object.assign(param, components.timeAreaComponent.saveTimeArea(ntpCopy.timeArea, ntpOrigin.getTimeArea));
        }

        if (JSON.stringify(ntpCopy.ntpFunction) !== JSON.stringify(ntpOrigin.getNTPFunction)) {
            param = Object.assign(param, components.ntpFunctionComponent.save(ntpCopy.ntpFunction, ntpOrigin.getNTPFunction));
        }

        if (JSON.stringify(ntpCopy.polling) !== JSON.stringify(ntpOrigin.getPolling)) {
            param = Object.assign(param, components.pollingComponent.save(ntpCopy.polling, ntpOrigin.getPolling));
        }

        if (JSON.stringify(ntpCopy.authen) !== JSON.stringify(ntpOrigin.getAuthen)) {
            param = Object.assign(param, components.ntpAuthenComponent.save(ntpCopy.authen, ntpOrigin.getAuthen));
        }

        // 判断dstEnabled/DST使能是否有变化
        if (ntpCopy.dstEnabled !== ntpOrigin.getDstEnabled) {
            param = Object.assign(param, components.timeAreaComponent.saveDstEnabled(ntpCopy.dstEnabled, ntpOrigin.getDstEnabled));
        }

        if (param && JSON.stringify(param) !== '{}') {
            post$ = this.patchData(url, param);
        } else {
            return of('').pipe(
                map(() => {
                    return 'failed';
                })
            );
        }

        return post$.pipe(
            map((result: any) => {
                const err = result.state !== 'success';
                if (err) {
                    return 'failed';
                } else {
                    return 'success';
                }
            })
        );

    }

    /**
     * 发送请求
     * @param url 请求链接
     */
    public getData(url: string) {
        return this.http.get(url);
    }

    /**
     * 下发请求
     * @param url 下发URL
     * @param data 下发数据
     */
    private patchData(url: string, data: object) {
        return this.http.patch(url, data).pipe(
            map(() => {
                return { state: 'success', label: 'success' };
            })
        );
    }
}
