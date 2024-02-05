import { Component, OnInit } from '@angular/core';
import { PowerTabs } from './models/powerOnOff';
import { PowerOnOffService } from './services/power-on-off.service';
import { TiTableSrcData } from '@cloud/tiny3';
import { GlobalDataService, LoadingService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-power-on-off',
    templateUrl: './power-on-off.component.html',
    styleUrls: ['./power-on-off.component.scss']
})
export class PowerOnOffComponent implements OnInit {

    constructor(
        private service: PowerOnOffService,
        private loading: LoadingService,
        private user: UserInfoService,
        private globalData: GlobalDataService,
    ) { }
    public tabs: PowerTabs[] = [
        {
            id: 'calculateNode',
            title: 'EMM_ALARM_CONFIG_COMPUTE_NODE',
            active: true
        }, {
            id: 'SwitchModul',
            title: 'POWERONOFF_SWITCH_MODUL',
            active: false
        }
    ];
    public nodeSrcData: TiTableSrcData;
    public modulSrcData: TiTableSrcData;
    public currentTab: string = 'calculateNode';
    public oemPowerControl = this.user.hasPrivileges(['OemPowerControl']);
    public showSwiTab: boolean = true;

    ngOnInit(): void {
        this.nodeSrcData = {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        this.modulSrcData = {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        this.globalData.swiSupport.then(res => {
            this.showSwiTab = res;
        });
        this.getCalNodeData();
    }

    /**
     * 上下电控制页签切换
     * @param isActive
     * @param type
     */
    public tabChange(isActive: boolean, type: string): void {
        if (isActive && type === 'calculateNode') {
            this.currentTab = 'calculateNode';
        } else if (isActive && type === 'SwitchModul') {
            this.currentTab = 'SwitchModul';
        }
    }

    /**
     * 获取计算节点表格数据
     */
    public getCalNodeData() {
        this.loading.state.next(true);
        this.service.getNodeData().subscribe((res) => {
            const viewData = res?.body?.Components;
            const powerData = this.service.dataInit(viewData);
            this.nodeSrcData.data = powerData.nodeData;
            this.modulSrcData.data = powerData.modulData;
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }
}
