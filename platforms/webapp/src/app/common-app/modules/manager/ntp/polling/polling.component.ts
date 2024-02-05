import { Component, OnInit } from '@angular/core';
import { NtpService } from '../ntp.service';
import { PollingService } from './polling.service';
import { Polling, PollingData, IPolling } from '../models';
import { TranslateService } from '@ngx-translate/core';
import { SystemLockService } from 'src/app/common-app/service';

@Component({
    selector: 'app-polling',
    templateUrl: './polling.component.html',
    styleUrls: ['./polling.component.scss']
})
export class PollingComponent implements OnInit {

    public pollingData: PollingData;
    public minSelect: IPolling;
    public maxSelect: IPolling;
    public showMinTip: boolean;
    public showMaxTip: boolean;
    public systemLocked: boolean;

    constructor(
        private pollingService: PollingService,
        private ntpService: NtpService,
        private i18n: TranslateService,
        private lockService: SystemLockService
    ) {
        this.pollingData = new PollingData();
        this.systemLocked = true;
        this.showMinTip = false;
        this.showMaxTip = false;
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
        this.ntpService.ntpLoad.polling = true;
        this.ntpService.loading$.next(this.ntpService.ntpLoad);
        this.pollingService.factory(this.ntpService).subscribe(
            (data) => {
                this.pollingData = data.PollingData;
                this.ntpService.timeNTP.setPolling = data.Polling;
                this.minSelect = this.pollingData.getMinSelect;
                this.maxSelect = this.pollingData.getMaxSelect;
                this.ntpService.ntpLoad.polling = false;
                this.ntpService.loading$.next(this.ntpService.ntpLoad);
            }
        );
    }

    public minChange(polling: IPolling) {
        const resultVal = this.pollingData.setMinSelect(polling);
        if (resultVal.state) {
            this.showTip(false, false);
            this.ntpService.verifica.polling = true;
        } else {
            this.showTip(true, false);
            this.ntpService.verifica.polling = false;
        }
        this.ntpService.timeNTP.getPolling.setMinPollingInterval = polling.id;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    public maxChange(polling: IPolling) {
        const resultVal = this.pollingData.setMaxSelect(polling);
        if (resultVal.state) {
            this.showTip(false, false);
            this.ntpService.verifica.polling = true;
        } else {
            this.showTip(false, true);
            this.ntpService.verifica.polling = false;
        }
        this.ntpService.timeNTP.getPolling.setMaxPollingInterval = polling.id;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    private showTip(showMinTip: boolean, showMaxTip: boolean): void {
        this.showMinTip = showMinTip;
        this.showMaxTip = showMaxTip;
    }

    public save(pollingCopy: any, polling: Polling): object {
        return this.pollingService.save(pollingCopy, polling);
    }
}
