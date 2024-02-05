import { Component, OnInit, Input } from '@angular/core';
import { TiTableSrcData, TiTableColumns } from '@cloud/tiny3';

@Component({
	selector: 'app-home-irm-asset',
	templateUrl: './home-irm-asset.component.html',
	styleUrls: ['./home-irm-asset.component.scss']
})
export class HomeIrmAssetComponent implements OnInit {
	@Input() srcDataZc: any;
	public showLoginInfor = false;


	constructor(

	) { }

	public columns: TiTableColumns = [{
		title: 'IRM_U_RANGE',
		width: '8%'
	},
	{
		title: 'IRM_HEALTH_STATE',
		width: '10%'
	},
	{
		title: 'IRM_DEVICE_MODEL',
		width: '8%'
	},
	{
		title: 'IRM_AUTH_NUMBER',
		width: '13%'
	},
	{
		title: 'IRM_VENDOR',
		width: '9%'
	},
	{
		title: 'IRM_PART_NUMBER',
		width: '12%'
	},
	{
		title: 'IRM_PCB_VERSION',
		width: '10%'
	},
	{
		title: 'IRM_BOARD_ID',
		width: '8%'
	},
	{
		title: 'IRM_FIRMWARE_VERSION',
		width: '12%'
	}];
	public displayed = [];
	public srcData: TiTableSrcData = {
		data: [],
		state: {
			searched: false,
			sorted: false,
			paginated: false
		}
	};
	public currentPage: number = 1;
	public totalNumber: number = 0;
	public pageSize: { options: any[], size: number } = {
		options: [5],
		size: 5
	};

	ngOnInit() {
		this.initData();
	}

	private initData(): void {
		this.srcData.data = this.srcDataZc;
		this.totalNumber = this.srcDataZc.length;
	}

}
