import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import {
    UserInfoService,
    AlertMessageService,
    ErrortipService,
    LoadingService,
    CommonService,
    SystemLockService,
    TimeoutService
} from 'src/app/common-app/service';
import { ConfigUpgradeService } from './services';
import { TiMessageService, TiFileItem, TiModalRef, TiModalService } from '@cloud/tiny3';
import { Router } from '@angular/router';
import { getByte, getMessageId } from 'src/app/common-app/utils';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT } from 'src/app/common-app/service/product.type';

@Component({
    selector: 'app-config-upgrade',
    templateUrl: './config-upgrade.component.html',
    styleUrls: ['../manager.component.scss', './config-upgrade.component.scss']
})
export class ConfigUpgradeComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private configUpgradeService: ConfigUpgradeService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private errorTipService: ErrortipService,
        private commonService: CommonService,
        private router: Router,
        private loading: LoadingService,
        private lockService: SystemLockService,
        private timeoutService: TimeoutService,
        private modal: TiModalService
    ) { }

    public isSystemLock: boolean = false;
    public importBtnDisable: boolean = true;
    public exportBtnDisable: boolean = false;
    public configFinish: boolean = true;
    public upgradeResult: string = '';
    public tipMessage: string = '';
    public fileName: string = '';
    public fileInstance = null;
    public file: TiFileItem;
    public fileMes: string = '';
    public addFileBtn: boolean = true;
    public exporting: boolean = false;
    private modalInstance: TiModalRef = null;
    private formData: FormData = null;
    public configUpgradeTip: string = '';
    public isIrm: boolean = false;

    public showAlert = {
        type: 'prompt',
        openAlert: true,
        closeIcon: false
    };
    public importProgress = {
        value: 0,
        maxValue: 100,
        show: false,
        importFail: false
    };
    public exportProgress = {
        value: 0,
        maxValue: 100,
        show: false,
        exportFail: false
    };
    public uploadFileDisable: boolean = this.user.privileges.indexOf('ConfigureUsers') < 0;
    public configImport = {
        url: '/UI/Rest/BMCSettings/Configuration/ImportConfig',
        headers: {
            From: 'WebUI',
            'X-CSRF-Token': this.user.getToken()
        },
        type: 'POST',
        autoUpload: false,
        alias: 'Content',
        filters: [{
            name: 'type',
            params: ['.xml']
        }, {
            name: 'maxCount',
            params: [1]
        }, {
            name: 'maxSize',
            params: [1024 * 1024]
        }],
        onAddItemFailed: ((fileItem) => {
            this.fileName = '';
            this.importProgress.show = false;
            this.importProgress.importFail = false;
            if (fileItem.validResults.indexOf('type') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: this.translate.instant('CONFIG_UPGRADE_FILETYPE_ERROR') });
            } else if (fileItem.validResults.indexOf('maxSize') >= 0) {
                this.alert.eventEmit.emit({ type: 'error', label: this.translate.instant('CONFIG_UPGRADE_FILESIZE_1M') });
            }
            if (this.fileInstance) {
                this.fileInstance.remove();
            }
        }),
        onAddItemSuccess: (fileItem) => {
            this.fileInstance = fileItem;
            const length = getByte(fileItem?._file?.name);
            // linux的限制长度255减去/tmp/web/的固定长度，前端限制246
            if (length > 246) {
                this.fileInstance?.remove();
                this.alert.eventEmit.emit({ type: 'error', label: this.translate.instant('CONFIG_UPGRADE_FILE_NAME') });
                return;
            }
            this.importBtnDisable = false;
            this.importProgress.show = false;
            this.importProgress.importFail = false;
            this.fileName = fileItem._file.name;
            this.addFileBtn = false;

            // 封装formData数据
            const formData = new FormData();
            formData.append('Content', fileItem._file, fileItem.file.name);
            formData.append('Type', 'URI');
            this.formData = formData;
        },
        onRemoveItems: ($event) => {
            this.importBtnDisable = true;
            this.fileName = '';
            this.addFileBtn = true;
            this.formData = null;
        }
    };

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    // 导入配置
    public startToImport() {
        this.openModal();
    }

    private openModal() {
        // 账号密码登录
        if (!this.user.loginWithoutPassword) {
            this.modalInstance = this.modal.open(SecondPasswordComponent, {
                id: 'configUpdateModal',
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

                    // 调用方法
                    if (this.formData) {
                        this.formData.append('ReauthKey', window.btoa(modalRef.content.instance.password));
                        this.uploadFile();
                        this.fileInstance.mouseenter = undefined;
                    }
                }
            });
        } else {
            // 调用方法
            if (this.formData) {
                this.uploadFile();
                this.fileInstance.mouseenter = undefined;
            }
        }
    }

    // 导入操作加入了二次认证，因此不能使用组件的自动上传功能，需手动使用 formData 导入
    private uploadFile() {
        this.configUpgradeService.uploadFile(this.formData).subscribe({
            next: () => {
                this.getTask();
                this.importProgress.show = true;

                // 文件上传成功后，重置模态框对象
                this.modalInstance?.destroy(true);
                this.modalInstance = null;
                this.formData = null;
            },
            error: (error) => {
                const errorIds = getMessageId(error.error);
                let errorMessage = '';
                if (errorIds.length > 0) {
                    const errorId = errorIds[0]?.errorId;
                    errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                    if (errorId === 'ReauthFailed') {
                        if (this.modalInstance) {
                            this.modalInstance.content.instance.state = true;
                        }
                        return;
                    }

                    if (errorId === 'NoValidSession') {
                        this.timeoutService.error401.next(errorId);
                    }

                    this.fileInstance?.remove();
                    this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                    // 关闭二次认证密码框
                    this.modalInstance?.destroy(false);
                }
            }
        });
    }

    // 导出配置按钮
    public startToExport() {
        this.exporting = true;
        if (this.isIrm) {
            this.exportData();
            return;
        }
        this.commonService.getGenericInfo().subscribe((resp: any) => {
            const res = resp['body'];
            if (res &&  (res.StorageConfigReady === 1 || res.StorageConfigReady === 4)) {
                this.exportData();
            } else {
                let contentText = '';
                switch (res?.StorageConfigReady) {
                    case 0:
                        contentText = this.translate.instant('CONFIG_UPGRADE_NOREADY_TIP');
                        break;
                    case 2:
                        contentText = this.translate.instant('CONFIG_UPGRADE_NOTSURPPORT_TIP');
                        break;
                    case 3:
                        contentText = this.translate.instant('CONFIG_UPGRADE_ABNORMOL_TIP');
                        break;
                    default:
                        contentText = this.translate.instant('CONFIG_UPGRADE_CONFIRM_TIP');
                }
                const INSTANCE = this.tiMessage.open({
                    id: 'exportConfigModal',
                    type: 'prompt',
                    content: contentText,
                    okButton: {
                        show: true,
                        text: this.translate.instant('CONFIG_UPGRADE_CONFIRM'),
                        autofocus: false,
                        click: () => {
                            INSTANCE.close();
                            this.exportData();
                        }
                    },
                    cancelButton: {
                        show: true,
                        text: this.translate.instant('CONFIG_UPGRADE_CANCEL'),
                        click(): void {
                            INSTANCE.dismiss();
                        }
                    }
                });
            }
        });
    }

    // 导出
    public exportData() {
        this.exportBtnDisable = true;
        this.configUpgradeService.configExport().subscribe((res: any) => {
            const response = res.body;
            this.exportProgress.exportFail = false;
            this.exportProgress.show = true;
            this.exportProgress.value = 0;
            this.preProgress(response.url);
            return;
        });
    }

    // 查询taskprepare进度
    public preProgress(url) {
        this.configUpgradeService.getTask(url).subscribe((resp: any) => {
            const res = resp['body'];
            if (res.prepare_progress === 100 && res.downloadurl) {
                this.exportBtnDisable = false;
                this.exportProgress.value = 100;
                this.commonService.restDownloadFile(res.downloadurl);
                this.exporting = false;
            } else if (res.prepare_progress !== 100 || !res.downloadurl) {
                this.exportProgress.value = res.prepare_progress;
                setTimeout(() => {
                    this.preProgress(url);
                }, 3000);
            } else {
                this.exportProgress.exportFail = true;
            }
        }, (error) => {
            this.exportProgress.exportFail = true;
        });
    }

    // 查询上传任务进度
    public getTask() {
        this.configUpgradeService.task().subscribe((resp: any) => {
            const res = resp['body'];
            if (res.Percentage === '100%') {
                this.importProgress.value = 100;
                this.importBtnDisable = false;
                this.fileInstance.remove();
                if (this.isIrm) {
                    return;
                }
                const INSTANCE = this.tiMessage.open({
                    id: 'importSuccessModal',
                    type: 'prompt',
                    content: this.translate.instant('CONFIG_UPGRADE_EFFECTRESTART'),
                    okButton: {
                        show: true,
                        text: this.translate.instant('CONFIG_UPGRADE_RESET_NOW'),
                        autofocus: false,
                        click: () => {
                            INSTANCE.close();
                            // 立即重启跳转至服务器上下电页面
                            this.router.navigate(['/navigate/system/power'], { queryParams: { id: 2 } });
                        }
                    },
                    cancelButton: {
                        show: true,
                        text: this.translate.instant('CONFIG_UPGRADE_RESET_LATER'),
                        click(): void {
                            INSTANCE.dismiss();
                            // 清空文件上传框
                            this.file['fileItems'][0].remove();
                            this.importProgress.show = false;
                        }
                    },
                    title: this.translate.instant('CONFIG_UPGRADE_CONFIRM_TITLE')
                });
            } else if (res.Percentage !== '100%' && res.Percentage !== '-1%') {
                let taskPercentage = res.Percentage;
                if (taskPercentage != null) {
                    taskPercentage = taskPercentage.replace('%', '');
                    this.importBtnDisable = true;
                    this.importProgress.value = taskPercentage;
                }
                setTimeout(() => {
                    this.getTask();
                }, 3000);
            } else {
                this.fileInstance.remove();
                this.importProgress.importFail = true;
                this.fileMes = res.Messages ? ' (' + res.Messages + ')' : '';
            }
        });
    }

    public getUpgradeTip(res) {
        switch (res) {
            case PRODUCT.IBMC: case PRODUCT.PANGEA:
                this.configUpgradeTip = this.translate.instant('CONFIG_UPGRADE_TIP');
                this.isIrm = false;
                break;
            case PRODUCT.IRM:
                this.configUpgradeTip = this.translate.instant('IRM_IMPORTED_EXPORTED_TIP_INFO');
                this.isIrm = true;
                break;
            case PRODUCT.EMM:
                break;
        }
    }

    ngOnInit(): void {
        this.loading.state.next(false);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
        });
        this.getUpgradeTip(CommonData.productType);
    }

}
