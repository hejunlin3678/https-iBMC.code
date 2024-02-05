import { Component, OnInit } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiMessageService, TiFileItem } from '@cloud/tiny3';
import { TwoFactorService } from '../../services/two-factor.service';
import { Subscription } from 'rxjs';
import { getMessageId } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';
import {
    UserInfoService,
    ErrortipService,
    AlertMessageService,
    LoadingService,
    SystemLockService,
    TimeoutService
} from 'src/app/common-app/service';

@Component({
    selector: 'app-root-certificate',
    templateUrl: './root-certificate.component.html',
    styleUrls: ['./root-certificate.component.scss']
})
export class RootCertificateComponent implements OnInit {

    constructor(
        private service: TwoFactorService,
        private tiMessage: TiMessageService,
        private user: UserInfoService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private translate: TranslateService,
        private loading: LoadingService,
        private systemLockService: SystemLockService,
        private timeoutService: TimeoutService,
    ) {

    }

    public canNotDeleted: boolean = false;
    public subscription: Subscription;
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureUsers']);
    public systemLocked: boolean = this.systemLockService.getState();

    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    private data: TiTableRowData[] = [];
    public columns: TiTableColumns[] = [
        {
            title: 'TWO_FACTORS_CERTIFICATES_ID',
            width: '10%'
        },
        {
            title: 'TWO_FACTORS_ISSUEDBY',
            width: '20%'
        },
        {
            title: 'TWO_FACTORS_ISSUEDTO',
            width: '20%'
        },
        {
            title: 'TWO_FACTORS_DEADLING',
            width: '20%'
        },
        {
            title: 'TWO_FACTORS_CRL',
            width: '20%'
        },
        {
            title: 'TWO_FACTORS_CERTIFICATE_OPERATE',
            width: '10%'
        }
    ];

    public isUpload: boolean = false;

    public rootUpload: any = {
        url: '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportRootCertificate',
        autoUpload: true,
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.token
        },
        alias: 'Content',
        filters: [{
            name: 'type',
            params: ['.cer,.pem,.crt']
        }, {
            name: 'maxSize',
            params: [1024 * 1024]
        }],
        rootAddItemFailed: ((data) => {
            if (data.validResults.indexOf('type') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FILE_SUFFIX_CER_PEM_CRT' });
            } else if (data.validResults.indexOf('maxSize') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR' });
            }
        }),
        rootAddItemSuccess: (() => {
            this.isUpload = true;
        }),
        rootErrorItems: ((fileItem) => {
            this.isUpload = false;
            const res = fileItem.response;
            this.dealError(res);
        }),
        rootSuccessItems: ((row, fileItem: TiFileItem) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.getRootCertList();
            this.isUpload = false;
        })
    };
    public crlUpload: any = {
        url: '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportCrlCertificate',
        autoUpload: true,
        alias: 'Content',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.token,
        },
        filters: [{
            name: 'type',
            params: ['.crl']
        }, {
            name: 'maxSize',
            params: [100 * 1024]
        }],
        crlBeforeSendItems: (fileItems, row) => {
            fileItems.forEach((item: TiFileItem) => {
                item.formData = {
                    CertID: row.certId,
                };
            });
        },
        crlAddItemFailed: ((data) => {
            if (data.validResults.indexOf('type') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FILE_SUFFIX_CRL' });
            } else if (data.validResults.indexOf('maxSize') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR4' });
            }
        }),
        crlAddItemSuccess: ((row) => {
            row.isCRLUpload = true;
        }),
        crlErrorItems: ((fileItem, row) => {
            row.isCRLUpload = false;
            const res = fileItem.response;
            this.dealError(res);
        }),
        crlSuccessItems: ((fileItem: TiFileItem, row) => {
            row.isCRLUpload = false;
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.getRootCertList();
        })
    };

    public getRootCertList() {
        this.service.getTwoFactoryData().subscribe((response) => {
            this.canNotDeleted = response['body'].TwoFactorAuthenticationEnabled;
            const certificates = this.service.rootCertInfoAssemble(response['body']);
            this.data = certificates;
            this.srcData = {
                data: this.data,
                state: {
                    searched: false,
                    sorted: false,
                    paginated: false
                }
            };
        });
    }

    public delRootCertClick(certId) {
        if (this.systemLocked || this.canNotDeleted) {
            return;
        }
        this.tiMessage.open({
            type: 'prompt',
            id: 'delRoot',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('TWO_FACTORS_DELETE_CERTIFICATE_TIP'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: {},
            close: () => {
                const param = {
                    CertificateType: 0,
                    ID: certId
                };
                this.loading.state.next(true);
                this.service.delRootCert(param).subscribe(() => {
                    this.loading.state.next(false);
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.getRootCertList();
                }, () => {
                    this.loading.state.next(false);
                });
            },
            dismiss: () => {

            }
        });
    }

    public deleteCrlCertify(id: number) {
        if (this.systemLocked) {
            return;
        }
        this.tiMessage.open({
            type: 'prompt',
            id: 'delCrlCertify',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('TWO_FACTORS_DELETE_CRL_TIP'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: {},
            close: () => {
                this.loading.state.next(true);
                this.service.delCrlVerification(id).subscribe(() => {
                    this.loading.state.next(false);
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.getRootCertList();
                }, () => {
                    this.loading.state.next(false);
                });
            },
            dismiss: () => {}
        });
    }

    // 统一处理文件上传的错误
    public dealError (res) {
        let errorId: string;
        let errorMessage = 'COMMON_FAILED';
        if (getMessageId(res).length > 0) {
            errorId = getMessageId(res)[0]?.errorId;
            errorMessage = this.errorTip.getErrorMessage(errorId);
            if (errorId === 'NoValidSession') {
                this.timeoutService.error401.next(errorId);
            }
        }
        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
    }

    public init() {
        // 系统锁定服务实现监听
        this.systemLockService.lockStateChange.subscribe({
            next: state => {
                this.systemLocked = state;
            }
        });

        this.subscription = this.service.eventEmit.subscribe((response) => {
            this.canNotDeleted = response.body.TwoFactorAuthenticationEnabled;
            const certificates = this.service.rootCertInfoAssemble(response['body']);
            this.data = certificates;
            this.srcData = {
                data: this.data,
                state: {
                    searched: false,
                    sorted: false,
                    paginated: false
                }
            };
        });
    }

    ngOnInit() {
        this.init();
    }
    ngOnDestroy(): void {
        this.subscription.unsubscribe();
    }
}
