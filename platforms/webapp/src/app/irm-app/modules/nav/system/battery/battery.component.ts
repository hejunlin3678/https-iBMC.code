import { Component, OnInit } from '@angular/core';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { BatteryService } from './battery.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';

@Component({
    selector: 'app-battery',
    templateUrl: './battery.component.html',
    styleUrls: ['./battery.component.scss']
})
export class BatteryComponent implements OnInit {

    constructor(
        private service: BatteryService,
        private user: UserInfoService,
        private loading: LoadingService,
        private alert: AlertMessageService
    ) { }

    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public ratedCapacityWattHour: any;
    public remainCapacityWattHour: any;
    public batteryInfos: any = [];
    public batteryDetailForm: any = {
        // 纯文本展示表单
        textForm: {
            type: 'text',
            lineHeight: '34px'
        }
    };

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public init() {
        this.service.getBatteryInfos().subscribe((response: any) => {
            this.ratedCapacityWattHour = response.ratedCapacityWattHour;
            this.remainCapacityWattHour = response.remainCapacityWattHour;
            this.batteryInfos = response.batteryArr;
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
        });
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        this.init();
    }

}
