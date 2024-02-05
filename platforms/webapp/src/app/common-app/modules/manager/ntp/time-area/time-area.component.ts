import { Component, OnInit } from '@angular/core';
import { TimeAreaService } from './time-area.service';
import { NtpService } from '../ntp.service';
import { LanguageService, SystemLockService } from 'src/app/common-app/service';
import { TimeAreaData, TimeArea, IZreo } from '../models';

@Component({
    selector: 'app-time-area',
    templateUrl: './time-area.component.html',
    styleUrls: ['./time-area.component.scss']
})
export class TimeAreaComponent implements OnInit {

    public timeAreaData: TimeAreaData;
    public timeArea: TimeArea;

    public areaSelect: IZreo;
    public timeSelect: IZreo;
    public systemLocked: boolean;
    public dstEnabled: boolean;

    private locale: string;

    constructor(
        private timeAreaService: TimeAreaService,
        private ntpService: NtpService,
        private langService: LanguageService,
        private lockService: SystemLockService
    ) {
        this.timeAreaData = new TimeAreaData();
        this.timeArea = new TimeArea();
        this.locale = this.langService.getTargetLanguage();
        this.timeAreaService.locale = this.locale;
        this.dstEnabled = true;
        this.systemLocked = true;
    }

    ngOnInit() {
        this.init();
        // 系统锁定
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
    }

    public init() {
        this.ntpService.ntpLoad.timeArea = true;
        this.ntpService.loading$.next(this.ntpService.ntpLoad);
        this.timeAreaService.factory().subscribe((results) => {
            this.timeAreaData = results.timeAreaData;
            this.timeArea = results.timeArea;
            this.areaSelect = this.timeAreaData.getAreaSelect;
            this.timeSelect = this.timeAreaData.getTimeSelect;
            this.dstEnabled = this.timeAreaData.getDstEnabled;
            this.ntpService.ntpLoad.timeArea = false;
            this.ntpService.loading$.next(this.ntpService.ntpLoad);
        });
    }

    public areaChange(area: IZreo) {
        this.ntpService.timeNTP.getTimeArea.setArea = area.key;
        this.timeAreaService.areaChange(area.key, this.locale, this.timeAreaData.getTimeOrigin).subscribe((data) => {
            this.timeAreaData.setParamList = data.paramList;
            this.timeAreaData.setTimeList = data.timeList;
            this.timeAreaData.setAreaSelect = this.areaSelect;
            this.timeSelect = data.timeSelect;
            this.timeChange(data.timeSelect);
        });
    }

    public timeChange(time: IZreo) {
        this.ntpService.timeNTP.getTimeArea.setTime = time.key;
        this.timeAreaData.setTimeSelect = this.timeSelect;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    public dstChange(): void {
        this.ntpService.timeNTP.setDstEnabled = this.dstEnabled;
        // 前端回显赋值
        this.timeAreaData.setDstEnabled = this.dstEnabled;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    public saveTimeArea(ntpCopy: any, ntpTimeArea: TimeArea): object {
        return this.timeAreaService.saveTimeArea(ntpCopy, ntpTimeArea, this.timeAreaData.getParamList);
    }

    public saveDstEnabled(dstEnabledCopy: boolean, dstEnabledFinal: boolean): object {
        return this.timeAreaService.saveDstEnabled(dstEnabledCopy, dstEnabledFinal);
    }
}
