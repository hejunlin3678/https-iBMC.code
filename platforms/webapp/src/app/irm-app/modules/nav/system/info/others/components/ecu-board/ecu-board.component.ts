import { Component, OnInit } from '@angular/core';
import { ECUBoardArray } from '../../models';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { OthersService } from '../../others.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { EcuBoardService } from './ecu-board.service';

@Component({
    selector: 'app-ar-card',
    templateUrl: './ecu-board.component.html'
})
export class EcuBoardComponent implements OnInit {

    public ecuBoardArray: ECUBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private ecuBoardService: EcuBoardService,
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
        this.ecuBoardArray = this.otherService.activeCard.getCardArray as ECUBoardArray;
        const post$ = JSON.stringify(this.ecuBoardArray) === '{}' ?
            this.ecuBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.ecuBoardArray));
        post$.subscribe(
            (ecuBoardArray) => {
                this.ecuBoardArray = ecuBoardArray;
                this.otherService.activeCard.setCardArray = this.ecuBoardArray;
                this.columns = this.ecuBoardArray.title;
                this.srcData.data = this.ecuBoardArray.getECUBoards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
