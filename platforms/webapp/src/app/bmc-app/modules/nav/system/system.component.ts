import { Component } from '@angular/core';
import { TiLeftmenuItem } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { NavMenuService } from 'src/app/bmc-app/services/navMenu.service';

@Component({
	selector: 'app-system',
	templateUrl: './system.component.html',
	styleUrls: ['./system.component.scss']
})
export class SystemComponent {
	public elementId: string;
	public headLabel: string;
	public items: TiLeftmenuItem[];
	public active: TiLeftmenuItem;
	constructor(
		private translate: TranslateService,
		private navService: NavMenuService
	) {
		this.elementId = 'leftmenu';
		this.headLabel = this.translate.instant('COMMON_HEADER_SYSTEM');
		this.items = this.navService.mainMenu[1].children;
		this.active = this.items[0];
	}
}
