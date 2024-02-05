import { Component } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';

@Component({
	selector: 'app-ibma-modal',
	templateUrl: './ibma-modal.component.html',
	styleUrls: ['./ibma-modal.component.scss']
})
export class IbmaModalComponent {

	constructor(
		private translate: TranslateService
	) { }

	public modal = {
		header: this.translate.instant('IBMA_INSTALL_NOTE'),
		des1: this.translate.instant('IBMA_INSTALL_DES1'),
		des2: this.translate.instant('IBMA_INSTALL_DES2'),
		des3: this.translate.instant('IBMA_INSTALL_DES3'),
		system: this.translate.instant('IBMA_INSTALL_TABLETH1'),
		path: this.translate.instant('IBMA_INSTALL_TABLETH2'),
		helpInformation: this.translate.instant('IBMA_INSTALL_DES4'),
		start:  this.translate.instant('IBMA_START_KVM'),
		cancel: this.translate.instant('COMMON_CANCEL')
	};
	public operSystemInfo: {system: string, path: string}[] = [{
		system: 'Linux',
		path: 'Linux/install.sh'
	}];

	/**
	 * 模板中实际调用的是Modal服务提供的close和dismiss方法，并非此处定义的方法；
	 * 在此处定义close和dismiss方法只是为了避免生产环境打包时报错
	 */
	close(): void {
	}
	dismiss(): void {
	}

}
