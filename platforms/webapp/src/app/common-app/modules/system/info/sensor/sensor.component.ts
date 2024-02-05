import { Subscription } from 'rxjs';
import { Component, OnInit } from '@angular/core';
import { TiTableSrcData, TiTableColumns, TiTableRowData } from '@cloud/tiny3';
import { LoadingService } from 'src/app/common-app/service';
import { SensorService } from './service';

@Component({
    selector: 'app-sensor',
    templateUrl: './sensor.component.html',
    styleUrls: ['./sensor.component.scss']
})
export class SensorComponent implements OnInit {

    private getSensorSub: Subscription;

    public totalNumber: number = 0;

    public currentPage: number = 1;

    public loaded = false;

    public pageSize: { options: number[], size: number} = {
        options: [15, 30, 50],
        size: 15
    };

    public displayed: TiTableRowData[] = [];

    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };

    public searchValue = '';

    public searchWords = [this.searchValue];

    public searchKeys = [];

    public columns: TiTableColumns[] = [
        {
            title: 'SENSOR_NUMBER',
            width: '5%',
            content: 'id',
        },
        {
            title: 'SENSOR_TITLE',
            width: '15%',
            content: 'name',
            sortKey: 'name',
            compareFn: this.sensorServ.sortBy('Name')
        },
        {
            title: 'SENSOR_CURRENT_VAL',
            width: '10%',
            content: 'readingValue',
        },
        {
            title: 'SENSOR_STATE',
            width: '10%',
            content: 'status',
        },
        {
            title: 'SENSOR_EMERGENCY_LOWER',
            width: '10%',
            content: 'lowerThresholdFatal',
        },
        {
            title: 'SENSOR_CRITICAL_LOWER',
            width: '10%',
            content: 'lowerThresholdCritical',
        },
        {
            title: 'SENSOR_SLIGHT_LOWER',
            width: '10%',
            content: 'lowerThresholdNonCritical',
        },
        {
            title: 'SENSOR_SLIGHT_TOP',
            width: '10%',
            content: 'upperThresholdNonCritical',
        },
        {
            title: 'SENSOR_CRITICAL_TOP',
            width: '10%',
            content: 'upperThresholdCritical',
        },
        {
            title: 'SENSOR_EMERGENCY_TOP',
            width: '10%',
            content: 'upperThresholdFatal',
        }
    ];

    public onSearch(value: string) {
        this.searchWords[0] = value;
    }

    public clear() {
        this.searchWords[0] = '';
    }

    // 因为status进行了修改，所以需要对searchKeys进行修改
    private filterSearchKeys() {
        const unSearchedKeys = ['status'];
        const addedKeys = ['statusText'];
        const searchKeys = this.columns.map((column) => {
            if (unSearchedKeys.indexOf(column.content) >= 0) {
                return;
            }
            return column.content;
        });
        this.searchKeys = searchKeys.filter((key) => key);
        this.searchKeys = this.searchKeys.concat(addedKeys);
    }

    constructor(
        private loadingServ: LoadingService,
        private sensorServ: SensorService,
    ) { }

    ngOnInit() {
        this.loadingServ.state.next(true);
        this.filterSearchKeys();

        this.getSensorSub = this.sensorServ.getData().subscribe((data) => {
            this.srcData.data = data;
            this.totalNumber = data.length;

            this.loadingServ.state.next(false);
            this.loaded = true;
        }, () => {
            this.loadingServ.state.next(false);
            this.loaded = true;
        });
    }

    ngOnDestroy(): void {
        this.getSensorSub.unsubscribe();
    }

}
