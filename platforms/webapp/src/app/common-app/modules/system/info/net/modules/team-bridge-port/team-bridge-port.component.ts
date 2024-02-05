import { Component, OnInit, Input } from '@angular/core';

@Component({
    selector: 'app-team-bridge-port',
    templateUrl: './team-bridge-port.component.html',
    styleUrls: ['./team-bridge-port.component.scss']
})
export class TeamBridgePortComponent implements OnInit {
    public displayed = [];
    @Input() srcData;
    @Input() columns;
    constructor() { }

    ngOnInit() {
    }

}
