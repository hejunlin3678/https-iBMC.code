import { Component } from '@angular/core';
import { TiLeftmenuItem } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';

@Component({
	selector: 'app-user',
	templateUrl: './user.component.html',
	styleUrls: ['./user.component.scss']
})
export class UserComponent {
	constructor(
		private translate: TranslateService
	) { }
	public elementId: string = 'leftmenu';
	public headLabel: string = this.translate.instant('COMMON_HEADER_USERS');
	public items = CommonData.mainMenu[3].children;
	public active: TiLeftmenuItem = this.items[0];
}
