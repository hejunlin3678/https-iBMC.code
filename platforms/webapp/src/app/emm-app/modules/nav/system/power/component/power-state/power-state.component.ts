import { Component, OnInit } from '@angular/core';
import { TiModalRef, TiModalService, TiTableColumns, TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { LoadingService, AlertMessageService, UserInfoService, GlobalDataService } from 'src/app/common-app/service';
import { PowerSleepComponent } from './component/power-sleep/power-sleep.component';
import { PowerSleep } from './model/power-sleep';
import { PowerStateService } from './service/power-state.service';
@Component({
    selector: 'app-power-state',
    templateUrl: './power-state.component.html',
    styleUrls: ['./power-state.component.scss']
})
export class PowerStateComponent implements OnInit {

    constructor(
        private loading: LoadingService,
        private tiModal: TiModalService,
        private service: PowerStateService,
        private alert: AlertMessageService,
        private globalData: GlobalDataService,
        private user: UserInfoService
    ) { }
    public isPrivileges = this.user.hasPrivileges(['OemPowerControl']);
    public showPowerSetBtn: boolean = true;
    public powerSleep: PowerSleep;
    // 表示表格实际呈现的数据（开发者默认设置为[]即可）
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public columns: TiTableColumns[] = [
        {
            title: 'HOME_POWER'
        },
        {
            title: 'POWER_TYPE'
        },
        {
            title: 'POWER_CURRENT_RATED',
            width: '350px'
        },
        {
            title: 'POWER_SOFTWARE_HAEDWARE_VERSION'
        },
        {
            title: 'STATUS'
        }
    ];

    public openSleep(): void {
        if (!this.powerSleep.isPowerHealth) {
            this.alert.eventEmit.emit({type: 'error', label: 'POWER_SUPPLYSETTINGS'});
            return;
        }
        this.tiModal.open(PowerSleepComponent, {
            id: 'powerSleepModal',
            modalClass: 'powerSleepModal',
            context: {
                powerSleep: this.powerSleep,
                param: null
            },
            close: (modalRef: TiModalRef): void => {
                if (modalRef.content.instance.param) {
                    const param = {
                        SleepModeEnabled : modalRef.content.instance.param?.sleepEnable,
                        SleepMode : modalRef.content.instance.param?.sleepMode
                    };
                    this.loading.state.next(true);
                    this.service.setPowerSleep(param).subscribe((powerResp) => {
                        if (powerResp.status === 200) {
                            setTimeout(() => {
                                // 请求成功显示提示，刷新数据
                                this.getPowerInfo();
                                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            }, 4000);
                        }
                    }, () => {
                        this.loading.state.next(false);
                    });
                }
            },
            dismiss: (): void => {
            }
        });
    }

    public getPowerInfo() {
        this.service.getPower().subscribe(( powerResp ) => {
            const productInfo = this.service.getPowerInfo(powerResp.body);
            this.srcData = {
                data: productInfo,
                state: {
                    searched: false,
                    sorted: false,
                    paginated: false
                }
            };
            this.powerSleep = new PowerSleep(this.service.getPowerSleep(powerResp.body)) ;
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        this.getPowerInfo();
        this.globalData.powerCappingSupport.then((res: boolean) => {
            this.showPowerSetBtn = res;
        });
    }

}
