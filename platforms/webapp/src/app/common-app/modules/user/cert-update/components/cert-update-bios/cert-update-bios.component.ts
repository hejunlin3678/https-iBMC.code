import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { TiFileItem, TiFileInfo, TiModalService } from '@cloud/tiny3';
import {
    AlertMessageService,
    LoadingService,
    UserInfoService,
    ErrortipService,
    TimeoutService
} from 'src/app/common-app/service';
import { getMessageId, getBrowserType } from 'src/app/common-app/utils';
import { CertUpdateService } from '../../services';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';

@Component({
    selector: 'app-cert-update-bios',
    templateUrl: './cert-update-bios.component.html',
    styleUrls: ['./cert-update-bios.component.scss']
})
export class CertUpdateBiosComponent implements OnInit {
    constructor(
        private translate: TranslateService,
        private alert: AlertMessageService,
        private user: UserInfoService,
        private errorTip: ErrortipService,
        private timeoutService: TimeoutService,
        private loadingService: LoadingService,
        private certUpdateService: CertUpdateService,
        private tiModal: TiModalService
    ) {}

    // 安全配置权限
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);

    // 系统是否锁定
    public isSystemLock = this.user.systemLockDownEnabled;
    public dialogBox;
    public inputFieldWidth = '270px';
    public uploadInstanceTrust;
    public uploadInstanceBlock;

    // bios上传证书
    public biosCerUpload = {
        url: '/UI/Rest/AccessMgnt/BootCertificates/ImportBootCertActionInfo',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        filters: [
            {
                name: 'type',
                params: ['.pem,.crt']
            },
            {
                name: 'maxSize',
                params: [100 * 1024]
            }
        ],
        autoUpload: true,
        alias: 'Content',
        successItems: (data: TiFileItem) => {
            this.loadingService.state.next(false);
            this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
        },
        addItemFailed: (data: TiFileInfo) => {
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
                    label: 'COMMON_FILE_SUFFIX_CER_PEM_CRT'
                });
            }
        },
        beforeSendItems: (fileItems) => {
            fileItems.forEach((item: TiFileItem) => {
                item.formData = {
                    user: this.user.userName,
                    bootType: 'Boot'
                };
            });
            this.loadingService.state.next(true);
        },
        errorItems: (fileItem) => {
            this.loadingService.state.next(false);
            const res = fileItem.response;
            this.dealError(res);
        }
    };
    // 吊销列表上传
    public crlUpload = {
        url: '/UI/Rest/AccessMgnt/BootCertificates/ImportBootCrlActionInfo',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
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
        autoUpload: true,
        alias: 'Content',
        successItems: (data: TiFileItem) => {
            this.loadingService.state.next(false);
            this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
        },
        addItemFailed: (data: TiFileInfo) => {
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
                    label: 'COMMON_FILE_SUFFIX_CER_PEM_CRT'
                });
            }
        },
        beforeSendItems: (fileItems) => {
            this.loadingService.state.next(true);
            fileItems.forEach((item: TiFileItem) => {
                item.formData = {
                    user: this.user.userName,
                    bootType: 'Boot'
                };
            });
        },
        errorItems: (fileItem) => {
            this.loadingService.state.next(false);
            const res = fileItem.response;
            this.dealError(res);
        }
    };

    public info = {
        issuer: '',
        subject: '',
        validNotBefore: '',
        validNotAfter: '',
        serialNumber: ''
    };
    public switchState = false;
    public modalOptsFooter: object = [
        {
            label: this.translate.instant('COMMON_OK'),
            disabled: true
        },
        {
            label: this.translate.instant('ALARM_CANCEL'),
            disabled: false
        }
    ];
    // 白名单
    public uploadConfigTrust = {
        url: '/UI/Rest/AccessMgnt/BootCertificates/ImportSecureBootCertActionInfo',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        isAutoUpload: false,
        alias: 'Content',
        filters: [
            {
                name: 'maxCount',
                params: [1]
            },
            {
                name: 'type',
                params: ['.crt,.cer,.pem,.pfx,.p12']
            },
            {
                name: 'maxSize',
                params: [1024 * 1024]
            }
        ],
        onBeforeSendItems: (fileItems) => {
            // 上传前动态添加formData
            const fileName = fileItems[0]._file.name;
            fileItems.forEach((item: TiFileItem) => {
                item.formData = {
                    user: this.user.userName,
                    bootType: 'SecureBoot',
                    type: 'db'
                };
            });
        },
        onRemoveItems: ($event): void => {
            this.modalOptsFooter[0].disabled = true;
        },
        onAddItemFailed: (fileItem) => {
            this.modalOptsFooter[0].disabled = false;
            if ((fileItem.file.type === 'pfx' || fileItem.file.type === 'p12') && fileItem.file.size > 102400) {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'SERVICE_WBS_FILE_SIZE_VALI_TIP01'
                });
            } else if (
                (fileItem.file.type === 'crt' || fileItem.file.type === 'cer' || fileItem.file.type === 'pem') &&
                fileItem.file.size > 1048576
            ) {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'SERVICE_WBS_FILE_SIZE_VALI_TIP02'
                });
            } else {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'SERVICE_UPLOADIN_CORRECT'
                });
            }
            this.dialogBox.close();
            if (this.uploadInstanceTrust) {
                this.uploadInstanceTrust.remove();
            }
        },
        onAddItemSuccess: (fileItem) => {
            this.uploadInstanceTrust = fileItem;
            this.modalOptsFooter[0].disabled = false;
        },
        onSuccessItems: (fileItem: TiFileItem) => {
            this.modalOptsFooter[0].disabled = true;
            this.modalOptsFooter[1].disabled = true;
            this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.dialogBox.close();
        },
        onErrorItems: (event: { fileItems: TiFileItem; response: string; status: number }) => {
            this.dealError(event.response);
            this.dialogBox.close();
        }
    };
    // 黑名单
    public uploadConfigBlock = {
        url: '/UI/Rest/AccessMgnt/BootCertificates/ImportSecureBootCertActionInfo',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        isAutoUpload: false,
        alias: 'Content',
        filters: [
            {
                name: 'maxCount',
                params: [1]
            },
            {
                name: 'type',
                params: ['.crt,.cer,.pem,.pfx,.p12']
            },
            {
                name: 'maxSize',
                params: [1024 * 1024]
            }
        ],
        onBeforeSendItems: (fileItems) => {
            // 上传前动态添加formData
            const fileName = fileItems[0]._file.name;
            fileItems.forEach((item: TiFileItem) => {
                item.formData = {
                    user: this.user.userName,
                    bootType: 'SecureBoot',
                    type: 'dbx'
                };
            });
        },
        onRemoveItems: ($event): void => {
            this.modalOptsFooter[0].disabled = true;
        },
        onAddItemFailed: (fileItem) => {
            this.modalOptsFooter[0].disabled = false;
            if ((fileItem.file.type === 'pfx' || fileItem.file.type === 'p12') && fileItem.file.size > 102400) {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'SERVICE_WBS_FILE_SIZE_VALI_TIP01'
                });
            } else if (
                (fileItem.file.type === 'crt' || fileItem.file.type === 'cer' || fileItem.file.type === 'pem') &&
                fileItem.file.size > 1048576
            ) {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'SERVICE_WBS_FILE_SIZE_VALI_TIP02'
                });
            } else {
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'SERVICE_UPLOADIN_CORRECT'
                });
            }
            this.dialogBox.close();
            if (this.uploadInstanceBlock) {
                this.uploadInstanceBlock.remove();
            }
        },
        onAddItemSuccess: (fileItem) => {
            this.uploadInstanceBlock = fileItem;
            this.modalOptsFooter[0].disabled = false;
        },
        onSuccessItems: (fileItem: TiFileItem) => {
            this.modalOptsFooter[0].disabled = true;
            this.modalOptsFooter[1].disabled = true;
            this.alert.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.dialogBox.close();
        },
        onErrorItems: (event: { fileItems: TiFileItem; response: string; status: number }) => {
            this.dealError(event.response);
            this.dialogBox.close();
        }
    };
    ngOnInit(): void {
        forkJoin([this.certUpdateService.getBootCertificates(), this.certUpdateService.getSecureBootState()]).subscribe(
            ([certificateData, stateData]) => {
                if (certificateData.body.HttpsCert?.length === 0) {
                    return false;
                }
                const cert = certificateData.body.HttpsCert[0];
                this.info.issuer = cert.Issuer.CommonName;
                this.info.serialNumber = cert.SerialNumber;
                this.info.subject = cert.Subject.CommonName;
                this.info.validNotBefore = cert.ValidNotBefore;
                this.info.validNotAfter = cert.ValidNotAfter;
                const secureBoot = stateData.body.SecureBoot;
                this.switchState = secureBoot === 'Enabled';
                this.loadingService.state.next(false);
            }
        );
    }

    // 统一处理文件上传的错误
    public dealError(res) {
        let errorId: string;
        let errorMessage = 'COMMON_FAILED';
        if (getMessageId(res).length > 0) {
            errorId = getMessageId(res)[0]?.errorId;
            errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
            if (errorId === 'NoValidSession') {
                this.timeoutService.error401.next(errorId);
            }
        }
        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
    }
    public securityBtnClick(content) {
        this.dialogBox = this.tiModal.open(content, {
            id: 'biosSecurity',
            context: {
                close: (): void => {
                    if (this.uploadInstanceTrust) {
                        this.uploadInstanceTrust.upload();
                    }
                    if (this.uploadInstanceBlock) {
                        this.uploadInstanceBlock.upload();
                    }
                },
                dismiss: (): void => {
                    this.dialogBox.dismiss();
                }
            }
        });
    }
    public switchChange(val: boolean) {
        const param = {
            SecureBoot: val ? 1 : 0
        };
        this.loadingService.state.next(true);
        this.certUpdateService.setSecureBootState(param).subscribe(
            (res) => {
                this.alert.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });
                this.loadingService.state.next(false);
            },
            (err) => {
                if (err.error) {
                    const errorId = getMessageId(err.error)?.[0]?.errorId;
                    if (errorId) {
                        const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({
                            type: 'error',
                            label: errorMessage
                        });
                        this.loadingService.state.next(false);
                        return;
                    }
                }
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: 'COMMON_FAILED'
                });
                this.loadingService.state.next(false);
            }
        );
    }
}
