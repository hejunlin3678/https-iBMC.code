import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-maintance',
    templateUrl: './maintance.component.html',
    styleUrls: ['./maintance.component.scss']
})
export class MaintanceComponent {

    constructor(
        private translate: TranslateService
    ) { }
    public headLabel: string = this.translate.instant('MAINTANCE');
    public elementId: string = 'leftmenu';
    public collapsed: boolean = false;
    public reloadState: boolean = true;
    public items = CommonData.mainMenu[2].children;
    public active = this.items[0];
}
