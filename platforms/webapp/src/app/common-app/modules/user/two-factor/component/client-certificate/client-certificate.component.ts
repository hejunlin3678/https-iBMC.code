import { Component, OnInit } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiMessageService, TiFileItem } from '@cloud/tiny3';
import { TwoFactorService } from '../../services/two-factor.service';
import { getMessageId } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';
import {
    AlertMessageService,
    UserInfoService,
    LoadingService,
    ErrortipService,
    SystemLockService,
    TimeoutService
} from 'src/app/common-app/service';

@Component({
    selector: 'app-client-certificate',
    templateUrl: './client-certificate.component.html',
    styleUrls: ['./client-certificate.component.scss']
})
export class ClientCertificateComponent implements OnInit {

    constructor(
        private service: TwoFactorService,
        private alert: AlertMessageService,
        private user: UserInfoService,
        private tiMessage: TiMessageService,
        private loading: LoadingService,
        private errorTip: ErrortipService,
        private translate: TranslateService,
        private systemLockService: SystemLockService,
        private timeoutService: TimeoutService,
    ) { }

    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    private data: TiTableRowData[] = [];
    public columns: TiTableColumns[] = [
        {
            title: 'TWO_FACTORS_CERTIFICATES_ID',
            width: '10%'
        },
        {
            title: 'COMMON_USERNAME',
            width: '15%'
        },
        {
            title: 'COMMON_ROLE',
            width: '15%'
        },
        {
            title: 'TWO_FACTORS_ROOT_CERTIFICATE_STATE',
            width: '15%'
        },
        {
            title: 'TWO_FACTORS_REVOKED_STATE',
            width: '15%'
        },
        {
            title: 'TWO_FACTORS_REVOKED_TIME',
            width: '15%'
        },
        {
            title: 'TWO_FACTORS_CLIENT_CERTIFICATE_FINGERPRINT',
            width: '20%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '10%'
        }
    ];
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureUsers']);
    public systemLocked: boolean = this.systemLockService.getState();

    public clientUpload: any = {
        url: '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportClientCertificate',
        autoUpload: true,
        alias: 'Content',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.token
        },
        filters: [{
            name: 'type',
            params: ['.cer,.pem,.crt']
        }, {
            name: 'maxSize',
            params: [1024 * 1024]
        }],
        clientBeforeSendItems: (fileItems, row) => {
            fileItems.forEach((item: TiFileItem) => {
                item.formData = {
                    UserID: row.id,
                };
            });
        },
        clientAddItemFailed: ((data) => {
            if (data.validResults.indexOf('type') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FILE_SUFFIX_CER_PEM_CRT' });
            } else if (data.validResults.indexOf('maxSize') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR' });
            }
        }),
        clientAddItemSuccess: ((row) => {
            row.isClientUpload = true;
        }),
        clientErrorItems: ((fileItem, row) => {
            row.isClientUpload = false;
            const res = fileItem.response;
            let errorMessage = 'COMMON_FAILED';
            let errorId: string;
            if (getMessageId(res).length > 0) {
                errorId = getMessageId(res)[0]?.errorId;
                errorMessage = this.errorTip.getErrorMessage(errorId);
                if (errorId === 'NoValidSession') {
                    this.timeoutService.error401.next(errorId);
                }
            }
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        }),
        clientSuccessItems: ((fileItem, row) => {
            row.isClientUpload = false;
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.init();
        })
    };

    public delClientCertClick(certId: number) {
        if (this.systemLocked) {
            return;
        }
        this.tiMessage.open({
            type: 'prompt',
            id: 'delClient',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('TWO_FACTORS_DELETE_CERTIFICATE_TIP'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: {},
            close: () => {
                const param = {
                    CertificateType: 1,
                    ID: certId
                };
                this.loading.state.next(true);
                this.service.delRootCert(param).subscribe((response) => {
                    this.loading.state.next(false);
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.init();
                }, () => {
                    this.loading.state.next(false);
                });
            },
            dismiss: () => { }
        });
    }

    public init() {
        // 系统锁定监控
        this.systemLockService.lockStateChange.subscribe({
            next: state => {
                this.systemLocked = state;
            }
        });

        this.service.getTwoFactoryData().subscribe((response) => {
            const clientCertData = this.service.clientCertInfoAssemble(response['body']);
            this.data = clientCertData;
            this.srcData = {
                data: this.data,
                state: {
                    searched: false,
                    sorted: false,
                    paginated: false
                }
            };
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });

    }
    ngOnInit() {
        this.loading.state.next(true);
        this.init();
    }

}
