import { Component, OnInit } from '@angular/core';
import { LCDArray } from '../../models';
import { LcdService } from './lcd.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-lcd',
    templateUrl: './lcd.component.html'
})
export class LCDComponent implements OnInit {

    private lCDArray: LCDArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private lcdService: LcdService,
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
        this.lCDArray = this.otherService.activeCard.getCardArray as LCDArray;
        const post$ = JSON.stringify(this.lCDArray) === '{}' ?
            this.lcdService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.lCDArray));
        post$.subscribe(
            (lCDArray) => {
                this.lCDArray = lCDArray;
                this.otherService.activeCard.setCardArray = this.lCDArray;
                this.columns = this.lCDArray.title;
                this.srcData.data = this.lCDArray.getLCDs;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
