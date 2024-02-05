import { Router } from '@angular/router';
import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';

import { TiMessageService, TiModalRef } from '@cloud/tiny3';

import { CommonService } from 'src/app/common-app/service/common.service';
import { getMessageId} from 'src/app/common-app/utils';

import { HomeService } from './../../services/home.service';
import { HomeEmmCollectComponent } from './../home-collect/home-collect.component';
import { IAccess } from '../../home.datatype';
import { AlertMessageService, ErrortipService, SystemLockService, TimeoutService, UserInfoService } from 'src/app/common-app/service';
import { GlobalDataService } from 'src/app/common-app/service';

@Component({
    selector: 'app-home-emm-quick-access',
    templateUrl: './home-quick-access.component.html',
    styleUrls: ['./home-quick-access.component.scss']
})
export class HomeEmmQuickAccessComponent implements OnInit {
    @Input() access: IAccess;
    public imgUrl: string;
    private messageModal: TiModalRef;
    constructor(
        private router: Router,
        private homeService: HomeService,
        private tiMessage: TiMessageService,
        private commonService: CommonService,
        private translate: TranslateService,
        private globalData: GlobalDataService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService
    ) { }

    ngOnInit() {
        this.imgUrl = `assets/emm-assets/image/home/default/${this.access.icon}.png`;
        if (this.access.onKeySelect) {
            this.showIsCollecting();
        }
    }

    public clickFn() {
        if (this.access.state) {
            if (this.access.state === '/navigate/system/power') {
                this.globalData.powerSleepSupport.then((res: boolean) => {
                    this.router.navigate([this.access.state], { queryParams: { id: (res ? 2 : 0) } });
                });
            } else {
                this.router.navigate([this.access.state]);
            }
        }
        if (this.access.onKeySelect) {
            this.showConfirmMsg();
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
            content: HomeEmmCollectComponent
        });
        this.commonService.getGenericInfo().subscribe({
            next: (res) => {
                let refreshTimeData = res.body.CurrentTime;
                refreshTimeData = refreshTimeData.substring(0, 16);
                refreshTimeData = refreshTimeData.replace(/-|:/g, '');
                refreshTimeData = refreshTimeData.replace(' ', '-');
                let productName = localStorage.getItem('productName');
                productName = productName ? productName.replace(' ', '') : '';
                const productSN = localStorage.getItem('productSN');
                const fileName = (productName ? productName + '_' : '')
                    + (productSN ? productSN + '_' : '') + refreshTimeData + '.tar.gz';
                if (!localStorage.getItem('fileName')) {
                    localStorage.setItem('fileName', fileName);
                }
                if (data !== 'init') {
                    this.homeService.oneKeyCollect().subscribe((url) => {
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

    private queryCollectProgress(fileName: string) {
        const url = sessionStorage.getItem('collectProgressURL');
        this.homeService.collectProgress(url).subscribe({
            next: (res) => {
                const {state, taskPercentage, downloadUrl} = res;
                    if (taskPercentage) {
                        this.homeService.sendMessage(taskPercentage);
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
            },
            error: (error) => {
                sessionStorage.removeItem('collectProgressURL');
                this.messageModal.close();
                if (error.error && getMessageId(error.error)?.length > 0) {
                    const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
                    this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                }
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
}
