import { Component, OnInit } from '@angular/core';
import { PCIeCardArray } from '../../models';
import { OthersService } from '../../others.service';
import { PcieCardService } from './pcie-cards.service';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
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

    constructor(
        private otherService: OthersService,
        private pcieCardService: PcieCardService,
        public i18n: TranslateService,
        private loading: LoadingService
    ) {
        this.displayed = [];
        this.srcData = { // 表格源数据，开发者对表格的数据设置请在这里进行
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
        this.pCIeCardArray = this.otherService.activeCard.getCardArray as PCIeCardArray;
        const post$ = JSON.stringify(this.pCIeCardArray) === '{}' ?
            this.pcieCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.pCIeCardArray));
        post$.subscribe(
            (pCIeCardArray) => {
                this.pCIeCardArray = pCIeCardArray;
                this.otherService.activeCard.setCardArray = this.pCIeCardArray;
                this.columns = this.pCIeCardArray.title;
                this.srcData.data = this.pCIeCardArray.getPCIeCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
