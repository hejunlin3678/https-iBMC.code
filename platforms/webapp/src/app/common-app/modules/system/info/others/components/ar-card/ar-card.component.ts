import { Component, OnInit } from '@angular/core';
import { ArCardArray } from '../../models';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { OthersService } from '../../others.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { ArCardService } from './ar-card.service';

@Component({
    selector: 'app-ar-card',
    templateUrl: './ar-card.component.html'
})
export class ArCrdComponent implements OnInit {

    public arCardArray: ArCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private arCardService: ArCardService,
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
        this.arCardArray = this.otherService.activeCard.getCardArray as ArCardArray;
        const post$ = JSON.stringify(this.arCardArray) === '{}' ?
            this.arCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.arCardArray));
        post$.subscribe(
            (arCardArray) => {
                this.arCardArray = arCardArray;
                this.otherService.activeCard.setCardArray = this.arCardArray;
                this.columns = this.arCardArray.title;
                this.srcData.data = this.arCardArray.getarCard;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
