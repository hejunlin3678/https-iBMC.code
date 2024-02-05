import { Component, OnInit } from '@angular/core';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { LoadingService } from 'src/app/common-app/service';
import { BasicComputationArray } from '../../models/basic-computation/basic-computation-array';
import { UnitsService } from '../../units.service';
import { BasicComputationService } from './basic-computation.service';

@Component({
    selector: 'app-basic-computation',
    templateUrl: './basic-computation.component.html',
    styleUrls: ['./basic-computation.component.scss']
})
export class BasicComputationComponent implements OnInit {

    private basicComputationArray: BasicComputationArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    public showDropDownIcon: boolean = false;

    constructor(
        private unitsService: UnitsService,
        private basicComputationService: BasicComputationService,
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
        this.basicComputationArray = this.unitsService.activeCard.getCardArray as BasicComputationArray;
        const post$ = JSON.stringify(this.basicComputationArray) === '{}' ?
            this.basicComputationService.factory(this.unitsService.activeCard) :
            of('').pipe(map(() => this.basicComputationArray));
        post$.subscribe(
            (basicComputationArray) => {
                this.basicComputationArray = basicComputationArray;
                this.unitsService.activeCard.setCardArray = this.basicComputationArray;
                this.columns = this.basicComputationArray.title;
                this.srcData.data = this.basicComputationArray.getbasicComputations;
                this.showDropDownIcon = basicComputationArray.getDropdownIconShow;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
