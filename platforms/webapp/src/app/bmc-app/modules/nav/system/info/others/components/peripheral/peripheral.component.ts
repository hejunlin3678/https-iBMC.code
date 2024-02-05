import { Component, OnInit } from '@angular/core';
import { PeripheralFirmwareArray } from '../../models';
import { PeripheralService } from './peripheral.service';
import { OthersService } from '../../others.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';


@Component({
    selector: 'app-peripheral',
    templateUrl: './peripheral.component.html'
})
export class PeripheralComponent implements OnInit {

    private peripheralFirmwareArray: PeripheralFirmwareArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private peripheralService: PeripheralService,
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
        this.peripheralFirmwareArray = this.otherService.activeCard.getCardArray as PeripheralFirmwareArray;
        const post$ = JSON.stringify(this.peripheralFirmwareArray) === '{}' ?
            this.peripheralService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.peripheralFirmwareArray));
        post$.subscribe(
            (peripheralFirmwareArray) => {
                this.peripheralFirmwareArray = peripheralFirmwareArray;
                this.otherService.activeCard.setCardArray = this.peripheralFirmwareArray;
                this.columns = this.peripheralFirmwareArray.title;
                this.srcData.data = this.peripheralFirmwareArray.getPeripheralFirmwares;
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }

}
