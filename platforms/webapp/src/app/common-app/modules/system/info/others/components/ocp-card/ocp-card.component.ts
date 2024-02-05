import { Component, OnInit } from '@angular/core';
import { OCPCardArray } from '../../models';
import { OCPCardService } from './ocp-card.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';

@Component({
    selector: 'app-ocp-card',
    templateUrl: './ocp-card.component.html',
    styleUrls: ['./ocp-card.component.scss']
})
export class OCPCardComponent implements OnInit {

    private oCPCardArray: OCPCardArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private oCPCardService: OCPCardService,
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
        this.oCPCardArray = this.otherService.activeCard.getCardArray as OCPCardArray;
        const post$ = JSON.stringify(this.oCPCardArray) === '{}' ?
            this.oCPCardService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.oCPCardArray));
        post$.subscribe(
            (oCPCardArray) => {
                this.oCPCardArray = oCPCardArray;
                this.otherService.activeCard.setCardArray = this.oCPCardArray;
                this.columns = this.oCPCardArray.title;
                this.srcData.data = this.oCPCardArray.getOCPCards;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
