import { Component, ElementRef, Input, OnChanges } from '@angular/core';
import { NetworkService } from '../../services';
import { AlertMessageService, UserInfoService , ErrortipService } from 'src/app/common-app/service';
import { NetworkPort, IpV4Vaild } from '../../model';
import { TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { UntypedFormBuilder, Validators } from '@angular/forms';
import { TranslateService } from '@ngx-translate/core';
import { getMessageId, HOSTNAME } from 'src/app/common-app/utils';

@Component({
    selector: 'app-network-host',
    templateUrl: './network-host.component.html',
    styleUrls: ['../../network.component.scss', './network-host.component.scss'],
    styles: [
        `.demo-labeleditor-container{
            line-height: 30px;
            max-width: 300px;
            display: inline;
        }`
    ]
})
export class NetworkHostComponent implements OnChanges {

    constructor(
        private networkService: NetworkService,
        private translate: TranslateService,
        private user: UserInfoService,
        private errorTipService: ErrortipService,
        private alert: AlertMessageService,
        private fb: UntypedFormBuilder,
        private eleRef: ElementRef
    ) { }

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isHostEdit: boolean = false;
    public tempHostName: string;
    public hostVaildStaus: boolean = false;

    public hostNameForm = this.fb.group({
        hostNameControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.rangeSize(1, 64), IpV4Vaild.hyphenation(), Validators.pattern(HOSTNAME)]
        ]
    });
    public hostNameValid: TiValidationConfig = {
        tip: this.translate.instant('IBMC_RANGE_OF_VALUES1'),
        type: 'blur',
        tipMaxWidth: '700px',
        tipPosition: 'top-left',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            rangeSize: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            hyphenation: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    @Input() networkPort: NetworkPort;
    @Input() isSystemLock;
    public netHost = {
        label:  '--',
        iconTip: this.translate.instant('COMMON_EDIT'),
        editor: () => {
            this.tempHostName = this.hostNameForm.get('hostNameControl').value;
            if (!this.isConfigureComponentsUser || this.isSystemLock) {
                return;
            }
            this.isHostEdit = true;
            setTimeout(() => {
                this.eleRef.nativeElement.querySelector('#hostNameInput').focus();
                this.hostVaildStaus = this.hostNameForm.valid;
            }, 0);
        },
        cancel: () => {
            this.netHost.label = this.tempHostName;
            this.hostNameForm.setValue({
                hostNameControl: this.tempHostName
            });
            this.isHostEdit = false;
        },
        confirm: () => {
            if (!this.isConfigureComponentsUser || this.isSystemLock || !this.hostVaildStaus) {
                return;
            }
            this.isHostEdit = false;
            const params = {
                HostName: this.netHost.label
            };
            this.networkService.saveNetWork(JSON.stringify(params)).subscribe((response) => {
                this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                this.networkService.portDetails(this.networkPort, response.body);
                this.hostInit(this.networkPort);
            }, (error) => {
                const errorId = getMessageId(error.error)[0].errorId;
                const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({type: 'error', label: errorMessage});
                this.networkService.portDetails(this.networkPort, error.error.data);
                this.hostInit(this.networkPort);
            });
        }
    };

    public hostInit = (networkPort) => {
        this.netHost['label'] = networkPort.getHostName() || '--';
    }

    public hostNameChange = () => {
        this.hostVaildStaus = this.hostNameForm.valid;
        this.hostNameForm.get('hostNameControl').updateValueAndValidity();
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.networkPort && changes.networkPort.currentValue) {
            this.hostInit(changes.networkPort.currentValue);
        }
    }

}
