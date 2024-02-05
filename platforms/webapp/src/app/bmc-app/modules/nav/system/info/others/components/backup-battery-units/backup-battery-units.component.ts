import { Component, OnInit } from '@angular/core';
import { OthersService } from '../../others.service';
import { BackupBatteryUnitsService } from './backup-battery-units.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { BackupBatteryUnitsArray } from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { of } from 'rxjs/internal/observable/of';
import { GlobalDataService, PRODUCTTYPE } from 'src/app/common-app/service';

@Component({
    selector: 'app-backup-battery-units',
    templateUrl: './backup-battery-units.component.html'
})
export class BackupBatteryUnitsComponent implements OnInit {

    public backupBatteryUnitsArray: BackupBatteryUnitsArray;
    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;

    constructor(
        private otherService: OthersService,
        private backupBatteryUnitsService: BackupBatteryUnitsService,
        public i18n: TranslateService,
        private loading: LoadingService,
        private globalData: GlobalDataService
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
        let i = 0;
        this.backupBatteryUnitsArray = this.otherService.activeCard.getCardArray as BackupBatteryUnitsArray;
        const post$ = JSON.stringify(this.backupBatteryUnitsArray) === '{}' ?
            this.backupBatteryUnitsService.factory(this.otherService.activeCard) :
            of('').pipe(map(() => this.backupBatteryUnitsArray));
        post$.subscribe(
            (backupBatteryUnitsArray) => {
                this.backupBatteryUnitsArray = backupBatteryUnitsArray;
                this.otherService.activeCard.setCardArray = this.backupBatteryUnitsArray;
                this.columns = this.backupBatteryUnitsArray.title;
                this.srcData.data = this.backupBatteryUnitsArray.getBackupBatteryUnits;
                if (this.globalData.productType === PRODUCTTYPE.TIANCHI) {
                    for (i = 0; i < this.columns.length; i++) {
                        if (this.columns[i].title === 'OTHER_M2_INPOSITION_1_2') {
                            this.columns = this.columns.filter(item => item !== this.columns[i]);
                        }
                    }
                    i = 0;
                    while (this.srcData.data[i]) {
                        this.srcData.data[i].inPosition = '';
                        i++;
                    }
                }
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
