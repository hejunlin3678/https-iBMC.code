import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { LocationUpgradeService } from '../services/location-upgrade.service';
import { TiMessageService, TiFileItem, TiTableRowData, TiTableSrcData, TiTableColumns } from '@cloud/tiny3';
import { Router } from '@angular/router';
import { UpgradeModel } from './upgrade-model';
import { UpgradeVersion } from '../../../model/upgrade-version';
import Date from 'src/app/common-app/utils/date';
import { GlobalDataService, CommonService, LoadingService, ErrortipService, AlertMessageService, UserInfoService, TimeoutService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';
import { PRODUCT } from 'src/app/common-app/service/product.type';
import { CommonData } from 'src/app/common-app/models';
import { UpgradeService } from '../../../services';


@Component({
    selector: 'app-location-upgrade',
    templateUrl: './location-upgrade.component.html',
    styleUrls: ['./location-upgrade.component.scss']
})
export class LocationUpgradeComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private upgradeService: LocationUpgradeService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private errorTipService: ErrortipService,
        private router: Router,
        private loadingService: LoadingService,
        private commonService: CommonService,
        private globalData: GlobalDataService,
        private timeoutService: TimeoutService,
        private upgradeVersionService: UpgradeService,
    ) { }

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;

    public mainBoardNpCardShow: boolean = false;
    public progressShow: boolean = false;
    public bmcUpdating: boolean = false;
    public firmBtnRest: boolean = false;
    public firmBtnBack: boolean = false;
    public startBtnDisabled: boolean = true;
    public canAddFile: boolean = true;
    public show: boolean = true;
    public upgrade = new UpgradeModel();
    public upgradeVersin = new UpgradeVersion();
    public closeIcon: boolean = false;
    public showAlert = {
        type: 'prompt',
        openAlert: true
    };
    public upgradeTip = '';
    public upgradeResult = '';
    public tipMessage = '';
    public maxValue: number = 100;
    public currentValue: number = 0;
    // 升级时，进度查询的失败次数
    public taskStateErrorAmout: number = 0;
    public networkErrorAmout: number = 0;
    private taskInterval: number = null;
    // 是否支持生效分离
    public upgradeActiveModeSupported: boolean = false;
    // 生效分离勾选状态
    public activeMode: boolean = false;
    public httpErrorStatus: number = 400;
    /**
     * 查询升级进度间隔时长
     * 为了能在OS下电状态下升级CPLD时查到升级完成的状态（升级完成到环境AC下电大概2秒左右），查询间隔设置为2秒
     */
    public PROGRESS_INTERVAL_TIME_MS: number = 2000;


    // 版本信息表格数据
    public searchWords = [];
    public searchKeys = ['name'];
    public displayed: TiTableRowData[] = [];
    private data: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public columns: TiTableColumns[] = [
        {
            title: this.getI18nValue('COMMON_NAME'),
            width: '35%',
            key: 'name',
            selected: null,
            options: [{
                label: this.getI18nValue('IBMC_MANUAL_FIRMWARE'),
                type: 'firmware'
            }, {
                label: this.getI18nValue('IBMC_MANUAL_SOFTWARE'),
                type: 'software'
            }, {
                label: this.getI18nValue('IBMC_MANUAL_DRUVING'),
                type: 'driver'
            }],
            multiple: true,
            selectAll: true
        },
        {
            title: this.getI18nValue('IBMC_VERSION_CURRENT'),
            width: '35%'
        }
    ];

    public inputFieldWidth = '360px';
    public certificatePassItem = {
        value: ''
    };
    public isUploading: boolean = false;
    public fileInstance = null;
    // 文件上传
    public uploaderConfig = {
        url: '/UI/Rest/BMCSettings/UpdateService/FirmwareUpdate',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        autoUpload: false,
        alias: 'Content',
        filters: [{
            name: 'type',
            params: ['.hpm']
        }, {
            name: 'maxCount',
            params: [1]
        }, {
            name: 'maxSize',
            params: [1024 * 1024 * 90]
        }],
        onAddItemFailed: ((fileItem) => {
            if (fileItem.validResults.indexOf('type') >= 0) {
                this.displayUpgradeState('failed', this.getI18nValue('IBMC_UP_FILE_FORMAT_ERROR'));
            } else if (fileItem.validResults.indexOf('maxSize') >= 0) {
                this.displayUpgradeState('failed', this.getI18nValue('IBMC_UPLOAD_FILE_1M_ERROR2'));
            }
            if (this.fileInstance) {
                this.fileInstance.remove();
            }
        }),
        onAddItemSuccess: (fileItem) => {
            this.fileInstance = fileItem;
            this.startBtnDisabled = false;
            this.canAddFile = false;
            this.displayUpgradeState('clear', '');
        },
        onRemoveItems: ($event) => {
            this.startBtnDisabled = true;
            this.canAddFile = true;
            this.fileInstance = null;
        },
        onBeforeSendItems: (fileItems) => {
            this.show = false;
            // 如果支持生效分离则加上ActiveMode参数
            if (this.upgradeActiveModeSupported) {
                fileItems.forEach((item: TiFileItem) => {
                    item.formData = {
                        ActiveMode: this.activeMode ? 'Immediately' : 'ResetBMC'
                    };
                });
            }
        },
        onSuccessItems: (fileItem: TiFileItem) => {
            this.isUploading = false;
            this.setUploadFileResult(true);
            this.queryTask();
        },
        onErrorItems: (event: { fileItems: TiFileItem; response: string; status: number }) => {
            if (this.fileInstance) {
                this.fileInstance.remove();
            }
            const res = event.response;
            let errorMessage = 'COMMON_FAILED';
            let errorId: string;
            if (getMessageId(res).length > 0) {
                errorId = getMessageId(res)[0]?.errorId;
                errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                if (errorId === 'NoValidSession') {
                    this.timeoutService.error401.next(errorId);
                }
            }
            // 文件名错误不需要提示右上角信息
            if (errorId !== 'FileNameError') {
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            }

            if (event.status > this.httpErrorStatus) {
                this.errorTipService.errorTip({
                    responseJSON: res
                });
            } else {
                const msg = errorId === 'FileNameError' ? errorMessage : this.getI18nValue('COMMON_FAILED');
                this.displayUpgradeState('failed', msg);
            }
            this.setUploadFileResult(false);
        }
    };

    // 获取国际化字段内容
    private getI18nValue(key: string): string {
        return this.translate.instant(key);
    }

    public initData(isInitted: boolean = true) {
        this.upgradeService.upgradeList().subscribe((res) => {
            this.upgrade = res[0];
            this.upgradeActiveModeSupported = this.upgrade.getActiveModeSupported;
            this.activeMode = this.upgrade.getActiveMode === 'Immediately' ? true : false;

            // 页面进入时，才查询一次升级进度
            if (this.upgrade.getUpdatingFlag && isInitted) {
                this.queryTask();
            }
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public getUpgradeTip(res) {
        switch (res) {
            case PRODUCT.IBMC:
                this.upgradeTip = this.translate.instant('IBMC_FIMWARE_TIP_INFO');
                break;
            case PRODUCT.IRM:
                this.upgradeTip = this.translate.instant('IRM_FIMWARE_TIP_INFO');
                break;
            case PRODUCT.PANGEA:
                this.upgradeTip = this.translate.instant('IBMC_FIMWARE_TIP_INFO');
                break;
            case PRODUCT.EMM:
                break;
        }
    }

    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.initData();
        this.getUpgradeTip(CommonData.productType);
    }

    // 固件升级的状态提示（成功，失败，原因）
    public displayUpgradeState(state: 'success' | 'failed' | 'clear', message: string): void {
        if (state === 'clear') {
            this.upgradeResult = '';
            this.tipMessage = '';
        } else {
            this.upgradeResult = state;
            this.tipMessage = message;
        }
    }

    // 开始升级
    public startToConfig = () => {
        const instanceBut = this.tiMessage.open({
            id: 'startUp',
            type: 'prompt',
            content: this.getI18nValue('IBMC_UP_COMM_FIRMWARE_BTN'),
            okButton: {
                show: true,
                text: this.getI18nValue('ALARM_OK'),
                autofocus: false,
                click: () => {
                    this.startBtnDisabled = true;
                    this.fileInstance.upload();
                    instanceBut.close();
                }
            },
            cancelButton: {
                show: true,
                text: this.getI18nValue('ALARM_CANCEL'),
                click(): void {
                    instanceBut.dismiss();
                }
            },
            title: this.getI18nValue('ALARM_OK'),
        });
    }

    // 文件上传成功或失败时的各变量状态
    public setUploadFileResult = (reuslt) => {
        this.startBtnDisabled = true;
        this.progressShow = reuslt;
        this.firmBtnRest = reuslt;
        this.firmBtnBack = reuslt;
        if (reuslt) {
            this.bmcUpdating = true;
        }
    }

    // 进度接口返回200，但是数据异常时
    public stateChangeCOfm = () => {
        this.progressShow = false;
        this.startBtnDisabled = true;
        this.firmBtnRest = false;
        this.firmBtnBack = false;
    }

    public taskCompleted = (taskData) => {
        this.clearQueryProgress();
        // 升级成功后，失败次数还原
        this.taskStateErrorAmout = 0;
        this.currentValue = 100;
        this.bmcUpdating = false;
        // 2分钟后，才隐藏进度条，以便自动化截图
        setTimeout(() => {
            this.progressShow = false;
        }, 120000);

        // 清空上传框会触发onRemove事件
        if (this.fileInstance) {
            this.fileInstance.remove();
        }

        const isBIOS = taskData.Component === 'BIOS';
        const isCPLD = taskData.Component.indexOf('CPLD') > -1;
        const isVRD = taskData.Component === 'Power Firmware';
        if (isBIOS || isCPLD || isVRD) {
            // BIOS,CPLD生效分离
            this.upgradeService.getPowerControl().subscribe(resData => {
                if (resData['body'].PowerState !== 'Off') {
                    if (isCPLD || isVRD) {
                        this.displayUpgradeState('success', this.getI18nValue('IBMC_AFTER_CPLD'));
                    } else {
                        this.displayUpgradeState('success', this.getI18nValue('IBMC_AFTER_REBOOT'));
                    }
                } else {
                    this.displayUpgradeState('success', this.getI18nValue('IBMC_FIMWARE_UPDATE_FIND'));
                }
            }, () => {
                this.displayUpgradeState('success', this.getI18nValue('IBMC_FIMWARE_UPDATE_FIND'));
            });
        } else {
            this.displayUpgradeState('success', this.getI18nValue('IBMC_FIMWARE_UPDATE_FIND'));
        }
        this.firmBtnRest = false;
        this.firmBtnBack = false;
        this.startBtnDisabled = true;
        this.canAddFile = true;
        this.upgradeVersionService.upgrade.setFirmBtnRest(false);
        this.upgradeVersionService.upgrade.setFirmBtnBack(false);

        setTimeout(() => {
            this.initData(false);
        }, this.PROGRESS_INTERVAL_TIME_MS);
    }

    public taskRunning = (taskData) => {
        this.reQueryProgress();
        // 接口成功后，重新统计失败次数
        this.taskStateErrorAmout = 0;
        this.progressShow = true;
        this.bmcUpdating = true;
        this.currentValue = taskData.Percentage.split('%')[0];
        this.firmBtnRest = true;
        this.firmBtnBack = true;
        this.startBtnDisabled = true;
        this.canAddFile = false;
        this.upgradeVersionService.upgrade.setFirmBtnRest(true);
        this.upgradeVersionService.upgrade.setFirmBtnBack(true);
    }

    public taskOtherCases = (taskData) => {
        if (this.taskStateErrorAmout > 10) {
            const state = (taskData && taskData.TaskState) ? taskData.TaskState : 'TaskState is not valid.';
            const errorMsg = 'type error, the current task state value is ：' + state;
            this.recordUpgradeErrorLogs(errorMsg);

            // 执行升级失败流程
            this.executeUpgradeFailed(taskData.ErrorCode);
            return;
        }

        // 出现状态不对时, 重试次数为10次
        this.taskStateErrorAmout++;
        this.reQueryProgress();
    }

    // 固件升级进度
    public queryTask = () => {
        this.upgradeService.queryTaskProgress().subscribe((res) => {
            this.networkErrorAmout = 0;
            const taskData = res['body'];
            if (taskData.TaskState === 'Completed') {
                this.taskCompleted(taskData);
            } else if (taskData.TaskState === 'Running') {
                this.taskRunning(taskData);
            } else {
                this.taskOtherCases(taskData);
            }
        }, () => {
            const errorMsg = 'network connect error!';
            this.recordUpgradeErrorLogs(errorMsg);

            // 当接口查询进度报错时，考虑正常升级时间预计是3分钟左右，设置为30次
            if (this.networkErrorAmout > 30) {
                return;
            }
            this.networkErrorAmout++;
            this.reQueryProgress();
        });
    }

    // 记录升级失败日志
    private recordUpgradeErrorLogs(errorMsg: string): void {
        const upgradeError = JSON.parse(localStorage.getItem('upgradeError')) || {};
        const curTime = new Date().pattern('yyyy-MM-dd HH:mm:ss');
        upgradeError[curTime] = errorMsg;
        localStorage.setItem('upgradeError', JSON.stringify(upgradeError));
    }

    // 重新触发进度查询
    private reQueryProgress(): void {
        this.taskInterval = window.setTimeout(() => {
            this.queryTask();
        }, this.PROGRESS_INTERVAL_TIME_MS);
    }

    // 清除定时查询任务
    private clearQueryProgress(): void {
        if (this.taskInterval) {
            clearTimeout(this.taskInterval);
            this.taskInterval = null;
        }
    }

    private executeUpgradeFailed(errorCode: number): void {
        // 清空上传框
        if (this.fileInstance) {
            this.fileInstance.remove();
        }

        let message = this.getI18nValue('IBMC_FIMWARE_FAILED');
        if (errorCode === 3) {
            message = this.getI18nValue('IBMC_UP_COMM_FILE_ERROR');
            this.stateChangeCOfm();
        } else if (errorCode === 2) {
            message = this.getI18nValue('IBMC_UP_FILE_NOT_EXIST');
            this.stateChangeCOfm();
        } else if (errorCode === 4) {
            message = this.getI18nValue('IBMC_UP_COMM_LITTLE_MEM');
            this.stateChangeCOfm();
        } else if (errorCode === 8) {
            message = this.getI18nValue('IBMC_UP_OS_POWER_ERROR');
            this.stateChangeCOfm();
        } else if (errorCode === 9) {
            message = this.getI18nValue('IBMC_UP_FIRMWARE_MISMATCH');
            this.stateChangeCOfm();
        } else if (errorCode === 12) {
            message = this.getI18nValue('IBMC_UP_FIRMWARE_FILE_MISMATCH');
            this.stateChangeCOfm();
        } else if (errorCode === 13) {
            message = this.getI18nValue('IBMC_UP_FIRMWARE_ERR_WRONG_ME_STATUS');
            this.stateChangeCOfm();
        } else if (errorCode === 27) {
            message = this.getI18nValue('IBMC_UP_WHITEBRAND_FILE_MISMATCH');
            this.stateChangeCOfm();
        } else if (errorCode === 28) {
            message = this.getI18nValue('IBMC_UP_FIRMWARE_ERR_RETRY');
            this.stateChangeCOfm();
        } else if (errorCode === 70) {
            message = this.getI18nValue('IBMC_UP_COMM_FILE_TOO_EARLY');
            this.stateChangeCOfm();
        } else {
            message = this.getI18nValue('IBMC_FIMWARE_FAILED');
            this.stateChangeCOfm();
        }
        this.displayUpgradeState('failed', message);
    }

    ngOnDestroy(): void {
        this.clearQueryProgress();
    }
}
