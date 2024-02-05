import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UpgradeService } from './services';
import { TiMessageService, TiFileItem } from '@cloud/tiny3';
import { EmmInfo, UpgradeModel } from './model';
import Date from 'src/app/common-app/utils/date';
import { LoadingService, ErrortipService, AlertMessageService, UserInfoService, TimeoutService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-upgrade',
    templateUrl: './upgrade.component.html',
    styleUrls: ['./upgrade.component.scss']
})
export class UpgradeComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private upgradeService: UpgradeService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService,
        private timeoutService: TimeoutService,
    ) { }

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;
    public progressShow: boolean = false;
    public bmcUpdating: boolean = false;
    public startBtnDisabled: boolean = true;
    public canAddFile: boolean = true;
    public upgrade = new UpgradeModel();
    public closeIcon: boolean = false;
    public upgradeResult = '';
    public tipMessage = '';
    public maxValue: number = 100;
    public currentValue: number = 0;
    // 升级时，进度查询的失败次数
    public taskStateErrorAmout: number = 0;
    public networkErrorAmout: number = 0;
    private taskInterval: number = null;

    public closeOthers: boolean = false;
    public showLCDandFan: number;
    public inputFieldWidth = '360px';
    public isUploading: boolean = false;
    public fileInstance = null;
    // 文件上传
    public uploaderConfig = {
        url: '/redfish/v1/UpdateService/FirmwareInventory',
        headers: {
            From: 'WebUI',
            'Token': this.user.getToken(),
            Accept: '*/*'
        },
        type: 'POST',
        autoUpload: false,
        alias: 'imgfile',
        filters: [{
            name: 'type',
            params: ['.hpm']
        }, {
            name: 'maxCount',
            params: [1]
        }, {
            name: 'maxSize',
            params: [1024 * 204800]
        }],
        onAddItemFailed: ((fileItem) => {
            if (fileItem.validResults.indexOf('type') >= 0) {
                this.displayUpgradeState('failed', this.getI18nValue('IBMC_UP_FILE_FORMAT_ERROR'));
            } else if (fileItem.validResults.indexOf('maxSize') >= 0) {
                this.displayUpgradeState('failed', this.getI18nValue('COMMON_UPLOAD_FILE_MORN_1M_ERROR2'));
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
        onSuccessItems: (fileItem: TiFileItem) => {
            this.isUploading = false;
            const filrUrl = '/tmp/web/' + fileItem['fileItems'][0]?._file?.name;
            this.upgradeService.upgradeHmm(filrUrl).subscribe((response) => {
                this.setUploadFileResult(true);
                const tasksUrl = response?.body['@odata.id'];
                const tasksId = tasksUrl?.split('/')?.pop();
                this.queryTask(tasksId);
            }, (error) => {
                if (this.fileInstance) {
                    this.fileInstance.remove();
                }
                this.setUploadFileResult(false);
                const errorId = getMessageId(error?.error)[0].errorId;
                const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            });
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
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            if (event.status > 400) {
                this.errorTipService.errorTip({
                    responseJSON: res
                });
            } else {
                const msg = this.getI18nValue('COMMON_FAILED');
                this.displayUpgradeState('failed', msg);
            }
            this.setUploadFileResult(false);
        }
    };

    // 获取国际化字段内容
    private getI18nValue(key: string): string {
        return this.translate.instant(key);
    }
    public initData() {
        // 页面进入时判断查询升级进度
        this.upgradeService.getUpdateInfo().subscribe((res) => {
            if (res['body']?.Task && res['body'].Task['@odata.id']) {
                const tasksUrl = res['body']?.Task['@odata.id'];
                const tasksId = tasksUrl?.split('/')?.pop();
                this.queryTask(tasksId);
            }
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.TCEtoMorePage();
        this.initData();
    }

    // E9000才显示风扇和LCD列表
    public TCEtoMorePage() {
        this.upgradeService.queryTCEType().subscribe((res) => {
            if (res['body'].Model === EmmInfo.TCE8040 || res['body'].Model === EmmInfo.TCE8080) {
                this.showLCDandFan = 1;
            } else if (res['body'].Model === EmmInfo.E9000) {
                this.showLCDandFan = 0;
            }
        });
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
        if (reuslt) {
            this.bmcUpdating = true;
        }
    }

    // 进度接口返回200，但是数据异常时
    public stateChangeCOfm = () => {
        this.progressShow = false;
        this.startBtnDisabled = true;
    }

    // 固件升级进度
    public queryTask(tasksId) {
        this.upgradeService.queryTaskProgress(tasksId).subscribe((res) => {
            this.networkErrorAmout = 0;
            const taskData = res['body'];
            const taskPercentage = this.upgradeService.getOemData(taskData)?.TaskPercentage?.split('%')[0] || 0;
            if (taskData?.TaskState === 'Completed') {
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
                this.displayUpgradeState('success', this.getI18nValue('IBMC_FIMWARE_UPDATE_FIND'));
                this.startBtnDisabled = true;
                this.canAddFile = true;
            } else if (taskData?.TaskState === 'Running') {
                this.reQueryProgress(tasksId);
                // 接口成功后，重新统计失败次数
                this.taskStateErrorAmout = 0;
                this.progressShow = true;
                this.bmcUpdating = true;
                this.currentValue = taskPercentage;
                this.startBtnDisabled = true;
                this.canAddFile = false;
            } else {
                if (this.taskStateErrorAmout > 10) {
                    const state = (taskData && taskData?.TaskState) ? taskData?.TaskState : 'TaskState is not valid.';
                    const errorMsg = 'type error, the current task state value is ：' + state;
                    this.recordUpgradeErrorLogs(errorMsg);

                    // 执行升级失败流程
                    this.executeUpgradeFailed(taskData?.ErrorCode);
                    return;
                }

                // 出现状态不对时, 重试次数为10次
                this.taskStateErrorAmout++;
                this.reQueryProgress(tasksId);
            }
        }, () => {
            const errorMsg = 'network connect error!';
            this.recordUpgradeErrorLogs(errorMsg);

            // 当接口查询进度报错时，考虑正常升级时间预计是3分钟左右，设置为30次
            if (this.networkErrorAmout > 30) {
                return;
            }
            this.networkErrorAmout++;
            this.reQueryProgress(tasksId);
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
    private reQueryProgress(tasksId): void {
        this.taskInterval = window.setTimeout(() => {
            this.queryTask(tasksId);
        }, 3000);
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
