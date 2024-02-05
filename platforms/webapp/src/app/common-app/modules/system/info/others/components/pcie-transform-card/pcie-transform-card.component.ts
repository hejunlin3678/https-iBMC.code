import { Component, OnInit } from '@angular/core';
import { PCIETransformCardArray } from '../../models';
import { PcieTransformCardService } from './pcie-transform-card.service';
import { OthersService } from '../../others.service';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-pcie-transform-card',
    templateUrl: './pcie-transform-card.component.html'
})
export class PCIeTransformCardComponent implements OnInit {

    public pCIETransformCardArray: PCIETransformCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private pcieTransformCardService: PcieTransformCardService,
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
        this.pCIETransformCardArray = this.otherService.activeCard.getCardArray as PCIETransformCardArray;
        const post$ = JSON.stringify(this.pCIETransformCardArray) === '{}' ?
            this.pcieTransformCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.pCIETransformCardArray));
        post$.subscribe(
            (pCIETransformCardArray) => {
                this.pCIETransformCardArray = pCIETransformCardArray;
                this.otherService.activeCard.setCardArray = this.pCIETransformCardArray;
                this.columns = this.pCIETransformCardArray.title;
                this.srcData.data = this.pCIETransformCardArray.getPCIETransformCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
