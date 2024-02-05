import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { BiosService } from './services/bios.service';
import { Bios } from './model/bios';
import { LoadingService, SystemLockService } from 'src/app/common-app/service';
import { IBiosTab } from './model/bios-interface';

@Component({
    selector: 'app-bios',
    templateUrl: './bios.component.html',
    styleUrls: ['./bios.component.scss']
})
export class BiosComponent implements OnInit {

    // 创建两个Bios对象用于数据修改后的比较
    public biosModel: Bios = new Bios();
    public initializesBios: Bios = new Bios();
    public isSystemLock: boolean = false;

    constructor(
        private translate: TranslateService,
        private service: BiosService,
        private loading: LoadingService,
        private lockService: SystemLockService
    ) { }

    public bootDevice: IBiosTab = {
        title: this.translate.instant('BIOS_BOOT_DEVICE'),
        active: true,
        show: true
    };

    public cpu: IBiosTab = {
        title: this.translate.instant('BIOS_CPU_ADJUSTMENT'),
        active: false,
        show: false,
        tips: this.translate.instant('BIOS_CPU_TIP1') + this.translate.instant('BIOS_CPU_TIP2')
    };

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public init() {
        this.service.getBios().then((bios) => {
            // 获取两个对象，是因为有些value是对象，会导致修改时两个Bios对象的值都会发生变化（相当于做了深拷贝）
            const bootData = this.service.getBootData(bios[0].body);
            const initializesBoot = this.service.getBootData(bios[0].body);
            this.biosModel = new Bios();
            this.service.setBootModel(this.biosModel, bootData);
            this.service.setBootModel(this.initializesBios, initializesBoot);
            if (bios[1]) {
                const cpuData = this.service.getCPUData(bios[1].body);
                const initializesCPU = this.service.getCPUData(bios[1].body);
                const cpuState = cpuData.CPUPLimit !== null && cpuData.CPUTLimit !== null;
                if (cpuState && cpuData.CPUPScales.length > 0 && cpuData.CPUTScales.length > 0) {
                    this.cpu.show = true;
                    this.service.setCPUModel(this.biosModel, cpuData);
                    this.service.setCPUModel(this.initializesBios, initializesCPU);
                }
            }
        }).finally(() => {
            this.loading.state.next(false);
        });
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
        });
        this.init();
    }
}
