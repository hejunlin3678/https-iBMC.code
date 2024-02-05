import { Router } from '@angular/router';
import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';

import { TiMessageService, TiModalRef, TiModalService } from '@cloud/tiny3';

import { CommonService } from 'src/app/common-app/service/common.service';
import { getMessageId, getRandomFileName } from 'src/app/common-app/utils';

import { HomeService } from '../../services/home.service';
import { HomeCollectComponent } from 'src/app/common-app/components/home-collect/home-collect.component';
import { IAccess } from '../../model/home.datatype';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { AlertMessageService, ErrortipService, UserInfoService, TimeoutService } from 'src/app/common-app/service';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';

@Component({
    selector: 'app-home-irm-quick-access',
    templateUrl: './home-irm-quick-access.component.html',
    styleUrls: ['./home-irm-quick-access.component.scss']
})
export class HomeIrmQuickAccessComponent implements OnInit {
    @Input() access: IAccess;
    public imgUrl: string;
    private messageModal: TiModalRef;
    public systemLocked: boolean;
    private modalInstance: TiModalRef = null;

    constructor(
        private router: Router,
        private homeService: HomeService,
        private tiMessage: TiMessageService,
        private commonService: CommonService,
        private translate: TranslateService,
        private lockService: SystemLockService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private user: UserInfoService,
        private timeoutService: TimeoutService,
        private modal: TiModalService
    ) { }

    ngOnInit() {
        this.imgUrl = `assets/irm-assets/image/home/default/${this.access.icon}.png`;
        if (sessionStorage.getItem('collectProgressURL')) {
            this.showIsCollecting();
        }
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
    }

    public clickFn() {
        if (this.access.state) {
            if (this.access.state === '/navigate/system/power') {
                this.router.navigate([this.access.state], { queryParams: { id: 2 } });
            } else {
                this.router.navigate([this.access.state]);
            }
        }
        if (this.access.onKeySelect) {
            this.showConfirmMsg();
        }
        if (this.access.restoreSet) {
            this.restoreSet();
        }
    }

    private showConfirmMsg() {
        const msgInstance = this.tiMessage.open({
            id: 'oneKeyConfirm',
            type: 'warn',
            okButton: {
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    this.oneKeyCollect('click');
                    msgInstance.close();
                }
            },
            cancelButton: {
                text: this.translate.instant('COMMON_CANCEL')
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('HOME_CONFIRM_ONE_KEY_SELECT')
        });
    }

    private oneKeyCollect(data: string) {
        this.messageModal = this.tiMessage.open({
            id: 'oneKeyModel',
            type: 'prompt',
            okButton: {
                show: false,
                autofocus: false,
            },
            cancelButton: {
                show: false
            },
            closeIcon: false,
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                if (reason) {
                    modalRef.destroy(reason);
                }
            },
            title: this.translate.instant('HOME_PROMPTMESSAGE'),
            content: HomeCollectComponent
        });

