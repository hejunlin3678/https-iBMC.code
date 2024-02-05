import { Component, Input, Output, EventEmitter, OnChanges, OnDestroy } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { TiModalService, TiModalRef } from '@cloud/tiny3';
import { CertifyUploadComponent } from '../certify-upload/certify-upload.component';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { getMessageId } from 'src/app/common-app/utils';
import { LdapService } from '../../ldap.service';
import { AlertMessageService } from 'src/app/common-app/service';

@Component({
    selector: 'app-ldap-certify',
    templateUrl: './ldap-certify.component.html',
    styleUrls: ['./ldap-certify.component.scss']
})
export class LdapCertifyComponent implements OnChanges, OnDestroy {
    constructor(
        private translate: TranslateService,
        private modal: TiModalService,
        private alert: AlertMessageService,
        private ldapService: LdapService
    ) { }
    @Output() uploadSuccess = new EventEmitter();
    @Input() cerInfo;
    @Input() activeController;
    @Input() systemLocked = false;
    @Input() hasPermission = true;
    public toggleCollspan = false;
    public isCollapsed = false;
    public tipInfo = this.translate.instant('ALARM_REPORT_SYSLOG_EXPIRED');
    public modalInstant: TiModalRef;
    public serverCertShow = false;
    public middleCertShow = false;
    public rootCertShow = false;

    // 吊销证书文件上传
    public uploadCertify() {
        this.modalInstant = this.modal.open(CertifyUploadComponent, {
            id: 'uploadCertify',
            modalClass: 'certifyUpload',
            context: {
                activeController: this.activeController
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                modalRef.destroy(reason);
                this.modalInstant = null;
                if (reason) {
                    this.uploadSuccess.emit();
                }
            }
        });
    }
    // 删除吊销证书
    public deleteCertify() {
        this.modalInstant = this.modal.open(SecondPasswordComponent, {
            id: 'deleteCertifyModal',
            context: {
                password: '',
                state: false,
                active: false
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                if (!reason) {
                    modalRef.destroy(reason);
                } else {
                    if (modalRef.content.instance.password === '' || !modalRef.content.instance.active) {
                        return;
                    }
                    modalRef.content.instance.active = false;
                    // 发送删除吊销证书请求
                    this.ldapService.deleteCrlVerification(this.activeController?.id, modalRef.content.instance.password).subscribe(() => {
                            this.modalInstant.close();
                            modalRef.destroy(reason);
                            this.uploadSuccess.emit();
                            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        }, (error) => {
                            // 发生错误时，先清除二次认证密码
                            this.modalInstant.content.instance.password = '';
                            const errorId = getMessageId(error.error)[0].errorId;
                            if (errorId === 'ReauthFailed') {
                                modalRef.content.instance.state = true;
                            } else {
                                this.modalInstant.close();
                            }
                        });
                }
            }
        });
    }
    ngOnDestroy(): void {
        if (this.modalInstant) {
            this.modalInstant.destroy(true);
        }
    }
    ngOnChanges() {
        this.serverCertShow = false;
        this.middleCertShow = false;
        this.rootCertShow = false;
        this.isCollapsed = true;

        if (this.cerInfo.server) {
            this.serverCertShow = true;
        }

        if (this.cerInfo.middle) {
            this.middleCertShow = true;
            // 如果有中间证书，则显示下拉按钮
            this.isCollapsed = true;
        }

        if (this.cerInfo.root) {
            this.rootCertShow = true;
        }
    }
    public clickFn() {
        this.isCollapsed = !this.isCollapsed;
    }
}
