import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
  selector: 'app-net-port',
  templateUrl: './net-port.component.html',
  styleUrls: ['./net-port.component.scss']
})
export class NetPortComponent implements OnInit {

    public displayed = [];
    @Input() srcData;
    @Input() columns;
    @Input() type;
    @Output() sliderClick =  new EventEmitter();
    constructor() { }

    ngOnInit() {}
    public toggle(row, index) {
        row.showDetails = !row.showDetails;
        row.type = this.type;
        this.sliderClick.emit(row.portName + index);
    }

}
