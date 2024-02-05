import { Component, OnInit } from '@angular/core';
import { OthersService } from '../../others.service';
import { DiskBackPlaneService } from './disk-back-plane.service';
import { DiskBackPlaneArray } from '../../models';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-disk-back-plane',
    templateUrl: './disk-back-plane.component.html'
})
export class DiskBackPlaneComponent implements OnInit {

    public diskBackPlaneArray: DiskBackPlaneArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    public disabled: boolean = false;
    public panelWidth: string = '250px';

    constructor(
        private otherService: OthersService,
        private diskBackPlaneService: DiskBackPlaneService,
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
        this.diskBackPlaneArray = this.otherService.activeCard.getCardArray as DiskBackPlaneArray;
        const post$ = JSON.stringify(this.diskBackPlaneArray) === '{}' ?
            this.diskBackPlaneService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.diskBackPlaneArray));
        post$.subscribe(
            (diskBackPlaneArray) => {
                this.diskBackPlaneArray = diskBackPlaneArray;
                this.otherService.activeCard.setCardArray = this.diskBackPlaneArray;
                this.columns = this.diskBackPlaneArray.title;
                this.srcData.data = this.diskBackPlaneArray.getDiskBackPlanes;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
