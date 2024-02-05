import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { TiFileItem } from '@cloud/tiny3';
import { AlertMessageService, UserInfoService, ErrortipService, SystemLockService } from 'src/app/common-app/service';
import { LdapService } from '../../ldap.service';
import { TranslateService } from '@ngx-translate/core';
import * as utils from 'src/app/common-app/utils';

@Component({
    selector: 'app-certify-upload',
    templateUrl: './certify-upload.component.html',
    styleUrls: ['./certify-upload.component.scss']
})
export class CertifyUploadComponent implements OnInit {
    @Input() activeController;
    @Output() uploadSuccess = new EventEmitter();
    constructor(
        private user: UserInfoService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private ldapService: LdapService,
        private translate: TranslateService,
        private lockService: SystemLockService
    ) { }
    // 保存按钮状态
    public systemLocked: boolean = false;
    public disabled: boolean = true;
    public loginPwd: string = '';
    public secondError: boolean = false;
    public errorMessage: string = '';
    public loginWithoutPassword: boolean = this.user.loginWithoutPassword;

    // 文件上传
    public certifyFormData: FormData;
    public cerInvalid = {
        url: '',
        alias: 'Content',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        method: 'POST',
        autoUpload: false,
        filters: [{
            name: 'maxCount',
            params: [1]
        }, {
            name: 'type',
            params: ['.crl']
        }, {
            name: 'maxSize',
            params: [100 * 1024]
        }],
        addItemFailed: (data: TiFileItem) => {
            const validResults = data['validResults'];
            const validResultValue = validResults[0];
            if (validResultValue === 'maxSize') {
                this.errorMessage = 'COMMON_UPLOAD_FILE_MORN_1M_ERROR4';
            } else if (validResultValue === 'type') {
                this.errorMessage = 'COMMON_FILE_SUFFIX_CRL';
            }
        },
        onAddItemSuccess: (fileItem) => {
            this.errorMessage = '';
            this.disabled = false;
            this.certifyFormData = null;
            const certifyData = new FormData();
            certifyData.append('Content', fileItem['_file'], fileItem.file.name);
            this.certifyFormData = certifyData;
        },
        onRemoveItem: () => {
            this.disabled = true;
        },
    };
    ngOnInit() {
        // 系统锁定
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
    }
    public close(): void {

    }
    public dismiss(): void {

    }
    public cancel(): void {
        this.dismiss();
    }
    public pwdChange() {
        this.secondError = false;
    }


    // 保存文件和密码
    public upload(): void {
        if (this.disabled || this.systemLocked) {
            return;
        }

        if (!this.loginWithoutPassword && this.loginPwd.length === 0) {
            return;
        }

        let formData: FormData;

        if (this.loginPwd !== '') {
            this.certifyFormData.append('ReauthKey', window.btoa(this.loginPwd));
        }

        formData = this.certifyFormData;

        this.disabled = true;
        this.ldapService.uploadFile({
            userId: this.activeController.id,
            content: formData,
            pwd: this.loginPwd
        }).subscribe({
            next: (res) => {
                this.alert.eventEmit.emit({
                    type: 'success',
                    label: this.translate.instant('COMMON_SUCCESS')
                });
                this.close();
            },
            error: (error) => {
                this.loginPwd = '';
                this.disabled = false;
                const errorId = utils.getMessageId(error.error)[0].errorId;
                if (errorId === 'ReauthFailed') {
                    this.secondError = true;
                } else {
                    const errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                    this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                    this.dismiss();
                }
            }
        });
    }

    public enter(ev) {
        if (ev.keyCode === 13) {
            this.upload();
        }
    }
}

