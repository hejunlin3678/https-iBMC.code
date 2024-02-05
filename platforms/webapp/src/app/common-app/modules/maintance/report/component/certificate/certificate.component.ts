import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { TiFileItem, TiMessageService } from '@cloud/tiny3';
import { getMessageId } from 'src/app/common-app/utils';
import {
    UserInfoService,
    AlertMessageService,
    ErrortipService,
    TimeoutService,
    LoadingService
} from 'src/app/common-app/service';
import { CertificateService } from './certificate.service';

@Component({
    selector: 'app-certificate',
    templateUrl: './certificate.component.html',
    styleUrls: ['./certificate.component.scss']
})
export class CertificateComponent implements OnInit {
    @Output() uploadSuccess = new EventEmitter();
    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private timeoutService: TimeoutService,
        private certificateService: CertificateService,
        private loading: LoadingService,
        private tiMessage: TiMessageService,
        private loadingService: LoadingService
    ) {}
    @Input() info;
    @Input() cerInvalidConfig;
    @Input() type: string = 'client';
    @Input() isSystemLock;
    @Input() hasPermission;
    public tipInfo = this.translate.instant('ALARM_REPORT_SYSLOG_EXPIRED');
    // 吊销证书文件上传
    public fileInstance = null;
    // 默认的 证书吊销列表 上传 的配置
    public cerInvalid = {
        url: '/UI/Rest/Maintenance/SyslogNotification/ImportCrl',
        alias: 'Content',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        method: 'POST',
        autoUpload: true,
        filters: [
            {
                name: 'type',
                params: ['.crl']
            },
            {
                name: 'maxSize',
                params: [100 * 1024]
            }
        ],
        addItemFailed: (data: TiFileItem) => {
            this.loadingService.state.next(false);
            const validResults = data['validResults'];
            const validResultValue = validResults[0];
            if (validResultValue === 'maxSize') {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR4'
                });
            } else if (validResultValue === 'type') {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'COMMON_FILE_SUFFIX_CRL'
                });
            }
        },
        successItems: (data: TiFileItem) => {
            this.loadingService.state.next(false);
            this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.uploadSuccess.emit();
        },
        errorItems: (fileItem) => {
            this.loadingService.state.next(false);
            const res = fileItem.response;
            const errorId = getMessageId(res)[0]?.errorId;
            const errorMessage = errorId ? this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED' : 'COMMON_FAILED';
            if (errorId === 'NoValidSession') {
                this.timeoutService.error401.next(errorId);
            }
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        },
        onAddItemSuccess: (fileItem) => {
            this.loadingService.state.next(true);
            this.fileInstance = fileItem;
        }
    };
    public delCrlCertify() {
        if (this.isSystemLock) {
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
                this.certificateService.delSyslogCrlCertificate(this.info.deleteCrlUrl).subscribe(() => {
                    this.loading.state.next(false);
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.uploadSuccess.emit();
                }, () => {
                    this.loading.state.next(false);
                });
            },
            dismiss: () => {}
        });
    }
    ngOnInit() {
        // cerInvalid 是 证书吊销列表 上传 的默认配置，cerInvalidConfig是传入的配置
        if (this.cerInvalidConfig && Object.prototype.toString.call(this.cerInvalidConfig) === '[object Object]') {
            // 传入的配置替换掉默认的一些配置项,比如上传地址
            Object.assign(this.cerInvalid, this.cerInvalidConfig);
        }
    }
}
