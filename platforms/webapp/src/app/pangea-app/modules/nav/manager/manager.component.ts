import { Component } from '@angular/core';
import { TiLeftmenuItem } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-manager',
    templateUrl: './manager.component.html'
})
export class ManagerComponent {

    constructor(
        private translate: TranslateService
    ) {

    }
    public elementId: string = 'leftmenu';
    public reloadState: boolean = true;
    public collapsed: boolean = false;
    public headLabel: string = this.translate.instant('IBMC_HEAD_IBMC_MAGAGE');
    public items = CommonData.mainMenu[5].children;
    public active: TiLeftmenuItem = this.items[0];

}
