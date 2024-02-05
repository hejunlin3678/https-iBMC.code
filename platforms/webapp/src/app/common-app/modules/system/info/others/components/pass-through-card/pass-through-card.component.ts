import { Component, OnInit } from '@angular/core';
import { PassThroughCardArray } from '../../models';
import { PassThroughCardService } from './pass-through-card.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
@Component({
    selector: 'app-pass-through-card',
    templateUrl: './pass-through-card.component.html'
})
export class PassThroughCardComponent implements OnInit {

    public passThroughCardArray: PassThroughCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private passThroughCardService: PassThroughCardService,
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
        this.passThroughCardArray = this.otherService.activeCard.getCardArray as PassThroughCardArray;
        const post$ = JSON.stringify(this.passThroughCardArray) === '{}' ?
            this.passThroughCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.passThroughCardArray));
        post$.subscribe(
            (passThroughCardArray) => {
                this.passThroughCardArray = passThroughCardArray;
                this.otherService.activeCard.setCardArray = this.passThroughCardArray;
                this.columns = this.passThroughCardArray.title;
                this.srcData.data = this.passThroughCardArray.getPassThroughCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
