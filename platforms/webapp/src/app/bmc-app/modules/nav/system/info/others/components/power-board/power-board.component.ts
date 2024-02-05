import { Component, OnInit } from '@angular/core';
import { PowerBoardArray } from '../../models';
import { PowerBoardService } from './power-board.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';


@Component({
    selector: 'app-power-board',
    templateUrl: './power-board.component.html'
})
export class PowerBoardComponent implements OnInit {

    public powerBoardArray: PowerBoardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private powerBoardService: PowerBoardService,
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
        this.powerBoardArray = this.otherService.activeCard.getCardArray as PowerBoardArray;
        const post$ = JSON.stringify(this.powerBoardArray) === '{}' ?
            this.powerBoardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.powerBoardArray));
        post$.subscribe(
            (powerBoardArray) => {
                this.powerBoardArray = powerBoardArray;
                this.otherService.activeCard.setCardArray = this.powerBoardArray;
                this.columns = this.powerBoardArray.title;
                this.srcData.data = this.powerBoardArray.getPowerBoards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
