import { Component, OnInit, Input, ViewChild, ElementRef } from '@angular/core';

@Component({
    selector: 'app-emm-second',
    templateUrl: './emm-second.component.html',
    styleUrls: ['./emm-second.component.scss']
})
export class EmmSecondComponent implements OnInit {
    @Input() state: boolean = false;
    @Input() active: boolean = false;
    @ViewChild('confirm', { static: true }) confirm: ElementRef;
    public password: string = '';
    constructor() { }

    ngOnInit(): void {
    }

    public cleanError() {
        this.state = false;
        if (this.password === '') {
            this.active = false;
        } else {
            this.active = true;
        }
    }

    public enter(ev: KeyboardEvent): void {
        // tslint:disable-next-line: deprecation
        if (ev.keyCode === 13) {
            this.confirm.nativeElement.click();
        }
    }

    public close() {

    }
    public dismiss() {

    }

}
