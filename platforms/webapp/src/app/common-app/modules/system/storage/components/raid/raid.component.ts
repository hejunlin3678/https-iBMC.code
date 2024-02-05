import { Component, OnInit } from '@angular/core';
import { TiModalService, TiModalRef, TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { RaidService } from './raid.service';
import { RaidEditComponent } from './raid-edit/raid-edit.component';
import { StorageTree, Raid, RaidNode, RaidEditModel, RaidType } from '../../models';
import { HomeService } from 'src/app/bmc-app/modules/nav/home/services';
import { LoadingService, UserInfoService, SystemLockService, AlertMessageService, ErrortipService, CommonService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';
import { StorageCollectLogService } from 'src/app/common-app/service/storage-collectlog.service';

@Component({
    selector: 'app-raid',
    templateUrl: './raid.component.html',
    styleUrls: ['./raid.component.scss']
})
export class RaidComponent implements OnInit {

    public raid: Raid;
    private raidNode: RaidNode;

    public isShowSetting: boolean = false;
    public systemLocked: boolean = true;
    public isShowCollectBtn: boolean = false;
    // 是否处于一键收集状态
    public startCollectingLogs: boolean = false;
    // 收集按钮显示查看进度还是收集日志
    public collectLogTxt = 'STORE_COLLECTING_LOGS';
    public percentValue: string = '';
    public timers = [];

    constructor(
        private raidService: RaidService,
        private loading: LoadingService,
        private tiModal: TiModalService,
        private i18n: TranslateService,
        public user: UserInfoService,
        private homeService: HomeService,
        private tiMessage: TiMessageService,
        private commonService: CommonService,
        private lockService: SystemLockService,
        private alert: AlertMessageService,
        private errorTipService: ErrortipService,
        private storageCollectLogService: StorageCollectLogService
    ) { }

    ngOnInit() {
        this.raidNode = StorageTree.getInstance().getCheckedNode as RaidNode;
        this.showIsCollecting();
        if (this.raidNode.getRaid) {
            this.raid = this.raidNode.getRaid;
            this.getConllectBtn();
            this.isShowSetting = this.settingShow();
        } else {
            this.factory();
        }
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
    }
    ngOnDestroy() {
        this.timers.forEach((item) => {
            clearTimeout(item);
        });
    }

    public getCollectUrl() {
        const collectProgressURL = sessionStorage.getItem('raidCollectProgressURL');
        if (collectProgressURL) {
            const obj = JSON.parse(collectProgressURL);
            const id = this.raidNode.id;
            return obj[id];
        }
        return null;
    }

    public showIsCollecting() {
        const collectLogUrl =  this.getCollectUrl();
        if (collectLogUrl) {
            this.oneKeyCollect('init');
        }
    }

    public getConllectBtn() {
        const conllectCard = this.raidNode.getRaid?.getBandManag &&
            (this.raidNode.getRaidType === RaidType.ARIES || this.raidNode.getRaidType === RaidType.PMC);
        if (conllectCard) {
            this.isShowCollectBtn = true;
            const id = this.raidNode.labelId;
            this.startCollectingLogs = this.storageCollectLogService.getCollectedState(id) || false;
            this.storageCollectLogService.changeState.subscribe((res) => {
                if (res.key === id) {
                    this.startCollectingLogs = res.value;
                }
            });
        }
    }

    private factory() {
        this.loading.state.next(true);
        this.raidService.factory(this.raidNode, this.raidNode.getRaidType === RaidType.ARIES).subscribe(
            (node: Raid) => {
                node.setEpdSupported = this.raidNode.getEpdSupported;
                node.setJbodStateSupported = this.raidNode.getJbodStateSupported;
                this.raid = node;
                this.raidNode.setRaid = this.raid;
                this.getConllectBtn();
                this.isShowSetting = this.settingShow();
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

    show(): void {
        this.tiModal.open(RaidEditComponent, {
            id: 'raidEdit',
            modalClass: 'raid-edit-class',
            context: {
                raid: this.raid,
                raidNode: this.raidNode
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                RaidEditModel.destroy();
                modalRef.destroy(false);
                // 保存按钮触发，重新请求更新raid
                if (reason) {
                    this.factory();
                }
            },
            close: (modalRef: TiModalRef): void => {
            },
            dismiss: (modalRef: TiModalRef): void => {
            }
        });
    }

    showCollectMsg() {
        if (this.collectLogTxt === 'STORE_COLLECTING_LOGS') {
            const msgInstance = this.tiMessage.open({
                id: 'oneKeyCollect',
                type: 'warn',
                okButton: {
                    text: this.i18n.instant('COMMON_OK'),
                    autofocus: false,
                    click: () => {
                        this.oneKeyCollect('click');
                        msgInstance.close();
                    }
                },
                cancelButton: {
                    text: this.i18n.instant('COMMON_CANCEL')
                },
                title: this.i18n.instant('COMMON_CONFIRM'),
                content: this.i18n.instant('STORE_COLLECTING_CONFIRM')
            });
        } else {
            this.showIsCollecting();
        }
    }
    private oneKeyCollect(data: string) {
        const fileName = this.raidNode.label + '.tar.gz';
        const collectUrl = this.raidNode.getUrl;
        if (!localStorage.getItem('raidFileName')) {
            localStorage.setItem('raidFileName', fileName);
        }
        if (data !== 'init') {
            this.raidService.oneKeyCollect(fileName, collectUrl).subscribe(
                (url) => {
                    const str = sessionStorage.getItem('raidCollectProgressURL');
                    if (str) {
                        const obj = JSON.parse(str);
                        obj[this.raidNode.id] = url;
                        const str2 = JSON.stringify(obj);
                        sessionStorage.setItem('raidCollectProgressURL', str2);
                    } else {
                        const obj: any = {};
                        obj[this.raidNode.id] = url;
                        const str2 = JSON.stringify(obj);
                        sessionStorage.setItem('raidCollectProgressURL', str2);
                    }
                    this.queryCollectProgress(fileName);
                },
                (err) => {
                    const collectLogUrl =  this.getCollectUrl();
                    if (collectLogUrl && getMessageId(err.error)[0].errorId.indexOf('DuplicateExportingErr') > -1) {
                        this.queryCollectProgress(fileName);
                    }
                }
            );
        } else {
            const collectLogUrl =  this.getCollectUrl();
            if (collectLogUrl) {
                this.queryCollectProgress(fileName);
            }
        }
    }
    private queryCollectProgress(fileName: string) {
        const labelId = this.raidNode.labelId;
        const collectLogUrl =  this.getCollectUrl();
        this.homeService.collectProgress(collectLogUrl).subscribe(
            ({ state, taskPercentage, downloadUrl }) => {
                if (taskPercentage) {
                    this.homeService.sendMessage(taskPercentage);
                    this.percentValue = taskPercentage;
                }
                if (taskPercentage === '100%' && state === 'WAIT_TRANSFER') {
                    const str = sessionStorage.getItem('raidCollectProgressURL');
                    if (str) {
                        const obj = JSON.parse(str);
                        delete obj[this.raidNode.id];
                        const str2 = JSON.stringify(obj);
                        sessionStorage.setItem('raidCollectProgressURL', str2);
                    }
                    localStorage.removeItem('raidFileName');
                    this.commonService.oneKeyDownloadFile(downloadUrl, fileName);
                    const pecentTimer = setTimeout(() => {
                        this.collectLogTxt = 'STORE_COLLECTING_LOGS';
                        this.percentValue = '';
                        this.storageCollectLogService.changeState.next({ key: labelId, value: false });
                    }, 1000);
                    const showResultTimer = setTimeout(() => {
                        this.showSuccessMessage();
                    }, 3000);
                    this.timers.push(pecentTimer, showResultTimer);
                } else {
                    this.storageCollectLogService.changeState.next({ key: labelId, value: true });
                    this.collectLogTxt = 'STORE_COLLECTING_LOGS_VIEW';
                    const progressTimer = setTimeout(() => {
                        this.queryCollectProgress(fileName);
                    }, 2000);
                    this.timers.push(progressTimer);
                }
            },
            (error) => {
                this.storageCollectLogService.changeState.next({ key: labelId, value: false });
                this.collectLogTxt = 'STORE_COLLECTING_LOGS';
                const str = sessionStorage.getItem('raidCollectProgressURL');
                if (str) {
                    const obj = JSON.parse(str);
                    delete obj[this.raidNode.id];
                    const str2 = JSON.stringify(obj);
                    sessionStorage.setItem('raidCollectProgressURL', str2);
                }
                localStorage.removeItem('raidFileName');
                const errorId = getMessageId(error.error)[0].errorId;
                const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            }
        );
    }
    private showSuccessMessage() {
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
            title: this.i18n.instant('HOME_PROMPTMESSAGE'),
            content: this.i18n.instant('COMMON_SUCCESS')
        });
    }

    private settingShow(): boolean {
        if (this.raid.getChidrenData) {
            const isBRCM = this.raidNode.getRaidType === RaidType.BRCM;
            const isARIES = this.raidNode.getRaidType === RaidType.ARIES;
            const isPMC = this.raidNode.getRaidType === RaidType.PMC;
            const isHBA = this.raidNode.getRaidType === RaidType.HBA;
            // 是否支持带外管理
            const oobSupport = this.raid.getChidrenData.oobSupport || false;
            // 逻辑盘列表
            const volumeNodeList = this.raidNode.children?.filter((item) => item.componentName === 'VolumeNode') || [];
            if (isBRCM || isARIES || isHBA) {
                return oobSupport;
            }

            // 设置按钮显示:1.若PMC卡不支持工作模式的场景隐藏（PMC卡目前只支持工作模式）2.有逻辑盘且模式不能是HBA
            if (isPMC) {
                const mode = this.raid.getSupportedModes.options;
                return mode !== null && oobSupport;
            }
        }

        return false;
    }
}
