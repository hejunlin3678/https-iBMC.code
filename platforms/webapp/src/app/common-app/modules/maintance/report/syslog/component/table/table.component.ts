import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { TiTableRowData } from '@cloud/tiny3';

@Component({
    selector: 'app-table',
    templateUrl: './table.component.html',
    styleUrls: ['./table.component.scss']
})
export class TableComponent implements OnInit {

    constructor() { }
    public displayed: TiTableRowData = [];

    @Input() srcData;
    @Input() columns;
    @Input() isSystemLock;
    @Input() hasPermission;
    @Output() operateChange = new EventEmitter();
    ngOnInit() {
    }
    public edit(row) {
        if (this.isSystemLock || !this.hasPermission) {
            return;
        }
        const params = {
            key: 'edit',
            value: row
        };
        this.operateChange.emit(params);
    }
    public cancel(row) {
        const params = {
            key: 'cancel',
            value: row
        };
        this.operateChange.emit(params);
    }
    public save(row) {
        const params = {
            key: 'save',
            value: row
        };
        this.operateChange.emit(params);
    }
    public test(row) {
        if (!this.hasPermission) {
            return;
        }
        const params = {
            key: 'test',
            value: row
        };
        this.operateChange.emit(params);
    }
}
