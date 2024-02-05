import { Component, OnInit } from '@angular/core';
import { LCDService } from './lcd.service';
import { TiTableRowData, TiTableSrcData, TiTableColumns } from '@cloud/tiny3';

@Component({
    selector: 'app-lcd',
    templateUrl: './lcd.component.html',
    styleUrls: ['./lcd.component.scss']
})
export class LCDComponent implements OnInit {
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    constructor(
        private lcdService: LCDService
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
    }
    public columns: TiTableColumns[] = [
        {
            title: 'COMMON_NAME',
            width: '50%'
        },
        {
            title: 'OTHER_VERSION',
            width: '50%'
        }
    ];
    ngOnInit(): void {
        this.lcdService.factory().subscribe(
            (res) => {
                this.srcData.data = res;
            }
        );
    }
}
