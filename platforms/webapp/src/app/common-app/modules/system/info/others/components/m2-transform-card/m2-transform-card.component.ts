import { Component, OnInit } from '@angular/core';
import { M2TransformCardArray } from '../../models';
import { M2TransformCardService } from './m2-transform-card.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-m2-transform-card',
    templateUrl: './m2-transform-card.component.html'
})
export class M2TransformCardComponent implements OnInit {

    public m2TransformCardArray: M2TransformCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private m2TransformCardService: M2TransformCardService,
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
        this.m2TransformCardArray = this.otherService.activeCard.getCardArray as M2TransformCardArray;
        const post$ = JSON.stringify(this.m2TransformCardArray) === '{}' ?
            this.m2TransformCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.m2TransformCardArray));
        post$.subscribe(
            (m2TransformCardArray) => {
                this.m2TransformCardArray = m2TransformCardArray;
                this.otherService.activeCard.setCardArray = this.m2TransformCardArray;
                this.columns = this.m2TransformCardArray.title;
                this.srcData.data = this.m2TransformCardArray.getM2TransformCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
