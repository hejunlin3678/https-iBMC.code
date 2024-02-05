import { Component, OnInit } from '@angular/core';
import { OthersService } from '../../others.service';
import { ExpandBoardService } from './expand-board.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { ExpandBoardArray } from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { of } from 'rxjs/internal/observable/of';

@Component({
    selector: 'app-expand-board',
    templateUrl: './expand-board.component.html'
})
export class ExpandBoardComponent implements OnInit {

    public expandBoardArray: ExpandBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private expandBoardService: ExpandBoardService,
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
        this.expandBoardArray = this.otherService.activeCard.getCardArray as ExpandBoardArray;
        const post$ = JSON.stringify(this.expandBoardArray) === '{}' ?
            this.expandBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.expandBoardArray));
        post$.subscribe(
            (expandBoardArray) => {
                this.expandBoardArray = expandBoardArray;
                this.otherService.activeCard.setCardArray = this.expandBoardArray;
                this.columns = this.expandBoardArray.title;
                this.srcData.data = this.expandBoardArray.getExpandBoard;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
