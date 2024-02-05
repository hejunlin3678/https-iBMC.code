import { Component, OnInit } from '@angular/core';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';

@Component({
    selector: 'app-footer',
    templateUrl: './footer.component.html',
    styleUrls: ['./footer.component.scss']
})
export class FooterComponent implements OnInit {
    public ibmcTime: string = '';
    public copyRight: string = '';
    constructor(
        private global: GlobalDataService
    ) { }

    ngOnInit() {
        this.copyRight = this.global.copyright;
        this.global.ibmcTime.subscribe((res) => {
            this.ibmcTime = res.ibmcTime ? res.ibmcTime : this.ibmcTime;
            if (res.offset) {
                const timeArr = this.ibmcTime.split('(');
                this.ibmcTime = timeArr[0] + res.offset;
            }
        });
    }

    public cancelBubble(ev: MouseEvent) {
        ev.stopPropagation();
        ev.preventDefault();
    }
}
