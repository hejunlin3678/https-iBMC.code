import { Component, OnInit, ViewChild } from '@angular/core';
import { NtpService } from './ntp.service';
import { INTPLoad, TimeNTP, IComponents, IVerifica, NTPStaticModel } from './models';
import { BehaviorSubject } from 'rxjs/internal/BehaviorSubject';
import { TimeAreaComponent } from './time-area/time-area.component';
import { NTPFunctionComponent } from './ntp-function/ntp-function.component';
import { PollingComponent } from './polling/polling.component';
import { NtpAuthenComponent } from './ntp-authen/ntp-authen.component';
import { LoadingService, AlertMessageService, UserInfoService, SystemLockService } from 'src/app/common-app/service';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-ntp',
    templateUrl: './ntp.component.html',
    styleUrls: ['./ntp.component.scss']
})
export class NTPComponent implements OnInit {

    public ntpDisabled: boolean;
    private timeNTP: TimeNTP;
    private timeNTPCopy: string;
    public privilege: boolean;
    public systemLocked: boolean = true;

    private components: IComponents;

    @ViewChild(TimeAreaComponent, { static: false })
    private timeAreaComponent: TimeAreaComponent;

    @ViewChild(NTPFunctionComponent, { static: false })
    private ntpFunctionComponent: NTPFunctionComponent;

    @ViewChild(PollingComponent, { static: false })
    private pollingComponent: PollingComponent;

    @ViewChild(NtpAuthenComponent, { static: false })
    private ntpAuthenComponent: NtpAuthenComponent;

    constructor(
        private i18n: TranslateService,
        private ntpService: NtpService,
        private loading: LoadingService,
        private alert: AlertMessageService,
        private user: UserInfoService,
        private tiMessage: TiMessageService,
        private lockService: SystemLockService
    ) {
        this.ntpDisabled = true;
        this.timeNTP = new TimeNTP();
        this.ntpService.timeNTP = this.timeNTP;
        this.ntpService.buttonState$ = new BehaviorSubject<[TimeNTP, IVerifica]>([this.timeNTP, this.ntpService.verifica]);
        this.privilege = this.user.hasPrivileges(NTPStaticModel.getInstance().privileges);
    }

    ngOnInit(): void {

        this.init();

        this.ntpService.loading$.subscribe(
            (ntpLoad: INTPLoad) => {
                let load = false;
                for (const key in ntpLoad) {
                    if (ntpLoad.hasOwnProperty(key)) {
                        load = ntpLoad[key] || load;
                    }
                }
                if (!load) {
                    this.timeNTPCopy = JSON.stringify(this.timeNTP);
                    this.ntpService.buttonState$.next([this.ntpService.timeNTP, this.ntpService.verifica]);
                }
                this.loading.state.next(load);
            }
        );

        this.ntpService.buttonState$.subscribe(
            ([timeNTP, verifica]) => {
                let state = true;
                let verificaState = true;
                for (const key in verifica) {
                    if (verifica.hasOwnProperty(key)) {
                        verificaState = verifica[key] && verificaState;
                    }
                }
                const strOrigin = JSON.stringify(timeNTP);
                state = verificaState ? (Boolean(this.timeNTPCopy === strOrigin) ? true : false) : true;
                this.ntpDisabled = state;
            }
        );

        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
    }

    private init() {
        this.ntpService.init();
        this.ntpService.ntpLoad.ntpFunction = true;
        this.ntpService.ntpLoad.timeArea = true;
        this.ntpService.loading$.next(this.ntpService.ntpLoad);
    }

    public saveTip() {
        const saveTipInstance = this.tiMessage.open({
            id: 'saveTipMessage',
            type: 'prompt',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('NTP_SAVE_TIP'),
            okButton: {
                text: this.i18n.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    saveTipInstance.dismiss();
                    this.ntpSaveButton();
                }
            },
            cancelButton: {
                text: this.i18n.instant('COMMON_CANCEL')
            }
        });
    }
    /**
     * 保存下发
     */
    private ntpSaveButton(): void {
        this.ntpService.ntpLoad.ntpFunction = true;
        this.ntpService.ntpLoad.timeArea = true;
        this.ntpService.loading$.next(this.ntpService.ntpLoad);
        this.components = {
            timeAreaComponent: this.timeAreaComponent,
            ntpFunctionComponent: this.ntpFunctionComponent,
            pollingComponent: this.pollingComponent,
            ntpAuthenComponent: this.ntpAuthenComponent
        };
        const ntpCopy = JSON.parse(this.timeNTPCopy);
        this.ntpService.ntpSaveButton(ntpCopy, this.timeNTP, this.components).subscribe(
            (result: any) => {
                this.ntpFunctionComponent.changeFootTime();
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                this.ntpService.loading$.next(this.ntpService.ntpLoad);
                this.refresh();
            },
            (error: any) => {
                this.refresh();
            },
            () => {
                this.ntpService.ntpLoad.ntpFunction = false;
                this.ntpService.ntpLoad.timeArea = false;
                this.ntpService.loading$.next(this.ntpService.ntpLoad);
            }
        );
    }

    private refresh(): void {
        this.init();
        this.timeAreaComponent.init();
        this.ntpFunctionComponent.init();
    }

}
