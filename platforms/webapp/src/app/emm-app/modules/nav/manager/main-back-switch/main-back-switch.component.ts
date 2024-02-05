import { Component, OnInit } from '@angular/core';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { AlertMessageService, CommonService, LoadingService } from 'src/app/common-app/service';
import { MainBackSwitchService } from './services/main-back-switch.service';
import * as utils from 'src/app/common-app/utils';
import { Router } from '@angular/router';
import { ProductName } from 'src/app/emm-app/models/common.datatype';

@Component({
    selector: 'app-main-back-switch',
    templateUrl: './main-back-switch.component.html',
    styleUrls: ['./main-back-switch.component.scss']
})
export class MainBackSwitchComponent implements OnInit {

    constructor(
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private service: MainBackSwitchService,
        private alert: AlertMessageService,
        private router: Router,
        private loadingService: LoadingService,
        private commonService: CommonService,
    ) { }

    public mainBoardIndex: number = 1;
    public standbyBoardOnline: boolean = false;
    public productType: string;
    public boardSyncStatus = {
        text: 'EMM_SYNC_SYNCING',
        showLoading: true
    };
    public boardLists = [
        {
            title: '',
            chosed: true,
            isShow: false,
            img: 'assets/emm-assets/image/boardSwitch/activation.png'
        }
    ];
    public boardName: string = 'SMM';
    public interval;

    ngOnInit(): void {
        this.productType = sessionStorage.getItem('productModel') || 'TCE8080';
        this.loadingService.state.next(true);
        this.init();
    }
    ngOnDestroy(): void {
        clearInterval(this.interval);
    }
    public init() {
        this.service.getMainBackInfo().subscribe((response) => {
            this.mainBoardIndex = response.ActiveIndex;
            this.boardLists[0].isShow = true;
            switch (this.productType) {
                case ProductName.Emm12U:
                    this.boardName = 'MM';
                    this.boardLists[0].title = `${this.boardName}${this.mainBoardIndex}${this.getI18nValue('EMM_MAIN_BOARD')}`;
                    break;
                case ProductName.Tce4U:
                    this.boardLists[0].title = `${this.boardName}${this.mainBoardIndex}${this.getI18nValue('EMM_MAIN_BOARD')}`;
                    this.boardLists[0].img = 'assets/emm-assets/image/boardSwitch/activation_4U.png';
                    break;
                case ProductName.Tce8U:
                    this.boardLists[0].title = `${this.boardName}${this.mainBoardIndex}${this.getI18nValue('EMM_MAIN_BOARD')}`;
                    this.boardLists[0].img = 'assets/emm-assets/image/boardSwitch/activation_8U.png';
                    break;
                default:
                    break;
            }
            this.standbyBoardOnline = response?.StandbyPresent === 1;
            // 先判断是否有备板
            if (this.standbyBoardOnline) {
                const standbyBoardInfo = {
                    title: `${this.boardName}${response.StandbyIndex}${this.getI18nValue('EMM_STANDBY_BOARD')}`,
                    chosed: false,
                    isShow: true,
                    img: 'assets/emm-assets/image/boardSwitch/activation.png'
                };
                switch (this.productType) {
                    case ProductName.Emm12U:
                        standbyBoardInfo.title = `${this.boardName}${response.StandbyIndex}${this.getI18nValue('EMM_STANDBY_BOARD')}`;
                        break;
                    case ProductName.Tce4U:
                        standbyBoardInfo.img = 'assets/emm-assets/image/boardSwitch/un_activation_4U.png';
                        break;
                    case ProductName.Tce8U:
                        standbyBoardInfo.img = 'assets/emm-assets/image/boardSwitch/un_activation_8U.png';
                        break;
                    default:
                        break;
                }
                this.boardLists.push(standbyBoardInfo);
            }
            const statusValue = response?.SyncStatus;
            if (statusValue === 'Complete') {
                this.boardSyncStatus.text = 'EMM_SYNC_COMPLETE';
                this.boardSyncStatus.showLoading = false;
            } else if (statusValue === 'Syncing') {
                // 状态为“同步中”时执行轮询，直到状态变为：同步完成
                this.interval = setInterval(() => {
                    this.service.getMainBackInfo().subscribe((res) => {
                        const syncStatus = res?.SyncStatus === 'Complete';
                        if (syncStatus) {
                            this.boardSyncStatus.text = 'EMM_SYNC_COMPLETE';
                            this.boardSyncStatus.showLoading = false;
                            clearInterval(this.interval);
                        }
                    });
                }, 15000);
            }
            this.loadingService.state.next(false);
        }, (error) => {
            this.loadingService.state.next(false);
        });
    }
    // 获取国际化字段内容
    private getI18nValue(key: string): string {
        return this.translate.instant(key);
    }
    // 主备板倒换
    public changeBoard() {
        const warnTitle =
            utils.formatEntry(this.getI18nValue('EMM_SWITCHOVER_WARMINGS_CONTENT'), [this.boardName + this.mainBoardIndex.toString()]);
        const instance = this.tiMessage.open({
            id: 'changeBoard',
            type: 'prompt',
            content: `
                <div class='title'>${this.getI18nValue('EMM_SWITCHOVER_WARMINGS')}</div>
                <div class='content'>${warnTitle}</div>`,
            okButton: {
                show: true,
                autofocus: false,
                text: this.getI18nValue('ALARM_OK'),
                click: () => {
                    this.loadingService.state.next(true);
                    this.service.switchoverBoard().subscribe(response => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        setTimeout(() => {
                            this.commonService.clear();
                            localStorage.setItem('firmwareToLogin', 'true');
                            this.router.navigate(['login']);
                        }, 2500);
                    }, (error) => {
                        this.alert.eventEmit.emit({ type: 'error', label: 'EMM_SWITCHOVER_ERROR_MESSAGE' });
                        this.loadingService.state.next(false);
                    });
                    instance.close();
                }
            },
            cancelButton: {
                show: true,
                text: this.getI18nValue('ALARM_CANCEL'),
                click(): void {
                    instance.dismiss();
                }
            },
            title: this.getI18nValue('ALARM_OK'),
        });
    }
}
