import { Component, Input, OnInit } from '@angular/core';
import { BiosService } from '../../services/bios.service';
import { AlertMessageService, UserInfoService } from 'src/app/common-app/service';
import { Bios } from '../../model/bios';
import { TranslateService } from '@ngx-translate/core';
import { ICPU } from '../../model/bios-interface';

@Component({
    selector: 'app-cpu',
    templateUrl: './cpu.component.html',
    styleUrls: ['./cpu.component.scss']
})
export class CpuComponent implements OnInit {

    constructor(
        private user: UserInfoService,
        private service: BiosService,
        private alert: AlertMessageService,
        private translate: TranslateService
    ) { }

    public isPrivileges: boolean = this.user.hasPrivileges(['OemPowerControl']);

    @Input() public biosModel: Bios;
    @Input() public initializesBios: Bios;
    @Input() public isSystemLock: boolean;

    public buttonDisable: boolean = true;

    public cpu: ICPU = {
        controlMethod: {
            label: this.translate.instant('BIOS_CONTROL_METHOD_LABEL')
        },
        operatingFrequency: {
            label: this.translate.instant('BIOS_OPERATING_FREQUENCY_LABEL'),
            tip: this.translate.instant('BIOS_OPERATING_FREQUENCY_TIP1') + this.translate.instant('BIOS_OPERATING_FREQUENCY_TIP2'),
        },
        workingHours: {
            label: this.translate.instant('BIOS_DUTY_CYCLE_LABEL'),
            tip: this.translate.instant('BIOS_DUTY_CYCLE_TIP1') + this.translate.instant('BIOS_DUTY_CYCLE_TIP2'),
        },
        frequency: {
            disabled: false,
            min: 0,
            tipFormatterFn(value: string) {
                return `P${value}`;
            }
        },
        freeTime: {
            disabled: false,
            min: 0,
            tipFormatterFn(value: string) {
                return `T${value}`;
            }
        }
    };

    private getParam() {
        const param = {};
        if (this.biosModel.cpuPState !== this.initializesBios.cpuPState) {
            param['Pstate'] = this.biosModel.cpuPState;
        }

        if (this.biosModel.cpuTState !== this.initializesBios.cpuTState) {
            param['Tstate'] = this.biosModel.cpuTState;
        }
        return param;
    }

    private buttonMonitor() {
        const param = this.getParam();
        this.buttonDisable = JSON.stringify(param) === '{}';
    }
    public frequencyChange() {
        this.buttonMonitor();
    }

    public freeTimeChange() {
        this.buttonMonitor();
    }

    public cpuSave() {
        this.buttonDisable = true;
        const param = this.getParam();
        this.service.setCPU(param).subscribe((response) => {
            const cpuData = this.service.getCPUData(response.body);
            this.service.setCPUModel(this.biosModel, cpuData);
            const initializesCPU = this.service.getCPUData(response.body);
            this.service.setCPUModel(this.initializesBios, initializesCPU);
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
        }, (error) => {
            const cpuData = this.service.getCPUData(error.error.data);
            this.service.setCPUModel(this.biosModel, cpuData);
            const initializesCPU = this.service.getCPUData(error.error.data);
            this.service.setCPUModel(this.initializesBios, initializesCPU);
        });
    }

    ngOnInit(): void {
        this.cpu.frequency.disabled = this.biosModel.cpuPState === null;
        this.cpu.freeTime.disabled = this.biosModel.cpuTState === null;
    }
}
