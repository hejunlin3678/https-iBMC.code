import { Component, OnInit } from '@angular/core';
import { PeripheralCardArray } from '../../models';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { OthersService } from '../../others.service';
import { PeripheralCardService } from './peripheral-card.service';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-peripheral-card',
    templateUrl: './peripheral-card.component.html'
})
export class PeripheralCardComponent implements OnInit {

    public peripheralCardArray: PeripheralCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private otherService: OthersService,
        private peripheralCardService: PeripheralCardService,
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
        setTimeout(() => {

        }, 0);
    }
    ngOnInit(): void {
        this.peripheralCardArray = this.otherService.activeCard.getCardArray as PeripheralCardArray;
        const post$ = JSON.stringify(this.peripheralCardArray) === '{}' ?
            this.peripheralCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.peripheralCardArray));
        post$.subscribe(
            (peripheralCardArray) => {
                this.peripheralCardArray = peripheralCardArray;
                this.otherService.activeCard.setCardArray = this.peripheralCardArray;
                this.columns = this.peripheralCardArray.title;
                this.srcData.data = this.peripheralCardArray.getPeripheralCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
