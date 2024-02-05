import { Component, Input, OnInit } from '@angular/core';
import { TiFileInfo, TiFileItem, TiTableColumns, TiTableRowData, TiTableSrcData, TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { SPUpgradeServices } from './sp-upgrade.services';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { getMessageId } from 'src/app/common-app/utils';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { IUpdataFileData } from './dataType/data.interface';
import { Router } from '@angular/router';

@Component({
    selector: 'app-sp-upgrade',
    templateUrl: './sp-upgrade.component.html',
    styleUrls: ['./sp-upgrade.component.scss']
})

export class SPUpgradeComponent implements OnInit {
    constructor(
        private upgradeServices: SPUpgradeServices,
        private translate: TranslateService,
        private loadingService: LoadingService,
        private tiMessage: TiMessageService,
        private router: Router,
        private alertService: AlertMessageService
    ) { }

    @Input() isSystemLock;
    public showAlert = {
        type: 'prompt',
        openAlert: true,
        closeIcon: false
    };

    public upgradeStatus = {
        upgradeFile: {
            uploaded: false
        },
        signatureFile: {
            uploaded: false
        }
    };
    public currentPage: number = 1;
    public totalNumber: number = 0;
    public pageSize: { options: number[], size: number } = {
        options: [10, 20, 50, 100],
        size: 10
    };
    // 表示表格实际呈现的数据（开发者默认设置为[]即可）
    public displayed: TiTableRowData[] = [];
    private data: TiTableRowData[] = [];
    public columns: TiTableColumns[] = [
        {
            title: this.translate.instant('OTHER_ID'),
            width: '10%'
        },
        {
            title: this.translate.instant('COMMON_NAME'),
            width: '90%'
        }
    ];
    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };
    // 保存按钮状态
    public upgradeDisabled: boolean = true;
    public signatureDisabled: boolean = true;
    public upgradeFileAddError: boolean = false;
    public signatureFileAddError: boolean = false;
    // 文件上传
    public upgradeErrorTips: string = '';
    public upgradeFormData;
    public signatureErrorTips: string = '';
    public signatureFormData;
    public uploaderConfig = {
        placeholder: this.translate.instant('IBMC_UPGRADE_ZIP_FORMAT'),
        url: '',
        type: 'POST',
        filters: [{
            name: 'type',
            params: ['.zip']
        }, {
            name: 'maxCount',
            params: [1]
        },
        {
            name: 'maxSize',
            params: [1024 * 1024 * 499]
        }
        ],
        onAddItemFailed: (error: { file: TiFileInfo, validResults: string[] }) => {
            if (error.validResults.indexOf('type') >= 0) {
                this.upgradeErrorTips = 'IBMC_UPGRADE_ZIP_FORMAT';
            } else if (error.validResults.indexOf('maxSize') >= 0) {
                this.upgradeErrorTips = 'IBMC_SP_FILE_OUT_RANGE';
            }
            this.upgradeDisabled = true;
            this.upgradeFileAddError = true;
        },
        onAddItemSuccess: (fileItem: TiFileItem) => {
            this.upgradeFileAddError = false;
            this.upgradeFormData = fileItem;
            this.upgradeDisabled = false;
        },
        onRemoveItems: () => {
            this.upgradeDisabled = true;
            this.upgradeFileAddError = false;
        },
    };
    public uploaderSignature = {
        placeholder: this.translate.instant('IBMC_UPGRADE_ASC_FORMAT'),
        url: '',
        type: 'POST',
        filters: [{
            name: 'type',
            params: ['.asc,.p7s']
        }, {
            name: 'maxCount',
            params: [1]
        }, {
            name: 'maxSize',
            params: [1024 * 1024 * 90]
        }],
        onAddItemFailed: (error: { file: TiFileInfo, validResults: string[] }) => {
            if (error.validResults.indexOf('type') >= 0) {
                this.signatureErrorTips = 'IBMC_UPGRADE_ASC_FORMAT';
            } else if (error.validResults.indexOf('maxSize') >= 0) {
                this.signatureErrorTips = 'IBMC_SP_FILE_OUT_RANGE';
            }
            this.signatureDisabled = true;
            this.signatureFileAddError = true;
        },
        onAddItemSuccess: (fileItem: TiFileItem) => {
            this.signatureFileAddError = false;
            this.signatureFormData = fileItem;
            this.signatureDisabled = false;
        },
        onRemoveItems: () => {
            this.signatureDisabled = true;
            this.signatureFileAddError = false;
        },
    };
    // 上传文件
    public upgradeUpload() {
        if (this.signatureDisabled || this.upgradeDisabled) {
            return;
        }
        this.signatureDisabled = true;
        this.upgradeDisabled = true;
        const formData = new FormData();
        formData.append('ImagePath', this.upgradeFormData._file, this.upgradeFormData._file.name);
        formData.append('SignaturePath', this.signatureFormData._file, this.signatureFormData._file.name);
        this.loadingService.state.next(true);
        this.upgradeServices.uploadFile({
            formData
        }).subscribe({
            next: () => {
                this.loadingService.state.next(false);
                if (this.upgradeFormData) {
                    this.upgradeFormData.remove();
                }
                if (this.signatureFormData) {
                    this.signatureFormData.remove();
                }
                this.alertService.eventEmit.emit({
                    type: 'success',
                    label: this.translate.instant('COMMON_SUCCESS')
                });
                this.initData();
                const upgradeSpModel = this.tiMessage.open({
                    id: 'upgradeSpModel',
                    type: 'confirm',
                    okButton: {
                        show: true,
                        autofocus: false,
                        click: () => {
                            this.router.navigate(['/navigate/manager/sp']);
                            upgradeSpModel.close();
                        }
                    },
                    cancelButton: {
                        show: true,
                        text: this.translate.instant('COMMON_CANCEL'),
                        click: () => {
                            upgradeSpModel.close();
                        }
                    },
                    title: this.translate.instant('IBMC_UPGRADE_UPLOAD_SUCCESS'),
                    content: this.translate.instant('IBMC_UPGRADE_UPLOAD_SP')
                });
            },
            error: (error) => {
                this.loadingService.state.next(false);
                this.signatureDisabled = false;
                this.upgradeDisabled = false;
                if (this.upgradeFormData) {
                    this.upgradeFormData.remove();
                }
                if (this.signatureFormData) {
                    this.signatureFormData.remove();
                }
                const errorId = getMessageId(error.error)[0]?.errorId;
                const errorMsg = errorId ? this.translate.instant(errorId)['errorMessage'] || 'COMMON_FAILED' : 'COMMON_FAILED';
                this.alertService.eventEmit.emit({
                    type: 'error',
                    label: errorMsg
                });
            }
        });
    }
    public delList() {
        const delUpgradeSpModel = this.tiMessage.open({
            id: 'delUpgradeSpModel',
            type: 'prompt',
            okButton: {
                show: true,
                autofocus: false,
                click: () => {
                    delUpgradeSpModel.close();
                    this.loadingService.state.next(true);
                    this.upgradeServices.delSPUpdateFileList().subscribe(() => {
                        this.initData();
                        this.alertService.eventEmit.emit({ type: 'success', label: this.translate.instant('COMMON_SUCCESS')});
                        this.loadingService.state.next(false);
                    }, () => {
                        this.alertService.eventEmit.emit({ type: 'error', label: this.translate.instant('COMMON_FAILED')});
                        this.loadingService.state.next(false);
                    });
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_CANCEL'),
                click: () => {
                    delUpgradeSpModel.close();
                }
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK')
        });
    }
    public initData = () => {
        this.upgradeServices.getSPUpdateFileList().subscribe((res: IUpdataFileData[]) => {
            this.srcData.data = res;
            this.totalNumber = res.length;
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    ngOnInit(): void {
        this.initData();
    }
}
