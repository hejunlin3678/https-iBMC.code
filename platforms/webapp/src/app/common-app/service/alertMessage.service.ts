import { Injectable, EventEmitter } from '@angular/core';

@Injectable({
    providedIn: 'root'
})
export class AlertMessageService {
    public type: string = 'prompt';
    public label: string = '';
    public open: boolean = true;
    public dismissOnTimeout: number = 1000;
    public eventEmit = new EventEmitter();

    constructor() { }

    public closeAlertMessage() {
        this.eventEmit.emit({type: this.type, label: this.label, open: false});
    }
}
