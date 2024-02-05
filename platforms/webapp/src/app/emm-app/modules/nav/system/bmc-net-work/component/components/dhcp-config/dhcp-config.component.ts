import { Component, OnInit, Output, EventEmitter } from '@angular/core';

@Component({
    selector: 'app-dhcp-config',
    templateUrl: './dhcp-config.component.html',
    styleUrls: ['./dhcp-config.component.scss']
})
export class DhcpConfigComponent implements OnInit {
    @Output() activeChange: EventEmitter<any> = new EventEmitter<boolean>();
    constructor() {

    }

    ngOnInit(): void {
    }

    public activeAll() {
        this.activeChange.emit(true);
    }

    public deActiveAll() {
        this.activeChange.emit(false);
    }

}
