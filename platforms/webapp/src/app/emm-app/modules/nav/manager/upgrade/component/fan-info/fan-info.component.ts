import { Component, OnInit } from '@angular/core';
import { FanInfoService } from './fan-info.service';
import { TiTableRowData, TiTableSrcData, TiTableColumns } from '@cloud/tiny3';

@Component({
    selector: 'app-fan-info',
    templateUrl: './fan-info.component.html',
    styleUrls: ['./fan-info.component.scss']
})
export class FanInfoComponent implements OnInit {
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    constructor(
        private fanInfoService: FanInfoService
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
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '25%'
        },
        {
            title: 'EMM_UPGRADE_BOARDTYE',
            width: '25%'
        },
        {
            title: 'EMM_UPGRADE_HARDWARE_VERSION',
            width: '25%'
        },
        {
            title: 'EMM_UPGRADE_SOFTWARE_VERSION',
            width: '25%'
        }
    ];
    ngOnInit(): void {
        this.fanInfoService.factory().subscribe(
            (fansInfo) => {
                this.srcData.data = fansInfo;
            }
        );
    }
}
