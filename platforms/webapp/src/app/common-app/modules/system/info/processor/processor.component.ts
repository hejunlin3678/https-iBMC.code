import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs/internal/Subscription';
import { ProcessorService } from './service';
import { IRow, ICpuTableRow, INpuTableRow, IDetail } from './processor.datatype';
import { LoadingService } from 'src/app/common-app/service';
import { TiTableColumns } from '@cloud/tiny3';

@Component({
    selector: 'app-processor',
    templateUrl: './processor.component.html',
    styleUrls: ['./processor.component.scss']
})
export class ProcessorComponent implements OnInit {

    private getProcessorSub: Subscription;

    private handledData: IRow;

    public cpuData: ICpuTableRow[];

    public npuData: INpuTableRow[];

    public cpuColumns: TiTableColumns[];

    public npuColumns: TiTableColumns[];

    public cpuDetails: IDetail[];

    public npuDetails: IDetail[];

    public totalNumber: number;

    public activedNumber: number;

    public cpuNumDetail: string;

    public npuNumDetail: string;

    public showMenu: boolean;

    public activatedProcessor: string;

    public loaded = false;

    public showCpu(): void {
        this.activatedProcessor = 'cpu';
        this.totalNumber = this.handledData.totalCpuLength;
        this.activedNumber = this.handledData.cpu.length;
    }

    public showNpu(): void {
        this.activatedProcessor = 'npu';
        this.totalNumber = this.handledData.totalNpuLength;
        this.activedNumber = this.handledData.npu.length;
    }

    constructor(
        private processorServ: ProcessorService,
        private loadingServ: LoadingService,
    ) { }

    ngOnInit() {
        this.loadingServ.state.next(true);
        this.getProcessorSub = this.processorServ.getData().subscribe((data) => {
            this.handledData = data;
            this.cpuData = this.handledData.cpu;
            this.npuData = this.handledData.npu;
            this.cpuNumDetail = `${this.cpuData.length}/${this.handledData.totalCpuLength}`;
            this.npuNumDetail = `${this.npuData.length}/${this.handledData.totalNpuLength}`;

            // 同时有cpu和npu时，需要左侧显示菜单
            this.showMenu = this.cpuData.length > 0 && this.npuData.length > 0;
            if (this.cpuData && this.cpuData.length) {
                this.cpuData.forEach(cpu => {
                    if (cpu.partNumber === undefined) {
                        this.generateCpu('hide');
                    } else {
                        this.generateCpu('show');
                    }
                });
            }
            this.generateNpu();

            /**
             * 1.如果既没有cpu也没有npu，不显示表格，只显示“总数”，“在位数”;
             * 2.如有cpu（不管有没有npu），显示cpu表格数据;
             * 3.如果没有cpu，则显示npu表格数据;
             */
             if (this.cpuData.length === 0 && this.npuData.length === 0) {
                this.totalNumber = this.handledData.totalCpuLength;
                this.activedNumber = this.handledData.cpu.length;
                this.endLoading();
                return;
            }

            if (this.cpuData.length > 0) {
                this.showCpu();
                this.endLoading();
                return;
            }

            if (this.cpuData.length === 0 && this.npuData.length > 0) {
                this.showNpu();
                this.endLoading();
            }
        }, () => {
            this.loaded = true;
            this.loadingServ.state.next(false);
        });
    }

    ngOnDestroy(): void {
        this.getProcessorSub.unsubscribe();
    }

    private generateCpu(data) {
        const cpuColumns = [
            {
                title: '',
            },
            {
                title: 'COMMON_NAME',
                width: '8%',
                content: 'name'
            },
            {
                title: 'COMMON_MANUFACTURER',
                width: '14%',
                content: 'manufacturer',
                hide: this.showMenu,
            },
            {
                title: 'PROCESSOR_MODEL',
                width: '24%',
                content: 'model'
            },
            {
                title: 'PROCESSOR_FREQENCY',
                width: '10%',
                content: 'frequencyMHz'
            },
            {
                title: 'PROCESSOR_CORECOUNT_THREADCOUNT',
                width: '17%',
                content: 'totalCores'
            },
            {
                title: 'PROCESSOR_FDM_ALARM_CACHE',
                width: '17%',
                content: 'cacheKiB'
            },
            {
                title: 'PROCESSOR_STATE',
                width: '10%',
                content: 'processorState',
                needPipe: true,
            }
        ];

        this.cpuColumns = cpuColumns.filter((column) => !column.hide);

        this.cpuDetails = [
            {
                label: 'COMMON_NAME',
                content: 'name',
                hide: false,
            },
            {
                label: 'COMMON_MANUFACTURER',
                content: 'manufacturer',
                hide: false,
            },
            {
                label: 'PROCESSOR_MODEL',
                content: 'model',
                hide: false,
            },
            {
                label: 'PROCESSOR_ID',
                content: 'id',
                hide: false,
            },
            {
                label: 'PROCESSOR_FREQENCY',
                content: 'frequencyMHz',
                hide: false,
            },
            {
                label: 'PROCESSOR_CORECOUNT_THREADCOUNT',
                content: 'totalCores',
                hide: false,
            },
            {
                label: 'PROCESSOR_FDM_ALARM_CACHE',
                content: 'cacheKiB',
                hide: false,
            },
            {
                label: 'PROCESSOR_STATE',
                content: 'processorState',
                needPipe: true,
                hide: false,
            },
            {
                label: 'OTHER_PART_NUMBER',
                content: 'partNumber',
                hide: data === 'hide',
            },
            {
                label: 'COMMON_SERIALNUMBER',
                content: 'serialNumber',
                hide: false,
            },
            {
                label: 'PROCESSOR_OTHER',
                content: 'otherParameters',
                labelTop: true,
                hide: false,
            },
        ];
    }

    private generateNpu() {
        const npuColumns = [
            {
                title: '',
            },
            {
                title: 'COMMON_NAME',
                content: 'name',
                width: '10%'
            },
            {
                title: 'COMMON_MANUFACTURER',
                content: 'manufacturer',
                width: '15%',
                hide: this.showMenu,
            },
            {
                title: 'PROCESSOR_MODEL',
                content: 'model',
                width: '20%',
            },
            {
                title: 'HOME_POWERW',
                content: 'powerWatts',
                width: '17%'
            },
            {
                title: 'COMMON_FIXED_VERSION',
                content: 'firmwareVersion',
                width: '18%'
            },
            {
                title: 'PROCESSOR_DIE_ID',
                content: 'serialNumber',
                width: '20%'
            }
        ];

        this.npuColumns = npuColumns.filter((column) => !column.hide);

        this.npuDetails = [
            {
                label: 'COMMON_NAME',
                content: 'name',
                hide: false,
            },
            {
                label: 'COMMON_MANUFACTURER',
                content: 'manufacturer',
                hide: false,
            },
            {
                label: 'PROCESSOR_MODEL',
                content: 'model',
                hide: false,
            },
            {
                label: 'HOME_POWERW',
                content: 'powerWatts',
                hide: false,
            },
            {
                label: 'COMMON_FIXED_VERSION',
                content: 'firmwareVersion',
                hide: false,
            },
            {
                label: 'PROCESSOR_DIE_ID',
                content: 'serialNumber',
                hide: false,
            },
        ];
    }

    private endLoading() {
        this.loaded = true;
        this.loadingServ.state.next(false);
    }

}
