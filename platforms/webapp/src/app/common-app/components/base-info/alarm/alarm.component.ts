import { Component, OnInit, OnChanges, Input, SimpleChanges } from '@angular/core';
import { Router } from '@angular/router';

@Component({
    selector: 'app-alarm',
    templateUrl: './alarm.component.html',
    styleUrls: ['./alarm.component.scss']
})
export class AlarmComponent implements OnInit, OnChanges {
    @Input() alarmInfo;
    @Input() id;
    public critical: number = 0;
    public major: number = 0;
    public minor: number = 0;
    constructor(
        private rotuer: Router
    ) { }

    ngOnInit(): void {

    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.alarmInfo.currentValue) {
            this.critical = changes.alarmInfo.currentValue.critical;
            this.major = changes.alarmInfo.currentValue.major;
            this.minor = changes.alarmInfo.currentValue.minor;
        }
    }

    public gotoAlarm() {
        this.rotuer.navigate(['navigate/maintance/alarm']);
    }
}
