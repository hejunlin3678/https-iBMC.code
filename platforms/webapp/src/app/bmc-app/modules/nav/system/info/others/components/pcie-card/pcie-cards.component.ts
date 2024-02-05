import { Component, OnInit } from '@angular/core';
import { PCIeCardArray } from '../../models';
import { OthersService } from '../../others.service';
import { PcieCardService } from './pcie-cards.service';
import { TiMessageService, TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { AlertMessageService, LoadingService, SystemLockService, UserInfoService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-pcie-cards',
    templateUrl: './pcie-cards.component.html',
    styleUrls: ['./pcie-cards.component.scss']
})
export class PCIeCardsComponent implements OnInit {
    public pCIeCardArray: PCIeCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    public isSystemLock: boolean = false;
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureComponents']);

    constructor(
        private otherService: OthersService,
        private pcieCardService: PcieCardService,
        public i18n: TranslateService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private lockService: SystemLockService,
        private user: UserInfoService,
        private loading: LoadingService
    ) {
        this.displayed = [];
        this.srcData = {
            // 表格源数据，开发者对表格的数据设置请在这里进行
            data: [],
            state: {
                searched: false, // 源数据未进行搜索处理
                sorted: false, // 源数据未进行排序处理
                paginated: false // 源数据未进行分页处理
            }
        };
        this.columns = [];
    }

    ngOnInit(): void {
         // 系统锁定功能
         this.isSystemLock = this.lockService.getState();
         this.lockService.lockStateChange.subscribe(res => {
             if (this.isSystemLock !== res) {
                 this.isSystemLock = res;
             }
         });
        this.pCIeCardArray = this.otherService.activeCard.getCardArray as PCIeCardArray;
        const post$ =
            JSON.stringify(this.pCIeCardArray) === '{}'
                ? this.pcieCardService.factory(this.otherService.activeCard)
                : of('').pipe(map(() => this.pCIeCardArray));
        post$.subscribe(
            (pCIeCardArray) => {
                this.pCIeCardArray = pCIeCardArray;
                this.otherService.activeCard.setCardArray = this.pCIeCardArray;
                this.columns = this.pCIeCardArray.title;
                this.srcData.data = this.pCIeCardArray.getPCIeCards;
                const data = this.srcData.data;
                /**
                 * 设置展开的显示数据
                 * 把字段为title的项取消展示
                 */
                data.forEach((item: any) => {
                    const extendAttr = item.extendAttr;
                    const showList: any = {};
                    if (extendAttr && JSON.stringify(extendAttr) !== '{}') {
                        for (const key in extendAttr) {
                            if (key !== 'title' && key !== 'buttonTitle') {
                                showList[key] = extendAttr[key];
                            }
                        }
                        item.showList = showList;
                    }
                });
            },
            () => {},
            () => {
                this.loading.state.next(false);
            }
        );
    }

    public sendNmi(row) {
        this.tiMessage.open({
            id: 'interruptDialog',
            type: 'warn',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('OTHER_INTERRUPT_WARN'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: {},
            close: () => {
                this.pcieCardService.sendNmiData(Number(row.id)).subscribe(
                    (response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    },
                    (error) => {
                        this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                    }
                );
            },
            dismiss: () => {}
        });
    }
}
