import { Component, Input, OnInit, ViewEncapsulation } from '@angular/core';
import { PowerSleep } from '../../model/power-sleep';

@Component({
    selector: 'app-power-sleep',
    templateUrl: './power-sleep.component.html',
    styleUrls: ['./power-sleep.component.scss'],
    encapsulation: ViewEncapsulation.None
})
export class PowerSleepComponent implements OnInit {

    @Input() powerSleep: PowerSleep;
    @Input() param;
    public saveBtn: boolean = true;
    constructor() { }

    public sleepEnable = {
        label: 'POWER_SLEEP_ENABLE',
        switchState: false,
        change: () => {
            this.sleepEnable.switchState = !this.sleepEnable.switchState;
            this.monitorBtn();
        }
    };

    public sleepMode = {
        label: 'POWER_SLEEP_MODE',
        selected: '',
        radioList: [{
            id: 'N+1',
            key: 'N+1'
        }, {
            id: 'N+N',
            key: 'N+N'
        }],
        disabled: false,
        change : () => {
            this.monitorBtn();
        }
    };
    public getParam() {
        const param = {};
        if (this.powerSleep.sleepEnable !== this.sleepEnable.switchState) {
            param['sleepEnable'] = this.sleepEnable.switchState;
        }
        if (this.powerSleep.sleepMode !== this.sleepMode.selected) {
            param['sleepMode'] = this.sleepMode.selected;
        }
        this.param = param;
        return param;
    }

    public monitorBtn() {
        const param = this.getParam();
        if (this.sleepEnable.switchState) {
            this.sleepMode.selected = this.sleepMode.radioList[0].key;
        } else if (!this.sleepEnable.switchState) {
            this.sleepMode.selected = '';
        }
        this.saveBtn = JSON.stringify(param) === '{}';
    }

    close(): void {
    }

    dismiss(): void {
    }

    ngOnInit(): void {
        this.sleepEnable.switchState = this.powerSleep.sleepEnable;
        this.sleepMode.selected = this.powerSleep.sleepMode;
    }

}
