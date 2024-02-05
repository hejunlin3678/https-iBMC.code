import { Component, OnInit } from '@angular/core';
import { PCIERiserCardArray } from '../../models';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { PcieRiserCardService } from './pcie-riser-card.service';
import { OthersService } from '../../others.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
@Component({
    selector: 'app-pcie-riser-card',
    templateUrl: './pcie-riser-card.component.html'
})
export class PCIeRiserCardComponent implements OnInit {

    public pCIERiserCardArray: PCIERiserCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private otherService: OthersService,
        private pcieRiserCardService: PcieRiserCardService,
        public i18n: TranslateService,
        private loading: LoadingService
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
        this.columns = [];
    }
    ngOnInit(): void {
        this.pCIERiserCardArray = this.otherService.activeCard.getCardArray as PCIERiserCardArray;
        const post$ = JSON.stringify(this.pCIERiserCardArray) === '{}' ?
            this.pcieRiserCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.pCIERiserCardArray));
        post$.subscribe(
            (pCIERiserCardArray) => {
                this.pCIERiserCardArray = pCIERiserCardArray;
                this.otherService.activeCard.setCardArray = this.pCIERiserCardArray;
                this.columns = this.pCIERiserCardArray.title;
                this.srcData.data = this.pCIERiserCardArray.getPCIERiserCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
