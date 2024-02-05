import { Component, Input, OnInit } from '@angular/core';
import { EmmInfoService } from './emm-info.service';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiMessageService } from '@cloud/tiny3';
import { UpgradeModel, EmmInfo } from '../../model';
import { TranslateService } from '@ngx-translate/core';
import { AlertMessageService, CommonService, ErrortipService, LoadingService } from 'src/app/common-app/service';
import { Router } from '@angular/router';
import { getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-emm-info',
    templateUrl: './emm-info.component.html',
    styleUrls: ['./emm-info.component.scss']
})
export class EmmInfoComponent implements OnInit {
    @Input() isSystemLock: boolean;
    @Input() showLCDandFan: number;
    @Input() isConfigureComponentsUser;
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    constructor(
        private emmInfoService: EmmInfoService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private alert: AlertMessageService,
        private commonService: CommonService,
        private router: Router,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService,
    ) {
        this.displayed = [];
        this.srcData = {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
    }
    public upgrade = new UpgradeModel();
    public colsNumber = 2;
    public verticalAlign = 'middle';
    public labelWidth = '88px';
    public standbyPresent: number = 1;
    public columns: TiTableColumns[] = [
        {
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '20%'
        },
        {
            title: 'OTHER_BOARDNAME',
            width: '20%'
        },
        {
            title: 'EMM_UPGRADE_SOFTWARE_VERSION',
            width: '20%',
            tip: 'EMM_UPGRADE_SOFTWARE'
        },
        {
            title: 'COMMIN_CPLD_VERSION',
            width: '20%',
            tip: 'EMM_UPGRADE_CPLD'
        },
        {
            title: 'COMMON_OPERATE',
            width: '20%'
        }
    ];
    public trackByFn(index: string, item: any): string {
        return index;
    }
    // 重启
    public restartHmm(id, spare) {
        // 备板复位中备板禁用重启按钮
        if (this.isSystemLock || !this.isConfigureComponentsUser || (!spare && this.standbyPresent === -1)) {
            return;
        }
        const hmmId = 'H' + id;
        const instance = this.tiMessage.open({
            id: 'resetOpen',
            type: 'prompt',
            content: this.translate.instant('IBMC_REST_IBMC_TIP'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    instance.close();
                    this.emmInfoService.resetFirmware(hmmId).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        // 备板重启不需要退出登录
                        if (spare) {
                            this.commonService.clear();
                            localStorage.setItem('firmwareToLogin', 'true');
                            this.router.navigate(['login']);
                        }
                    }, (error) => {
                        const errorId = getMessageId(error?.error)[0].errorId;
                        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                    });
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
    ngOnInit(): void {
        this.emmInfoService.getManagersid().subscribe((res) => {
            const managersid = 'HMM' + res['body']?.ActiveIndex;
            const backupManagersid = 'HMM' + res['body']?.StandbyIndex;
            this.standbyPresent = res['body']?.StandbyPresent;
            this.getList(managersid, backupManagersid);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    public getList(managersid, backupManagersid) {
        this.emmInfoService.getHmmInfo(backupManagersid).then((response) => {
            // 备板等于1在位且返回错误代表复位中
            if (!response && this.standbyPresent === 1) {
                this.standbyPresent = -1;
            }
            this.emmInfoService.getEmmList(managersid, backupManagersid, this.standbyPresent).subscribe(
                (res) => {
                    this.upgrade = res[0].upgrade;
                    this.columns[3].tip = this.upgrade?.getMainModel === EmmInfo.MM920
                        ? this.translate.instant('EMM_UPGRADE_CPLD_REST') : this.translate.instant('EMM_UPGRADE_CPLD');
                    // TCE8040或者TCE8080才显示可用分区Software版本
                    if (this.showLCDandFan !== 1) {
                        res?.forEach((item) => {
                            delete item?.infoDetail['activeBackupSoftware'];
                        });
                    }
                    this.srcData.data = res;
                    this.loadingService.state.next(false);
                }, () => {
                    this.loadingService.state.next(false);
                }
            );
        }, () => {
            this.loadingService.state.next(false);
        });
    }
}
