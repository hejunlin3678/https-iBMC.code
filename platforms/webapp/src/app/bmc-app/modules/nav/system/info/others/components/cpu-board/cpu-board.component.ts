import { Component, OnInit } from '@angular/core';
import { CPUBoardArray } from '../../models';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { boardIdCUidShow, OthersService } from '../../others.service';
import { CPUBoardService } from './cpu-board.service';
import { TranslateService } from '@ngx-translate/core';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { LoadingService } from 'src/app/common-app/service';

@Component({
    selector: 'app-cpu-board',
    templateUrl: './cpu-board.component.html'
})
export class CpuBoardComponent implements OnInit {

    private cPUBoardArray: CPUBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private cPUBoardService: CPUBoardService,
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
        this.cPUBoardArray = this.otherService.activeCard.getCardArray as CPUBoardArray;
        const post$ = JSON.stringify(this.cPUBoardArray) === '{}' ?
            this.cPUBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.cPUBoardArray));
        post$.subscribe(
            (cPUBoardArray) => {
                this.cPUBoardArray = cPUBoardArray;
                this.otherService.activeCard.setCardArray = this.cPUBoardArray;
                this.columns = this.cPUBoardArray.title;
                this.srcData.data = this.cPUBoardArray.getCPUBoards;
                boardIdCUidShow(this.columns, this.srcData.data);
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
