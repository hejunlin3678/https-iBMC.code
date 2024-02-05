import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { AlertMessageService, CommonService, HttpService } from 'src/app/common-app/service';
import { getMessageId, getRandomFileName } from 'src/app/common-app/utils/common';
import { SystemLogService } from '../../services';
import { TiMessageService } from '@cloud/tiny3';

@Component({
    selector: 'app-box',
    templateUrl: './box.component.html',
    styleUrls: ['./box.component.scss']
})
export class BoxComponent implements OnInit {
    public down: boolean = false;
    public isSecondEntry: boolean = false;
    public percentValue: number = 0;
    public percent: string = '0%';
    public sdiProgress: string = '0%';
    public isCollectingErr: boolean = false;
    public npuLogProgressUrl: string = sessionStorage.getItem('npuLogProgressUrl');
    public npuLogProgressItem: any = JSON.parse(sessionStorage.getItem('npuLogProgressItem'));
    public taskInterval: number = Number(localStorage.getItem('taskInterval'));
    public sdiLogProgressUrl: string = localStorage.getItem('sdiLogProgressUrl');
    public sdiLogProgressItem: any = JSON.parse(localStorage.getItem('sdiLogProgressItem'));
    @Input() systemInfo;
    @Input() isSystemLock;
    @Input() npuCards;
    constructor(
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private http: HttpService,
        private alert: AlertMessageService,
        private service: SystemLogService,
        private commonService: CommonService
    ) {}

