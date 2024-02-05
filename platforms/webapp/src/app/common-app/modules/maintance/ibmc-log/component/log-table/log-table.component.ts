import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
    selector: 'app-log-table',
    templateUrl: './log-table.component.html',
    styleUrls: ['./log-table.component.scss']
})
export class LogTableComponent implements OnInit {
    constructor() { }
    public displayed = [];
    @Input() type;
    @Input() columns;
    @Input() srcData;
    @Input() currentPage;
    @Input() pageSize;
    @Input() totalNumber;
    @Input() load;
    @Output() curuentPageUpdate = new EventEmitter<number>();
    ngOnInit() {
    }
    public pageUpdate(page) {
        this.curuentPageUpdate.emit(page);
    }
}
