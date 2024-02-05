import { Component, OnInit } from '@angular/core';
import { ChassisBackplaneArray } from '../../models';
import { ChassisBackPlaneService } from './chassis-back-plane.service';
import { boardIdCUidShow, OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { map } from 'rxjs/internal/operators/map';
import { of } from 'rxjs/internal/observable/of';
import { LoadingService } from 'src/app/common-app/service';

@Component({
    selector: 'app-chassis-back-plane',
    templateUrl: './chassis-back-plane.component.html'
})
export class ChassisBackplaneComponent implements OnInit {

    private chassisBackplaneArray: ChassisBackplaneArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private chassisBackPlaneService: ChassisBackPlaneService,
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
        this.chassisBackplaneArray = this.otherService.activeCard.getCardArray as ChassisBackplaneArray;
        const post$ = JSON.stringify(this.chassisBackplaneArray) === '{}' ?
            this.chassisBackPlaneService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.chassisBackplaneArray));
        post$.subscribe(
            (chassisBackplaneArray) => {
                this.chassisBackplaneArray = chassisBackplaneArray;
                this.otherService.activeCard.setCardArray = this.chassisBackplaneArray;
                this.columns = this.chassisBackplaneArray.title;
                this.srcData.data = this.chassisBackplaneArray.getChassisBackplanes;
                boardIdCUidShow(this.columns, this.srcData.data);
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
