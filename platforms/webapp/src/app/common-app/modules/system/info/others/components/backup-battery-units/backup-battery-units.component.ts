import { Component, OnInit } from '@angular/core';
import { OthersService } from '../../others.service';
import { BackupBatteryUnitsService } from './backup-battery-units.service';
import { TiTableSrcData, TiTableRowData, TiTableColumns } from '@cloud/tiny3';
import { BackupBatteryUnitsArray } from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { LoadingService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { of } from 'rxjs/internal/observable/of';

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
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
    }
}
