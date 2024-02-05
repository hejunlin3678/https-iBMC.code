import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { Router } from '@angular/router';
import { TiMessageService } from '@cloud/tiny3';
import { TwoFactorService } from './services/two-factor.service';
import { ISwitchs, ITab } from './interface';
import { AlertMessageService, LoadingService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-two-factor',
    templateUrl: './two-factor.component.html',
    styleUrls: ['./two-factor.component.scss']
})
export class TwoFactorComponent implements OnInit {

    constructor(
        private router: Router,
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private service: TwoFactorService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private user: UserInfoService
    ) { }

    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureUsers']);
    public systemLocked: boolean = this.user.systemLockDownEnabled;

    public twoFactor: ISwitchs = {
        label: 'COMMON_USER_TWO_FACTOR',
        state: false
    };
    public OCSP: ISwitchs = {
        label: 'TWO_FACTORS_REVOCATION_CHECK',
        state: false
    };
    public CRL: ISwitchs = {
        label: 'TWO_FACTORS_CRL_CHECK',
        state: false
    };

    public rootCertificate: ITab = {
        title: 'TWO_FACTORS_ROOT_CERTIFICATE',
        active: true,
        onActiveChange: (isActive: boolean): void => {
            if (isActive) {
                this.gotoRootCertificate();
            }
        }
    };

    public clientCertificate: ITab = {
        title: 'TWO_FACTORS_CLIENT_CERTIFICATE',
        active: false,
        onActiveChange: (isActive: boolean): void => {
            if (isActive) {
                this.gotoClientCertificate();
            }
        }
    };

    ngOnInit(): void {
        if (this.router.url === '/navigate/user/two-factor/client') {
            this.clientCertificate.active = true;
        }
        this.init();
    }

    // 系统锁定状态变化
    public lockStateChange(state) {
        this.systemLocked = state;
    }

    public init() {
        this.loading.state.next(true);
        this.service.getTwoFactoryData().subscribe((response) => {
            this.twoFactor.state = response['body'].TwoFactorAuthenticationEnabled;
            this.OCSP.state = response['body'].OCSPEnabled;
            this.CRL.state = response['body'].CRLEnabled;
            this.service.eventEmit.emit(response);
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }

    public gotoRootCertificate() {
        this.router.navigate(['/navigate/user/two-factor/root']);
        this.init();
    }

    public gotoClientCertificate() {
        this.router.navigate(['/navigate/user/two-factor/client']);
    }

    public saveSwitchData(param) {
        this.service.setTwoFactoryData(param).subscribe((response) => {
            this.twoFactor.state = response['body'].TwoFactorAuthenticationEnabled;
            this.OCSP.state = response['body'].OCSPEnabled;
            this.CRL.state = response['body'].CRLEnabled;
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
        });
    }

    public twoFactorChange() {
        if (this.systemLocked) {
            return;
        }
        const switchState = !this.twoFactor.state;
        const param = {
            TwoFactorAuthenticationEnabled: switchState
        };
        if (!switchState) {
            this.saveSwitchData(param);
            return;
        }
        this.tiMessage.open({
            id: 'twoFactorDialog',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('TWO_FACTORS_FACTOR_AUTH_TIP'),
            close: () => {
                this.saveSwitchData(param);
            },
            dismiss: () => { }
        });
    }

    public ocspChange() {
        if (this.systemLocked) {
            return;
        }
        const switchState = !this.OCSP.state;
        const param = {
            OCSPEnabled: switchState
        };
        if (!switchState) {
            this.saveSwitchData(param);
            return;
        }
        this.tiMessage.open({
            id: 'ocspDialog',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('TWO_FACTORS_OCSP_TIP'),
            okButton: {
                autofocus: false,
                primary: true,
            },
            cancelButton: {
            },
            close: () => {
                this.saveSwitchData(param);
            },
            dismiss: () => { }
        });
    }

    public crlChange() {
        if (this.systemLocked) {
            return;
        }
        const switchState = !this.CRL.state;
        const param = {
            CRLEnabled: switchState
        };
        if (!switchState) {
            this.saveSwitchData(param);
            return;
        }
        this.tiMessage.open({
            id: 'crlDialog',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('TWO_FACTORS_CRL_TIP'),
            okButton: {
                autofocus: false,
                primary: true,
            },
            cancelButton: {
            },
            close: () => {
                this.saveSwitchData(param);
            },
            dismiss: () => { }
        });
    }
}
