import { Component, OnInit, Input, SimpleChanges } from '@angular/core';
import { TiTableColumns, TiTableSrcData, TiTableRowData } from '@cloud/tiny3';
import { ICpuTableRow, INpuTableRow } from '../../processor.datatype';

@Component({
    selector: 'app-processor-category',
    templateUrl: './processor-category.component.html',
    styleUrls: ['./processor-category.component.scss']
})
export class ProcessorCategoryComponent implements OnInit {

    @Input() tableData: ICpuTableRow[] | INpuTableRow[];

    @Input() loaded: boolean = false;

    @Input() columns: TiTableColumns[] = [];

    @Input() details = [];

    public colsNumber = 2;

    public colsGap = ['50px'];

    public displayed: ICpuTableRow[] | INpuTableRow[];

    public verticalAlign = 'middle';

    public labelWidth = '175px';

    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };

    constructor() { }

    ngOnInit() {
    }

    ngOnChanges(changes: SimpleChanges): void {
        this.srcData.data = this.tableData;
    }

    public beforeToggle(row: TiTableRowData) {
        row.showDetails = !row.showDetails;
    }

}
