import { Component, OnInit } from '@angular/core';
import { SecurityArray } from '../../models';
import { SecurityService } from './security.service';
import { OthersService } from '../../others.service';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
@Component({
    selector: 'app-security',
    templateUrl: './security.component.html'
})
export class SecurityComponent implements OnInit {

    public securityArray: SecurityArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private otherService: OthersService,
        private securityService: SecurityService,
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
        this.securityArray = this.otherService.activeCard.getCardArray as SecurityArray;
        const post$ = JSON.stringify(this.securityArray) === '{}' ?
            this.securityService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.securityArray));
        post$.subscribe(
            (securityArray) => {
                this.securityArray = securityArray;
                this.otherService.activeCard.setCardArray = this.securityArray;
                this.columns = this.securityArray.title;
                this.srcData.data = this.securityArray.getSecuritys;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
