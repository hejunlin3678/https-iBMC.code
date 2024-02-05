import { Component } from '@angular/core';
import { TiLeftmenuItem } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-service',
    templateUrl: './service.component.html'
})
export class ServiceComponent {
    constructor(
        private translate: TranslateService
    ) {}

    public elementId: string = 'leftmenu';
    public reloadState: boolean = true;
    public collapsed: boolean = false;
    public headLabel: string = this.translate.instant('SERVICE_MANAGEMENT');
    public items = CommonData.mainMenu[4].children;
    public active: TiLeftmenuItem = this.items[0];

}
