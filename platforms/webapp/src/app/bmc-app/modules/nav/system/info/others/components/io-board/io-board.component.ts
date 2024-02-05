import { Component, OnInit } from '@angular/core';
import { IOBoardArray } from '../../models';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { boardIdCUidShow, OthersService } from '../../others.service';
import { IOBoardService } from './io-board.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-io-board',
    templateUrl: './io-board.component.html'
})
export class IoBoardComponent implements OnInit {

    public iOBoardArray: IOBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private iOBoardService: IOBoardService,
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
        this.iOBoardArray = this.otherService.activeCard.getCardArray as IOBoardArray;
        const post$ = JSON.stringify(this.iOBoardArray) === '{}' ?
            this.iOBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.iOBoardArray));
        post$.subscribe(
            (iOBoardArray) => {
                this.iOBoardArray = iOBoardArray;
                this.otherService.activeCard.setCardArray = this.iOBoardArray;
                this.columns = this.iOBoardArray.title;
                this.srcData.data = this.iOBoardArray.getIOBoards;
                boardIdCUidShow(this.columns, this.srcData.data);
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
