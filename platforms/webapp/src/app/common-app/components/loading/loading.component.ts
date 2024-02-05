import { Component, OnInit } from '@angular/core';
import { LoadingService } from 'src/app/common-app/service/loading.service';
@Component({
    selector: 'app-loading',
    templateUrl: './loading.component.html',
    styleUrls: ['./loading.component.scss']
})
export class LoadingComponent implements OnInit {

    constructor(
        private loadingService: LoadingService
    ) { }

    public state: boolean = true;

    ngOnInit() {
        this.loadingService.state.subscribe((value) => {
            this.state = value;
        });
    }

    public scroll(ev) {
        ev.stopPropagation();
        ev.preventDefault();
    }

    public click(ev) {
        if (ev.target.id === 'loading') {
            ev.stopPropagation();
            ev.preventDefault();
        }
    }

}
