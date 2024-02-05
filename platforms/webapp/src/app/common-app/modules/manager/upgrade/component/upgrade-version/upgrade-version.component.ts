import { Component, Input, OnInit, OnChanges, SimpleChanges } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService, ErrortipService, SystemLockService, CommonService } from 'src/app/common-app/service';
import { UpgradeVersion } from '../../model';
import { LoadingService } from 'src/app/common-app/service';
import { UpgradeVersionService } from './services';
import { TiMessageService, TiTableRowData, TiTableSrcData, TiTableColumns, TiFileItem } from '@cloud/tiny3';
import { AlertMessageService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { getMessageId } from 'src/app/common-app/utils';
import { UpgradeService } from '../../services';

@Component({
    selector: 'app-upgrade-version',
    templateUrl: './upgrade-version.component.html',
    styleUrls: ['./upgrade-version.component.scss']
})
export class UpgradeVersionComponent implements OnChanges {
    @Input() upgradeVersionInfo;
    @Input() versionList;

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private upgradeService: UpgradeService,
        private upgradeVersionService: UpgradeVersionService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private router: Router,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService,
        private commonService: CommonService,
        private lockService: SystemLockService
    ) { }

    // 权限判断
    public configureComponents = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = true;

    public firmWareVesion = {
        mainLabel: this.translate.instant('IBMC_MAIN_ROLLBACK'),
        activeLabel: this.translate.instant('IBMC_MAIN_PARTITY_IMAGE_VERSION_TWO'),
        backupLabel: this.translate.instant('IBMC_BACK_PARTITY_IMAGE_VERSION_TWO')
    };
    public firmWareVesionUnavailable = {
        ROLLBACK: this.translate.instant('IBMC_MAIN_ROLLBACK_TWO')
    };

    public upgrade = this.upgradeService.upgrade;
    public upgradeVersion = new UpgradeVersion();

    // 表格数据
    public searchWords = [];
    public searchKeys = ['name'];
    public displayed: TiTableRowData[] = [];
    private data: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public columns: TiTableColumns[] = [
        {
            title: this.translate.instant('COMMON_NAME'),
            width: '35%',
            key: 'name',
            selected: null,
            options: [{
                label: this.translate.instant('IBMC_MANUAL_FIRMWARE'),
                type: 'firmware'
            }, {
                label: this.translate.instant('IBMC_MANUAL_SOFTWARE'),
                type: 'software'
            }, {
                label: this.translate.instant('IBMC_MANUAL_DRUVING'),
                type: 'driver'
            }],
            multiple: true,
            selectAll: true
        },
        {
            title: this.translate.instant('IBMC_VERSION_CURRENT'),
            width: '35%'
        }
    ];

    public initData = () => {
        this.upgradeVersion = this.upgradeVersionInfo;
        if (this.upgradeVersion.getUnavailable) {
            this.firmWareVesion.mainLabel = this.firmWareVesionUnavailable.ROLLBACK;
        }

        if (this.upgradeVersion.getTaskMembers) {
            this.upgrade.setFirmBtnRest(true);
            this.upgrade.setFirmBtnBack(true);
        }

        // 版本列表数据
        const versionsData = this.versionList;
        this.data = versionsData;
        this.srcData = {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        this.loadingService.state.next(false);
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.upgradeVersionInfo && changes.upgradeVersionInfo.currentValue) {
            this.loadingService.state.next(true);
            this.isSystemLock = this.lockService.getState();
            this.lockService.lockStateChange.subscribe(res => {
                if (this.isSystemLock !== res) {
                    this.isSystemLock = res;
                }
            });
            this.initData();
        }
    }

    // 重启IBMC
    public resets = () => {
        const restParams = {};
        this.upgradeVersionService.resetFirmware(restParams).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.commonService.clear();
            localStorage.setItem('firmwareToLogin', 'true');
            this.router.navigate(['login']);
        }, function (error) {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        });
    }

    // 重启IBMC
    public resetFirmware = () => {
        const instance = this.tiMessage.open({
            id: 'resetOpen',
            type: 'prompt',
            content: this.translate.instant('IBMC_REST_IBMC_TIP'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                click: () => {
                    instance.close();
                    this.resets();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click(): void {
                    instance.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }

    // 主备置换
    public rollBacks = () => {
        const query = {};
        const rollBack = this.upgradeVersionService.rollBackFirmware(query);
        rollBack.subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            setTimeout(() => {
                this.commonService.clear();
                localStorage.setItem('firmwareToLogin', 'true');
                this.router.navigate(['login']);
            }, 3000);
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
        });
    }
    // 主备镜像倒换
    public rollBackFirmware = () => {
        const instance = this.tiMessage.open({
            id: 'rollBackOpen',
            type: 'prompt',
            content: !this.upgradeVersion.getUnavailable ? this.translate.instant('IBMC_MAIN_ROLLBACK_TIP') : this.translate.instant('IBMC_MAIN_ROLLBACK_TIP_TWO'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                click: () => {
                    instance.close();
                    this.rollBacks();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click(): void {
                    instance.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }

}
