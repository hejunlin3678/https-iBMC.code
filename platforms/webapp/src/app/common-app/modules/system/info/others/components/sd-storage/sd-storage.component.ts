import { Component, OnInit } from '@angular/core';
import { SDStorageArray } from '../../models';
import { SDStorageService } from './sd-storage.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-sd-storage',
    templateUrl: './sd-storage.component.html'
})
export class SDStorageComponent implements OnInit {

    private sDStorageArray: SDStorageArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private sDStorageService: SDStorageService,
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
        this.sDStorageArray = this.otherService.activeCard.getCardArray as SDStorageArray;
        const post$ = JSON.stringify(this.sDStorageArray) === '{}' ?
            this.sDStorageService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.sDStorageArray));
        post$.subscribe(
            (sDStorageArray) => {
                this.sDStorageArray = sDStorageArray;
                this.otherService.activeCard.setCardArray = this.sDStorageArray;
                this.columns = this.sDStorageArray.title;
                this.srcData.data = this.sDStorageArray.getSDStorages;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
