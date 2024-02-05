import { Component, OnInit } from '@angular/core';
import { AlarmConfigService } from './services';
import { TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { AlertMessageService, LoadingService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-alarm-config',
    templateUrl: './alarm-config.component.html',
    styleUrls: ['./alarm-config.component.scss']
})
export class AlarmConfigComponent implements OnInit {

    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    constructor(
        private configService: AlarmConfigService,
        private loadingService: LoadingService,
        private alert: AlertMessageService,
        private user: UserInfoService
    ) {
        this.displayed = [];
        this.srcData = {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
    }

    public isSystemLock = this.user.systemLockDownEnabled;
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isEdit: boolean = false;
    public buttonSave: boolean = true;
    public paramData: object = {};
    // 表格
    public columns: TiTableColumns = [
        {
            title: 'EMM_ALARM_CONFIG_PARTS',
            width: '30%'
        },
        {
            title: 'EMM_ALARM_CONFIG_SEVERITY',
            width: '70%'
        }
    ];
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    public levelChange() {
        this.paramData = {};
        this.srcData.data.forEach(item => {
            if (item.selectOption.id !== item.alarmLevel.key) {
                this.paramData[item.id] = item.selectOption.id;
            }
        });
        if (JSON.stringify(this.paramData) === '{}') {
            this.buttonSave = true;
        } else {
            this.buttonSave = false;
        }
    }
    ngOnInit(): void {
        this.getInfoInit();
    }
    public getInfoInit(): void {
        this.loadingService.state.next(true);
        this.configService.getAlarmConfigInfo().subscribe((response) => {
            this.srcData.data = response;
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }

    // 编辑
    public editConfig(): void {
        this.isEdit = true;
    }

    // 取消
    public cancelConfig(): void {
        this.isEdit = false;
        this.buttonSave = true;
        this.getInfoInit();
    }
    // 保存
    public saveConfig(): void {
        this.configService.saveAlarmConfigInfo(this.paramData).subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.buttonSave = true;
            this.isEdit = false;
            this.getInfoInit();
        }, (error) => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.loadingService.state.next(false);
            this.buttonSave = false;
        });
    }

}
