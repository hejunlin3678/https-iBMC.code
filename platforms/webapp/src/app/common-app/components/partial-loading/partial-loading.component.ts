import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-partial-loading',
  templateUrl: './partial-loading.component.html',
  styleUrls: ['./partial-loading.component.scss']
})
export class PartialLoadingComponent implements OnInit {

    constructor() { }
    @Input() state: boolean;

    public scroll(ev) {
        ev.stopPropagation();
        ev.preventDefault();
    }

    public click(ev) {
        if (ev.target.id === 'partialLoading') {
            ev.stopPropagation();
            ev.preventDefault();
        }
    }

    ngOnInit(): void {
    }

}
