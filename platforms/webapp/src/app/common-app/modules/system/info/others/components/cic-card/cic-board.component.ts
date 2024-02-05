import { Component, OnInit } from '@angular/core';
import { OthersService } from '../../others.service';
import { CicBoardService } from './cic-board.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { CicBoardArray } from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { of } from 'rxjs/internal/observable/of';

@Component({
    selector: 'app-cic-board',
    templateUrl: './cic-board.component.html',
    styleUrls: ['./cic-board.component.scss']
})
export class CicComponent implements OnInit {

    public cicBoardArray: CicBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private cicBoardService: CicBoardService,
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
        this.cicBoardArray = this.otherService.activeCard.getCardArray as CicBoardArray;
        const post$ = JSON.stringify(this.cicBoardArray) === '{}' ?
            this.cicBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.cicBoardArray));
        post$.subscribe(
            (cicBoardArray) => {
                this.cicBoardArray = cicBoardArray;
                this.otherService.activeCard.setCardArray = this.cicBoardArray;
                this.columns = this.cicBoardArray.title;
                this.srcData.data = this.cicBoardArray.getExpandBoard;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
