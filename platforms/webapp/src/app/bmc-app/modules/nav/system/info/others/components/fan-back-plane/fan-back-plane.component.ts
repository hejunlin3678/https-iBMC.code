import { Component, OnInit } from '@angular/core';
import { FanBackPlaneArray } from '../../models';
import { FanBackPlaneService } from './fan-back-plane.service';
import { boardIdCUidShow, OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-fan-back-plane',
    templateUrl: './fan-back-plane.component.html'
})
export class FanBackPlaneComponent implements OnInit {

    public fanBackPlaneArray: FanBackPlaneArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private fanBackPlaneService: FanBackPlaneService,
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
        this.fanBackPlaneArray = this.otherService.activeCard.getCardArray as FanBackPlaneArray;
        const post$ = JSON.stringify(this.fanBackPlaneArray) === '{}' ?
            this.fanBackPlaneService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.fanBackPlaneArray));
        post$.subscribe(
            (fanBackPlaneArray) => {
                this.fanBackPlaneArray = fanBackPlaneArray;
                this.otherService.activeCard.setCardArray = this.fanBackPlaneArray;
                this.columns = this.fanBackPlaneArray.title;
                this.srcData.data = this.fanBackPlaneArray.getFanBackPlanes;
                boardIdCUidShow(this.columns, this.srcData.data);
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