    ngOnInit() {
        if (this.npuLogProgressUrl) {
            this.isSecondEntry = true;
            this.taskService(this.npuLogProgressUrl, this.npuLogProgressItem, '');
        }
        if (this.sdiLogProgressUrl) {
            if (this.taskInterval) {
                clearTimeout(this.taskInterval);
                this.taskInterval = null;
            }
            this.http.keepAlive('Activate');
            this.taskService(this.sdiLogProgressUrl, this.sdiLogProgressItem, 'sdi');
        }
    }
    public change(item) {
        const par = {};
        if (item.id === 'blackBox') {
            par['BlackBoxEnabled'] = item.status;
        } else if (item.id === 'serialPort') {
            par['SerialPortDataEnabled'] = item.status;
        }
        this.service.updateSwitchState(par).subscribe(
            () => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            },
            () => {
                item.status = !item.status;
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            }
        );
    }
    public changePCIe(item) {
        const par = {};
        par['PCIeInterfaceEnabled'] = item.statusPCIe;
        this.service.updateSwitchState(par).subscribe(
            () => {
                if (item.statusPCIe) {
                    item.disable = false;
                } else {
                    item.disable = true;
                    item.status = false;
                }
                this.alert.eventEmit.emit({ type: 'success', label: 'SYSTEM_LOG_PCIE_TIP' });
            },
            () => {
                item.statusPCIe = !item.statusPCIe;
                this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            }
        );
    }
    public download(item, key) {
        const id = item.id;
        if (id === 'blackBox' && key === 'sdi') {
            // 下载二次弹框提示
            const upgradeSpModel = this.tiMessage.open({
                id: 'upgradeSpModel',
                type: 'prompt',
                okButton: {
                    show: true,
                    autofocus: false,
                    click: () => {
                        this.http.keepAlive('Activate');
                        upgradeSpModel.close();
                        if (item.statusPCIe) {
                            if (item.downSid) {
                                return;
                            }
                            item.downSid = true;
                            const url = '/UI/Rest/Maintenance/DownloadBlackBoxOfSdi';
                            const fileName = getRandomFileName(item.fileNameSid);
                            this.service.downloadLog(url).subscribe(
                                (data) => {
                                    const response = data.body;
                                    item.showSdiProgress = true;
                                    localStorage.setItem('sdiLogProgressUrl', response.url);
                                    localStorage.setItem('sdiLogProgressItem', JSON.stringify(item));
                                    this.taskService(response.url, item, key);
                                },
                                () => {
                                    item.downSid = false;
                                    item.showSdiProgress = false;
                                }
                            );
                        } else {
                            this.alert.eventEmit.emit({ type: 'error', label: 'SYSTEM_LOG_BLACKCLIP_SID' });
                        }
                    }
                },
                cancelButton: {
                    show: true,
                    text: this.translate.instant('COMMON_CANCEL'),
                    click: () => {
                        upgradeSpModel.close();
                    }
                },
                title: this.translate.instant('COMMON_CONFIRM'),
                content: `<div>
                <p class='accessResetModal'>
                ${this.translate.instant('SYSTEM_SID_DOWN_TIP1')}</p>
                <p>${this.translate.instant('SYSTEM_SID_DOWN_TIP2')}</p>
                <p>${this.translate.instant('SYSTEM_SID_DOWN_TIP3')}</p>
                <p>${this.translate.instant('SYSTEM_SID_DOWN_TIP4')}</p>
                </div>`
            });
        } else {
            if (item.status) {
                if (item.down) {
                    return;
                }
                item.down = true;
                let url = '';
                let npuId = 0;
                if (id === 'blackBox') {
                    url = '/UI/Rest/Maintenance/SystemDiagnostic/DownloadBlackBox';
                } else if (id === 'serialPort') {
                    url = '/UI/Rest/Maintenance/SystemDiagnostic/DownloadSerialPortData';
                } else if (id === 'NPU') {
                    url = '/UI/Rest/Maintenance/SystemDiagnostic/DownloadNpuLog';
                    npuId = Number(item.title.slice(3));
                }
                if (id === 'NPU') {
                    this.service.downloadNpuLog(url, npuId).subscribe(
                        (data) => {
                            const response = data.body;
                            item.showProgress = true;
                            sessionStorage.setItem('npuLogProgressUrl', response.url);
                            sessionStorage.setItem('npuLogProgressItem', JSON.stringify(item));
                            this.taskService(response.url, item, '');
                        },
                        () => {
                            item.down = false;
                        }
                    );
                } else {
                    this.service.downloadLog(url).subscribe(
                        (data) => {
                            const response = data.body;
                            this.taskService(response.url, item, '');
                        },
                        () => {
                            item.down = false;
                        }
                    );
                }
            } else {
                if (id === 'blackBox') {
                    this.alert.eventEmit.emit({ type: 'error', label: 'SYSTEM_LOG_BLACKCLIP_MEG' });
                } else if (id === 'serialPort') {
                    this.alert.eventEmit.emit({ type: 'error', label: 'SYSTEM_LOG_SERIAL_PORT_MEG' });
                }
            }
        }
    }
    public taskService(url, item, key) {
        this.service.getTaskStatus(url).subscribe(
            (res) => {
                const response = res.body;
                if (item.id === 'NPU') {
                    this.percent = response.prepare_progress ? response.prepare_progress + '%' : '0%';
                    this.percentValue = response.prepare_progress;
                }
                if (item.id === 'blackBox' && key === 'sdi' && response.prepare_progress <= 100) {
                    this.systemInfo[0].downSid = true;
                    this.systemInfo[0].showSdiProgress = true;
                    this.sdiProgress = response.prepare_progress ? response.prepare_progress + '%' : '0%';
                }
                if (response.prepare_progress === 100 && response.downloadurl) {
                    if (item.id === 'NPU') {
                        item.isCollectingOk = true;
                        this.clearTask(item);
                        if (this.isSecondEntry) {
                            this.npuCards.forEach((npu) => {
                                if (npu.title === item.title) {
                                    npu.isCollectingOk = true;
                                    this.clearTask(npu);
                                }
                            });
                            return false;
                        }
                    }

                    // SDI黑匣子下载完成，清除会话存储
                    if (item.id === 'blackBox' && key === 'sdi') {
                        this.clearTaskSDI(item);
                    }

                    this.commonService.restDownloadFile(response.downloadurl).then(
                        () => {
                            if (item.id === 'blackBox' && key === 'sdi') {
                                this.systemInfo[0].downSid = false;
                                this.systemInfo[0].showSdiProgress = false;
                                this.sdiProgress = '0%';
                            } else {
                                item.down = false;
                            }
                            this.alert.eventEmit.emit({ type: 'success', label: 'DOWNLOAD_SUCCESS' });
                        },
                        () => {
                            if (item.id === 'blackBox' && key === 'sdi') {
                                this.systemInfo[0].downSid = false;
                                this.systemInfo[0].showSdiProgress = false;
                                this.sdiProgress = '0%';
                            } else {
                                item.down = false;
                            }
                            this.alert.eventEmit.emit({ type: 'error', label: 'DOWNLOAD_FAILED' });
                        }
                    );
                } else {
                    if (item.id === 'blackBox' && key === 'sdi') {
                        this.taskInterval = window.setTimeout(() => {
                            this.http.keepAlive('Activate');
                            this.taskService(url, item, key);
                        }, 10000);
                        localStorage.setItem('taskInterval', this.taskInterval.toString());
                    } else {
                        setTimeout(() => {
                            this.taskService(url, item, key);
                        }, 500);
                    }
                }
            },
            (error) => {
                if (item.id === 'blackBox' && key === 'sdi') {
                    this.clearTaskSDI(item);
                    this.systemInfo[0].downSid = false;
                    this.systemInfo[0].showSdiProgress = false;
                    this.sdiProgress = '0%';
                } else {
                    item.down = false;
                }
                if (item.id === 'NPU') {
                    item.isCollectingErr = true;
                    this.clearTask(item);
                }
                const errorTip = getMessageId(error.error);
                const errorId = errorTip.length > 0 ? errorTip[0].errorId : 'DOWNLOAD_FAILED';
                this.alert.eventEmit.emit({
                    type: 'error',
                    label: this.translate.instant(errorId)['errorMessage'] || 'DOWNLOAD_FAILED'
                });
            }
        );
    }

    public clearTask(item) {
        sessionStorage.removeItem('npuLogProgressUrl');
        sessionStorage.removeItem('npuLogProgressItem');
        setTimeout(() => {
            this.percentValue = 0;
            this.percent = '0%';
            item.showProgress = false;
            item.isCollectingOk = false;
            item.isCollectingErr = false;
            item.down = false;
        }, 3000);
    }

    public clearTaskSDI(item) {
        localStorage.removeItem('sdiLogProgressUrl');
        localStorage.removeItem('sdiLogProgressItem');
        localStorage.removeItem('taskInterval');
    }
}
