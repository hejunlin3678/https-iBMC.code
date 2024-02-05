import { Component, OnInit } from '@angular/core';
import { MezzCardArray } from '../../models';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { boardIdCUidShow, OthersService } from '../../others.service';
import { MezzCardService } from './mezz-card.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
@Component({
    selector: 'app-mezz-card',
    templateUrl: './mezz-card.component.html'
})
export class MezzCardComponent implements OnInit {

    public mezzCardArray: MezzCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private otherService: OthersService,
        private mezzCardService: MezzCardService,
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
        this.mezzCardArray = this.otherService.activeCard.getCardArray as MezzCardArray;
        const post$ = JSON.stringify(this.mezzCardArray) === '{}' ?
            this.mezzCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.mezzCardArray));
        post$.subscribe(
            (mezzCardArray) => {
                this.mezzCardArray = mezzCardArray;
                this.otherService.activeCard.setCardArray = this.mezzCardArray;
                this.columns = this.mezzCardArray.title;
                this.srcData.data = this.mezzCardArray.getMezzCards;
                boardIdCUidShow(this.columns, this.srcData.data);
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
