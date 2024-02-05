import { Component, OnInit } from '@angular/core';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { PowerService } from './power.service';
import { LanguageService } from 'src/app/common-app/service/language.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';

@Component({
    selector: 'app-power',
    templateUrl: './power.component.html',
    styleUrls: ['./power.component.scss']
})
export class PowerComponent implements OnInit {

    constructor(
        private loading: LoadingService,
        private user: UserInfoService,
        private language: LanguageService,
        private service: PowerService
    ) { }

    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public powerConsumedWatts: any;
    public remainCapacityWattHour: any = 0;
    public powerInfos: any = [];
    public powerDetailForm: any = {
        // 纯文本展示表单
        textForm: {
            type: 'text',
            lineHeight: '34px',
            title: ''
        }
    };

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public init() {
        this.service.getpowerSupply().subscribe((response) => {
            this.powerConsumedWatts = response.powerConsumedWatts;
            this.remainCapacityWattHour = response.remainCapacityWattHour;
            this.powerInfos = response.powerArr;
           this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        this.init();
    }
}

