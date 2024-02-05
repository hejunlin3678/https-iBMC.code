import { Component, OnInit, Input, ViewChild, ElementRef } from '@angular/core';

@Component({
    selector: 'app-second-password',
    templateUrl: './second-password.component.html',
    styleUrls: ['./second-password.component.scss']
})
export class SecondPasswordComponent implements OnInit {
    @Input() password: string = '';
    @Input() state: boolean = false;
    @Input() active: boolean = false;
    @ViewChild('confirm', { static: true }) confirm: ElementRef;
    constructor() { }

    ngOnInit() {
    }

    public cleanError() {
        this.state = false;
        if (this.password === '') {
            this.active = false;
        } else {
            this.active = true;
        }
    }

    public close() {

    }
    public dismiss() {

    }

    public enter(ev) {
        if (ev.keyCode === 13) {
            this.confirm.nativeElement.click();
        }
    }
}
