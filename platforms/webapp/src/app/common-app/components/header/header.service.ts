import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { LanguageService } from 'src/app/common-app/service/language.service';
import { Observable } from 'rxjs/internal/Observable';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';

@Injectable({
    providedIn: 'root'
})
export class HeaderService {

    constructor(
        private http: HttpService,
        private langService: LanguageService,
        private userInfo: UserInfoService
    ) { }

    // 退出登录
    public loginOut() {
        const resourceId = JSON.parse(localStorage.getItem('userInfo')).resourceId;
        // 添加 ignoreKeepAlive 参数，阻止该接口发送 keepAlive请求
        return this.http.delete(`/UI/Rest/Sessions/${resourceId}`, { ignoreKeepAlive: true });
    }

    public getGenericInfo(): Observable<any> {
        return new Observable((observe) => {
            this.http.get('/UI/Rest/GenericInfo', {
                ignoreKeepAlive: true
            }).subscribe({
                next: (res) => {
                    // 查询ibmc系统时间
                    const time: string = res.body.CurrentTime;
                    sessionStorage.setItem('DateTime', time);
                    let ibmcTime: string = '';
                    if (time) {
                        const timeSplitArr = time.split(/\s+/);
                        const datePart = timeSplitArr[0].replace(/-/g, '/');
                        const timePart = timeSplitArr[1].substr(0, 5);
                        const timeZone = timeSplitArr[2];

                        ibmcTime = `${datePart} ${timePart} ${timeZone}`;

                        // 处理法语
                        if (this.langService.locale === 'fr-FR') {
                            ibmcTime = ibmcTime.replace(/:/g, 'h');
                        }
                    }

                    // 查询当前告警信息
                    const healthInfo = res.body.HealthSummary;
                    const alarmInfo = {
                        critical: healthInfo.NumberOfCriticalAlarm,
                        major: healthInfo.NumberOfMajorAlarm,
                        minor: healthInfo.NumberOfMinorAlarm
                    };

                    // UID 信息
                    const uidState = res.body.IndicatorLEDState;

                    // 电源信息
                    const powerState = res.body.PowerState;

                    // tce电源功率按钮的显示、隐藏
                    const powerCappingSupport = res.body?.PowerCappingSupport;
                    const powerSleepSupport = res.body?.PowerDeepSleepSupported;
                    const swiSupport = res.body?.SwiSupported;
                    // 各种菜单的支持状态
                    const fdmSupport = res.body.FDMSupported;
                    const ibmaSupport = res.body.SmsSupported;
                    const licenseSupport = res.body.LicSerSupported;
                    const partitySupport = res.body.FusionPartSupported;
                    const usbSupport = res.body.USBSupported;
                    const fanSupport = res.body.FanSupported;
                    const snmpSupport = res.body.SnmpSupported;
                    const spSupport = res.body.SPSupported;
                    const tpcmSupport = res.body.TPCMSupported;
                    const platform = res.body.ArmSupported ? 'Arm' : 'x86';
                    const ibmaRunning = res.body.SmsSupported && res.body.iBMARunningStatus === 'Running';
                    // 表示是否开启了移动定制化开关，取值0：未开启 1：已开启
                    const customizedId = res.body.CustomizedId || 0;
                    // 是否支持VLAN修改,true表示不支持
                    const modifyVlanIdUnsupported = res.body?.ModifyVlanIdUnsupported;
                    const kvmSupport = res.body.KVMSupported;
                    const vncSupport = res.body.VNCSupported;
                    const vmmSupport = res.body.VMMSupported;
                    const perfSupSupport = res.body.PerfSupported;
                    const videoScreenshotSupport = res.body.VideoScreenshotSupported;
                    // 表示自定义证书标志，取值true：自定义证书 false：预置证书，默认为自定义证书，防止有提示弹出
                    const customeCertFlag = res.body.CustomeCertFlag !== undefined ? res.body.CustomeCertFlag : true;
                    const info = {
                        ibmcTime,
                        alarmInfo,
                        uidState,
                        powerState,
                        powerCappingSupport,
                        powerSleepSupport,
                        swiSupport,
                        fdmSupport,
                        ibmaSupport,
                        licenseSupport,
                        partitySupport,
                        usbSupport,
                        spSupport,
                        fanSupport,
                        tpcmSupport,
                        snmpSupport,
                        platform,
                        ibmaRunning,
                        customizedId,
						customeCertFlag,
						modifyVlanIdUnsupported,
                        kvmSupport,
                        vncSupport,
                        vmmSupport,
                        perfSupSupport,
                        videoScreenshotSupport
                    };
                    observe.next(info);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    // 查询系统锁定状态
    public getSystemLockState() {
        return new Observable((observe) => {
            this.http.get('/UI/Rest/AccessMgnt/AdvancedSecurity', { ignoreKeepAlive: true }).subscribe((res) => {
                const state = res['body'].SystemLockDownEnabled;
                observe.next(state);
            });
        });
    }
}
