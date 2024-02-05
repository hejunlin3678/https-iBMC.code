import { Component, OnInit } from '@angular/core';
import { RaidCardArray } from '../../models';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { OthersService } from '../../others.service';
import { RaidCardService } from './raid-card.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-raid-card',
    templateUrl: './raid-card.component.html'
})
export class RAIDCardComponent implements OnInit {

    public raidCardArray: RaidCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private otherService: OthersService,
        private raidCardService: RaidCardService,
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
        this.raidCardArray = this.otherService.activeCard.getCardArray as RaidCardArray;
        const post$ = JSON.stringify(this.raidCardArray) === '{}' ?
            this.raidCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.raidCardArray));
        post$.subscribe(
            (raidCardArray) => {
                this.raidCardArray = raidCardArray;
                this.otherService.activeCard.setCardArray = this.raidCardArray;
                this.columns = this.raidCardArray.title;
                this.srcData.data = this.raidCardArray.getRaidCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
