import { Component, OnInit } from '@angular/core';
import { AlertMessageService, LoadingService } from 'src/app/common-app/service';
import { Chassis } from './model/chassis';
import { PowerCappingService } from './service/power-capping.service';

@Component({
    selector: 'app-power-capping-emm',
    templateUrl: './power-capping-emm.component.html',
    styleUrls: ['./power-capping-emm.component.scss']
})
export class PowerCappingEmmComponent implements OnInit {

    public chassis: Chassis;
    public chassisParam = null;
    public bladeParam = null;
    public isManual: string = null;
    public saveBtn = true;
    public capValue = null;

    constructor(
        private loading: LoadingService,
        private service: PowerCappingService,
        private alert: AlertMessageService,
    ) { }

    public init(): void {
        this.service.getPower().subscribe(powerResp => {
            this.chassis = this.service.getPowerCappingInfo(powerResp.body);
            this.isManual = this.chassis.capMode;
            this.capValue = this.chassis.capValue;
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }

    public getChassisParam(param): void {
        this.isManual = param?.LimitMode || this.chassis.capMode;
        this.capValue = param?.LimitInWatts || this.chassis.capValue;
        this.chassisParam = param;
        this.mointorBtn();
    }

    public getBladeParam(param): void {
        this.bladeParam = param;
        this.mointorBtn();
    }

    private mointorBtn(): void {
        if (this.chassisParam || this.bladeParam) {
            this.saveBtn = false;
        } else {
            this.saveBtn = true;
        }
    }

    public powerCapSave(): void {
        this.service.setPower(this.chassisParam, this.bladeParam).subscribe(powerResp => {
            this.chassis = this.service.getPowerCappingInfo(powerResp.body);
            this.isManual = this.chassis.capMode;
            this.capValue = this.chassis.capValue;
            this.loading.state.next(true);
            this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
            this.init();
        }, () => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.loading.state.next(false);
        });
    }

    ngOnInit(): void {
        this.loading.state.next((true));
        this.init();
    }

}
