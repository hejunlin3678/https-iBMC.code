import { Component, OnInit } from '@angular/core';
import { HorizontalAdapterArr } from '../../models';
import { HorizontalAdapterService } from './horizontal-connection-board.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-horizontal-connection-board',
    templateUrl: './horizontal-connection-board.component.html'
})
export class HorizontalAdapterComponent implements OnInit {

    public horizontalAdapterArr: HorizontalAdapterArr;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private horizontalAdapterService: HorizontalAdapterService,
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
        this.horizontalAdapterArr = this.otherService.activeCard.getCardArray as HorizontalAdapterArr;
        const post$ = JSON.stringify(this.horizontalAdapterArr) === '{}' ?
            this.horizontalAdapterService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.horizontalAdapterArr));
        post$.subscribe(
            (horizontalAdapterArr) => {
                this.horizontalAdapterArr = horizontalAdapterArr;
                this.otherService.activeCard.setCardArray = this.horizontalAdapterArr;
                this.columns = this.horizontalAdapterArr.title;
                this.srcData.data = this.horizontalAdapterArr.getHorizontalAdapters;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
