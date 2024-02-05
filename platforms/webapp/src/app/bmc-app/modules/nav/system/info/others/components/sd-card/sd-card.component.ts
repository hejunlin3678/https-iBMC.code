import { Component, OnInit } from '@angular/core';
import { SDCardArray } from '../../models';
import { SDCardService } from './sd-card.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-sd-card',
    templateUrl: './sd-card.component.html'
})
export class SDCardComponent implements OnInit {

    private sDCardArray: SDCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private sdCardService: SDCardService,
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
        this.sDCardArray = this.otherService.activeCard.getCardArray as SDCardArray;
        const post$ = JSON.stringify(this.sDCardArray) === '{}' ?
            this.sdCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.sDCardArray));
        post$.subscribe(
            (sDCardArray) => {
                this.sDCardArray = sDCardArray;
                this.otherService.activeCard.setCardArray = this.sDCardArray;
                this.columns = this.sDCardArray.title;
                this.srcData.data = this.sDCardArray.getSDCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
