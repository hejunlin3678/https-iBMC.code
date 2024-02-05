import { Component, OnInit } from '@angular/core';
import { GPUBoardArray } from '../../models';
import { GPUBoardService } from './gpu-board.service';
import { boardIdCUidShow, OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-gpu-board',
    templateUrl: './gpu-board.component.html'
})
export class GPUBoardComponent implements OnInit {

    private gPUBoardArray: GPUBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private gPUBoardService: GPUBoardService,
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
        this.gPUBoardArray = this.otherService.activeCard.getCardArray as GPUBoardArray;
        const post$ = JSON.stringify(this.gPUBoardArray) === '{}' ?
            this.gPUBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.gPUBoardArray));
        post$.subscribe(
            (diskBackPlaneArray) => {
                this.gPUBoardArray = diskBackPlaneArray;
                this.otherService.activeCard.setCardArray = this.gPUBoardArray;
                this.columns = this.gPUBoardArray.title;
                this.srcData.data = this.gPUBoardArray.getGPUBoards;
                boardIdCUidShow(this.columns, this.srcData.data);
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
