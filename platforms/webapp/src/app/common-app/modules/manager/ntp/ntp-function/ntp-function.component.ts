import { Component, OnInit } from '@angular/core';
import { NtpFunctionService } from './ntp-function.service';
import { NtpService } from '../ntp.service';
import { NTPFunction, NTPFunctionData, IRadio } from '../models';
import { TranslateService } from '@ngx-translate/core';
import { CommonService, GlobalDataService, LanguageService, PRODUCT, SystemLockService } from 'src/app/common-app/service';
import { TiDatetimeFormat, TiMessageService } from '@cloud/tiny3';
import { CommonData } from 'src/app/common-app/models';
import Date from 'src/app/common-app/utils/date';

@Component({
    selector: 'app-ntp-function',
    templateUrl: './ntp-function.component.html',
    styleUrls: ['./ntp-function.component.scss']
})
export class NTPFunctionComponent implements OnInit {

    public switchState: boolean = true;
    public radioList: IRadio[];
    public raidoSelect: string;
    public ntpData: NTPFunctionData;
    public systemLocked: boolean;
    public ishowTime: boolean = CommonData.productType === PRODUCT.EMM;
    public datetimeValue: Date;
    public format: TiDatetimeFormat = {
        date: 'yyyy/MM/dd',
        time: 'HH:mm:ss'
    };
    public createTimer: any;
    public isTimeChangeFlag: boolean = false;
    public transitionTime: Date;
    public timeChanged: boolean = false;

    constructor(
        private ntpFunctionService: NtpFunctionService,
        private ntpService: NtpService,
        public i18n: TranslateService,
        private lockService: SystemLockService,
        private commonService: CommonService,
        private global: GlobalDataService,
        private tiMessage: TiMessageService,
        private langService: LanguageService,
    ) {
        this.ntpData = new NTPFunctionData();
        this.systemLocked = true;
    }

    ngOnInit() {
        this.commonService.getGenericInfo().subscribe({
            next: (res) => {
                const datetime = res.body.CurrentTime;
                let newDate = datetime.replace('T', ' ');
                newDate = newDate.replace(/-/g, '/');
                const transitionValue = new Date(newDate);
                const nowMs = transitionValue.getTime();
                this.datetimeValue = new Date(nowMs);
                this.setDeviceTime();
            },
        });
        this.init();
        // 系统锁定
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
            this.ntpData.setSystemLocked = res;
        });
    }
    // 时间值递增函数
    public setDeviceTime(isShowTime: boolean = true) {
        clearTimeout(this.createTimer);
        // 递增
        if (isShowTime) {
            this.datetimeValue = new Date(this.datetimeValue.getTime() + 1000);
        } else {
            this.transitionTime = new Date(this.transitionTime.getTime() + 1000);
        }
        this.createTimer = setTimeout(() => {
            this.setDeviceTime(isShowTime);
        }, 1000);
    }
    public onBlur(event: Event): void {
        if (this.isTimeChangeFlag) {
            return;
        }
        this.datetimeValue = this.transitionTime;
        this.setDeviceTime();
    }
    public onNgModelChange(model: Date): void {
        this.datetimeValue = new Date(model.getTime());
    }
    public onFocus(event: Event): void {
        this.isTimeChangeFlag = false;
        this.transitionTime = this.datetimeValue;
        clearTimeout(this.createTimer);
        this.setDeviceTime(false);
    }
    public okClick() {
        this.isTimeChangeFlag = true;
        this.timeChanged = true;
        this.setDeviceTime();
        this.ntpService.timeNTP.getNTPFunction.setTimeChenge = this.timeChanged;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    // 同步时间
    public syncIntelTime() {
        if (this.switchState) {
            return;
        }
        this.tiMessage.open({
            type: 'prompt',
            id: 'syncTimeMessage',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('COMMON_ASK_OK'),
            close: () => {
                this.datetimeValue = new Date();
                this.timeChanged = true;
                this.setDeviceTime();
                this.ntpService.timeNTP.getNTPFunction.setTimeChenge = this.timeChanged;
                this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
            },
            dismiss(): void { }
        });
    }

    public ntpServeChange() {
        const valArr = this.ntpFunctionService.setVal(this.ntpData);
        this.ntpService.timeNTP.getNTPFunction.setPreferredNtpServer = valArr.preferredNtpServer;
        this.ntpService.timeNTP.getNTPFunction.setAlternateNtpServer = valArr.alternateNtpServer;
        this.ntpService.timeNTP.getNTPFunction.setExtraNtpServer = valArr.extraNtpServer;
        this.ntpService.verifica.servesPref = this.ntpData.getNTPServes ? this.ntpData.getNTPServes.valid : true;
        this.ntpService.verifica.servesAlte = this.ntpData.getNTPServesAlter ? this.ntpData.getNTPServesAlter.valid : true;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    public switchChange() {
        this.ntpData.setNTPSwitch = this.switchState;
        this.ntpService.timeNTP.getNTPFunction.setServiceEnabled = this.switchState;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    public radioChange(radio: IRadio) {
        this.ntpData.setRadioSelect = radio;
        this.ntpService.timeNTP.getNTPFunction.setNtpAddressOrigin = radio.id;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    // 给父组件调用的更新页脚时间的方法
    public changeFootTime() {
        this.commonService.getGenericInfo().subscribe({
            next: (res) => {
                // 查询ibmc系统时间
                const time: string = res.body.CurrentTime;
                if (time) {
                    let ibmcTime: string = '';
                    const timeSplitArr = time.split(/\s+/);
                    const datePart = timeSplitArr[0].replace(/-/g, '/');
                    const timePart = timeSplitArr[1].substr(0, 5);
                    const timeZone = timeSplitArr[2];
                    ibmcTime = `${datePart} ${timePart} ${timeZone}`;
                    // 处理法语
                    if (this.langService.locale === 'fr-FR') {
                        ibmcTime = ibmcTime.replace(/:/g, 'h');
                    }
                    this.global.ibmcTime.next({ ibmcTime });
                }
            },
        });
    }

    public save(ntpCopy: any, ntpFunction: NTPFunction): object {
        if (ntpFunction.getTimeChenge) {
            ntpFunction.setDateTime = new Date(this.datetimeValue).pattern('yyyy-MM-dd HH:mm:ss');
        }
        return this.ntpFunctionService.save(ntpCopy, ntpFunction);
    }

    public init() {
        this.ntpService.ntpLoad.ntpFunction = true;
        this.ntpService.loading$.next(this.ntpService.ntpLoad);
        this.ntpFunctionService.factory().subscribe(
            (data) => {
                this.ntpData = data.NTPFunctionData;
                this.switchState = this.ntpData.getNTPSwitch;
                this.ntpService.timeNTP.setNtpFunction = data.NTPFunction;
                this.radioList = this.ntpData.getRadioList;
                this.raidoSelect = this.ntpData.getRadioSelect.id;
                this.ntpService.ntpLoad.ntpFunction = false;
                this.ntpService.loading$.next(this.ntpService.ntpLoad);
            }
        );

    }

    // 该方法得作用在于解决自动化脚本再重置输入框得值为空值时，不能触发代码得change事件，同时 html的事件需要修改为ngModelChange
    public dipatchEvent(param: { value: string, name: string }): void {
        this.ntpData.getNTPServes.controls[param.name].patchValue(param.value);
    }
}
