import { Component, OnInit } from '@angular/core';
import { NtpAuthenService } from './ntp-authen.service';
import { NtpService } from '../ntp.service';
import { TranslateService } from '@ngx-translate/core';
import { AuthenData } from '../models/authen/authen-data.model';
import { Authen } from '../models/authen/authen.model';
import { TiUploadRef, TiUploadService, TiFileItem } from '@cloud/tiny3';
import { AlertMessageService, SystemLockService, ErrortipService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils/common';

@Component({
    selector: 'app-ntp-authen',
    templateUrl: './ntp-authen.component.html',
    styleUrls: ['./ntp-authen.component.scss']
})
export class NtpAuthenComponent implements OnInit {

    public authenData: AuthenData;
    public switchState: boolean = true;
    public uploader: TiUploadRef;
    public systemLocked: boolean;

    constructor(
        private ntpAuthenService: NtpAuthenService,
        private ntpService: NtpService,
        public i18n: TranslateService,
        private alert: AlertMessageService,
        public uploaderService: TiUploadService,
        private lockService: SystemLockService,
        private errorTip: ErrortipService,
    ) {
        this.authenData = new AuthenData();
        this.systemLocked = true;
    }

    ngOnInit(): void {
        this.init();
        // 系统锁定
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
    }

    public init() {
        this.ntpService.ntpLoad.authen = true;
        this.ntpService.loading$.next(this.ntpService.ntpLoad);
        this.ntpAuthenService.factory().subscribe(
            ({ authenData, authen }) => {
                this.authenData = authenData;
                this.switchState = this.authenData.getAuthenEnabled;
                this.ntpService.timeNTP.setAuthen = authen;
                this.uploader = this.uploaderService.create(authenData.getUploadConfig);
                this.ntpService.ntpLoad.authen = false;
                this.ntpService.loading$.next(this.ntpService.ntpLoad);
            }
        );
    }

    public switchChange() {
        this.authenData.setAuthEnabled = this.switchState;
        this.ntpService.timeNTP.getAuthen.setAuthenEnabled = this.switchState;
        this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
    }

    public save(ntpCopy: any, authen: Authen) {
        return this.ntpAuthenService.save(ntpCopy, authen);
    }

    public addItemFun(data: any): void {
        if (data.validResults.indexOf('type') >= 0) {
            this.alert.eventEmit.emit({ type: 'error', label: 'NTP_UPLOAD_TYPE' });
        } else if (data.validResults.indexOf('maxSize') >= 0) {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR3' });
        }
    }
    public addItemSuccess(data: TiFileItem): void {
        const fileItems = data['file'];
        this.ntpAuthenService.uploadNtpFile(fileItems.name, fileItems._file).subscribe(
            () => {
                this.ntpService.getData('/UI/Rest/BMCSettings/NTP').subscribe((res) => {
                    const status = Number(res.body.KeyStatus) === 1 ? true : false;
                    this.authenData.setKeyStatus = status;
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                }, () => {
                    this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                });
            },
            (error) => {
                const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            }
        );
    }
}