        this.commonService.getGenericInfo().subscribe({
            next: (res) => {
                let refreshTimeData = res.body.CurrentTime;
                refreshTimeData = refreshTimeData.substring(0, 16);
                refreshTimeData = refreshTimeData.replace(/-|:/g, '');
                refreshTimeData = refreshTimeData.replace(' ', '-');
                const productName = localStorage.getItem('productName').replace(/\s/g, '');
                const productSN = localStorage.getItem('productSN');
                const fileName = (productName ? productName + '_' : '')
                    + (productSN ? productSN + '_' : '') + refreshTimeData + '.tar.gz';
                if (!localStorage.getItem('fileName')) {
                    localStorage.setItem('fileName', fileName);
                }
                if (data !== 'init') {
                    this.homeService.oneKeyCollect(fileName).subscribe((url) => {
                        sessionStorage.setItem('collectProgressURL', url);
                        this.queryCollectProgress(fileName);
                    }, (err) => {
                        const url = sessionStorage.getItem('collectProgressURL');
                        if (url && getMessageId(err.error)[0].errorId.indexOf('DuplicateExportingErr') > -1) {
                            this.queryCollectProgress(fileName);
                        } else {
                            this.messageModal.close();
                        }
                    });
                } else {
                    const url = sessionStorage.getItem('collectProgressURL');
                    if (url) {
                        this.queryCollectProgress(fileName);
                    } else {
                        this.messageModal.close();
                    }
                }
            },
        });
    }

    private restoreSet() {
        if (this.access.lock && this.lockService.getState()) {
            return;
        }
        const instance: TiModalRef = this.tiMessage.open({
            id: 'restoreModel',
            type: 'warn',
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    instance.close();
                    this.resetModal();
                },
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_CANCEL'),
                click: () => {
                    instance.close();
                }
            },
            title: this.translate.instant('HOME_RESTOREDEFAULT'),
            content: `<div>
                <p class='accessResetModal'>
                ${this.translate.instant('HOME_CONFIRM_UPGRADE2')}</p>
                <p>${this.translate.instant('HOME_RESTOREDDEFAULTS')}</p>
                <div>${this.homeService.getAddressIPv4()}</div>
                </div>`
        });
    }

    private queryCollectProgress(fileName: string) {
        const url = sessionStorage.getItem('collectProgressURL');
        this.homeService.collectProgress(url).subscribe(({state, taskPercentage, downloadUrl}) => {
            if (taskPercentage) {
                this.commonService.sendMessage(taskPercentage);
            }
            if (taskPercentage === '100%' && state === 'WAIT_TRANSFER') {
                sessionStorage.removeItem('collectProgressURL');
                localStorage.removeItem('fileName');
                this.commonService.oneKeyDownloadFile(downloadUrl, fileName);
                setTimeout(() => {
                    this.showSuccessMessage();
                }, 3000);
            } else {
                setTimeout(() => {
                    this.queryCollectProgress(fileName);
                }, 2000);
            }
        });
    }

    private showSuccessMessage() {
        this.messageModal.close();
        const successModal = this.tiMessage.open({
            id: 'successModel',
            type: 'confirm',
            okButton: {
                show: true,
                autofocus: false,
                click() {
                    successModal.close();
                }
            },
            cancelButton: {
                show: false
            },
            title: this.translate.instant('HOME_PROMPTMESSAGE'),
            content: this.translate.instant('COMMON_SUCCESS')
        });
    }

    public showIsCollecting() {
        const collectProgressURL = sessionStorage.getItem('collectProgressURL');
        if (collectProgressURL) {
            this.oneKeyCollect('init');
        }
    }
    private resetModal() {
        let params = {};
        // 账号密码登录
        if (!this.user.loginWithoutPassword) {
            this.modalInstance = this.modal.open(SecondPasswordComponent, {
                id: 'resetModal',
                modalClass: 'secondModal',
                context: {
                    password: '',
                    state: false,
                    active: false
                },
                beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                    // reason, true:点击ok， false:点击取消
                    if (!reason) {
                        modalRef.destroy(false);
                        return;
                    }

                    if (modalRef.content.instance.password === '' || !modalRef.content.instance.active) {
                        return;
                    }
                    modalRef.content.instance.active = false;
                    params = {
                        'ReauthKey': window.btoa(modalRef.content.instance.password)
                    };
                    this.reset(params);
                }
            });
        } else {
            this.reset(params);
        }
    }

    private reset(params) {
        this.homeService.restoreSet(params).subscribe(
            () => {
                // 成功后，重置模态框对象
                this.modalInstance?.destroy(true);
                this.modalInstance = null;
                this.resetMsg();
            },
            (error) => {
                const errorIds = getMessageId(error.error);
                let errorMessage = '';
                if (errorIds.length > 0) {
                    const errorId = errorIds[0]?.errorId;
                    errorMessage = this.errorTip.getErrorMessage(errorId) || 'COMMON_FAILED';
                    if (errorId === 'AuthorizationFailed') {
                        if (this.modalInstance) {
                            this.modalInstance.content.instance.state = true;
                        }
                        return;
                    }
                    if (errorId === 'NoValidSession') {
                        this.timeoutService.error401.next(errorId);
                    }
                    this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                    // 关闭二次认证密码框
                    this.modalInstance?.destroy(false);
                }
            }
        );
    }

    private resetMsg() {
        const messageInstance = this.tiMessage.open({
            id: 'restoreMessage',
            type: 'error',
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    messageInstance.close();
                    this.commonService.clear();
                    this.router.navigate(['/login']);
                }
            },
            cancelButton: {
                show: false
            },
            title: this.translate.instant('COMMON_INFORMATION'),
            content: this.translate.instant('HOME_RESTORE_CONFIGURATION_SUCCESS')
        });
    }
}
