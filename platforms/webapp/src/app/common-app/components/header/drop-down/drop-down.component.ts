import { Component, OnInit, AfterViewChecked, Input, ViewChild, ElementRef } from '@angular/core';
import { Router } from '@angular/router';
import { GlobalData } from 'src/app/common-app/models/global-data.model';


@Component({
	selector: 'app-drop-down',
	templateUrl: './drop-down.component.html',
	styleUrls: ['./drop-down.component.scss']
})
export class DropDownComponent implements OnInit, AfterViewChecked {
	@Input() list: any[];
	@ViewChild('dropdown', { static: true }) dropDown: ElementRef;
	constructor(
		private router: Router
	) { }

	public webStyle = GlobalData.getInstance().getWebStyle;
	public display: string = 'none';
	public mainMemuWidth: number;

	public show(width: number): void {
		this.display = 'block';
		this.mainMemuWidth = width;
	}
	public hide(): void {
		this.display = 'none';
	}
	ngOnInit() {
	}

	goRouter(event, router) {
		// 必须阻止事件冒泡，防止触发一级菜单的点击事件
		event.stopPropagation();
		this.router.navigate(router);
		this.hide();
	}

	ngAfterViewChecked(): void {
		const offset = this.dropDown.nativeElement.offsetWidth;
		this.dropDown.nativeElement.style.left = (this.mainMemuWidth - offset) / 2 + 'px';
	}
}
