import { Component, OnInit } from '@angular/core';
import { MemoryInfo } from './models/memoryInfo.model';
import { MemoryService } from './memory.service';
import { TranslateService } from '@ngx-translate/core';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { LoadingService } from 'src/app/common-app/service';

@Component({
    selector: 'app-memory',
    templateUrl: './memory.component.html',
    styleUrls: ['./memory.component.scss']
})
export class MemoryComponent implements OnInit {

    public memoryInfo: MemoryInfo;

    public columns: TiTableColumns[];
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    public loaded: boolean;
    constructor(
        public i18n: TranslateService,
        private memoryService: MemoryService,
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
        this.loaded = false;
        this.memoryService.factory().subscribe(
            (memoryInfo) => {
                this.memoryInfo = memoryInfo;
                this.columns = this.memoryInfo.getColumns;
                this.srcData.data = this.memoryInfo.memoryArr;
                this.loading.state.next(false);
            }
        );
        this.loaded = true;
    }

}
