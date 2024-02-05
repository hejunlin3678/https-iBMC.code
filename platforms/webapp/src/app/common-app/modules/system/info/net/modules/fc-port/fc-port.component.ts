import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
    selector: 'app-fc-port',
    templateUrl: './fc-port.component.html',
    styleUrls: ['./fc-port.component.scss']
})
export class FcPortComponent implements OnInit {
    public displayed = [];
    @Input() srcData;
    @Input() columns;
    @Input() type;
    @Output() sliderClick =  new EventEmitter();
    constructor() { }

    ngOnInit() { }
    public toggle(row, i: number) {
        row.showDetails = !row.showDetails;
        row.type = this.type;
        this.sliderClick.emit(i);
    }
}
