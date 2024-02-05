import { Component, OnInit } from '@angular/core';
import { MemoryBoardArray } from '../../models';
import { MemoryBoardService } from './memory-board.service';
import { OthersService } from '../../others.service';
import { TiTableColumns, TiTableSrcData, TiTableRowData } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
@Component({
    selector: 'app-memory-board',
    templateUrl: './memory-board.component.html'
})
export class MemoryBoardComponent implements OnInit {

    public memoryBoardArray: MemoryBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private memoryBoardService: MemoryBoardService,
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
        this.memoryBoardArray = this.otherService.activeCard.getCardArray as MemoryBoardArray;
        const post$ = JSON.stringify(this.memoryBoardArray) === '{}' ?
            this.memoryBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.memoryBoardArray));
        post$.subscribe(
            (diskBackPlaneArray) => {
                this.memoryBoardArray = diskBackPlaneArray;
                this.otherService.activeCard.setCardArray = this.memoryBoardArray;
                this.columns = this.memoryBoardArray.title;
                this.srcData.data = this.memoryBoardArray.getMemoryBoards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
