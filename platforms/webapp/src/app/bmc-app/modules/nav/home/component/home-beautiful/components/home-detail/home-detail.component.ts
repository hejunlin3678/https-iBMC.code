import { Router } from '@angular/router';

import { Component, Input } from '@angular/core';
import { IServerDetail } from '../../../../home.datatype';


@Component({
    selector: 'app-home-detail-beautiful',
    templateUrl: './home-detail.component.html',
    styleUrls: ['./home-detail.component.scss'],
})
export class HomeDetailBeautifulComponent {

    @Input() loading = true;

    @Input() serverDetail: IServerDetail;

    public colsNumber = 1;

    public colsGap = ['100px'];

    constructor(
        private router: Router
    ) { }

    public navigatorToMore() {
        const moreLinkUrl = ['/navigate/system/info'];
        this.router.navigate(moreLinkUrl);
    }
}
