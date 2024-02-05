import { Component, OnInit } from '@angular/core';
import { LeakDetectionArray } from '../../models';
import { LeakDetectionService } from './leakDetection.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-leakDetection',
    templateUrl: './leakDetection.component.html'
})
export class LeakDetectionComponent implements OnInit {

    private leakDetectionArray: LeakDetectionArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private leakDetectionService: LeakDetectionService,
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
        this.leakDetectionArray = this.otherService.activeCard.getCardArray as LeakDetectionArray;
        const post$ = JSON.stringify(this.leakDetectionArray) === '{}' ?
            this.leakDetectionService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.leakDetectionArray));
        post$.subscribe(
            (leakDetectionArray) => {
                this.leakDetectionArray = leakDetectionArray;
                this.otherService.activeCard.setCardArray = this.leakDetectionArray;
                this.columns = this.leakDetectionArray.title;
                this.srcData.data = this.leakDetectionArray.getLDs;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
