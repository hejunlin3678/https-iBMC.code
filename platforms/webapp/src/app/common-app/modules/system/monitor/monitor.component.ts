import { Component, OnInit } from '@angular/core';
import { Monitor } from './model/monitor';
import { MonitorService } from './services/monitor.service';
import { TiMessageService, TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { LoadingService, AlertMessageService, UserInfoService, SystemLockService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { formatEntry } from 'src/app/common-app/utils/common';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { monitorType, IMoitorThresholdPercent, platform, IChartsData } from './model/monitor.datatype';
import { GlobalData } from 'src/app/common-app/models';

@Component({
    selector: 'app-monitor',
    templateUrl: './monitor.component.html',
    styleUrls: ['./monitor.component.scss']
})
export class MonitorComponent implements OnInit {

    constructor(
        private service: MonitorService,
        private tiMessage: TiMessageService,
        private loading: LoadingService,
        private alert: AlertMessageService,
        private translate: TranslateService,
        private user: UserInfoService,
        private lockService: SystemLockService
    ) { }

    public monitor: Monitor = new Monitor();
    public cpuChartsData: IChartsData = null;
    public memoryChartsData: IChartsData = null;
    public isLowBrowserVersion: boolean = GlobalData.getInstance().getIsLowBrowserVer;
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock: boolean = false;
    public clearTip: string = this.translate.instant('COMMON_CLEAR');
    public refreshTip: string = this.translate.instant('COMMON_REFRESH');
    public editorIconTip: string =  this.translate.instant('COMMON_EDIT');
    public monitorModal;
    public editorValidationRules = [
        TiValidators.required,
        TiValidators.integer,
        TiValidators.rangeValue(0, 100)
    ];
    public editorValidation: TiValidationConfig = {
        tip: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 100]),
        tipPosition: 'top',
        errorMessage: {
            integer: this.translate.instant('VALID_INTEGER_INFO'),
            required: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 100]),
            rangeValue: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [0, 100])
        }
    };

    public cpu: IMoitorThresholdPercent = {
        title: this.translate.instant('MONITOR_CPU_USAGE'),
        current: this.translate.instant('MONITOR_CURRENT_VAL'),
        threshold: this.translate.instant('MONITOR_THRESHOLD'),
        tip: this.user.platform === platform.ARM ? this.translate.instant('MONITOR_CPU_ARM_INFO') :
            this.translate.instant('MONITOR_CPU_INFO'),
        noData: this.translate.instant('COMMON_NO_DATA'),
        state: false,
        editor: () => {
            this.monitor.editorCPUThresholdPercent = this.monitor.cpuThresholdPercent.toString();
        },
        cancel: () => {
            this.monitor.editorCPUThresholdPercent =
                this.monitor.cpuThresholdPercent === null ? null : `${this.monitor.cpuThresholdPercent.toString()}%`;
        },
        confirm: (value) => {
            const currentCPU = parseInt(value, 10);
            const param = {
                ThresholdPercent: currentCPU
            };
            this.monitor.editorCPUThresholdPercent = `${value}%`;
            this.setThresholdPercent(param, 'cpu');
        }
    };

    public memory: IMoitorThresholdPercent = {
        title: this.translate.instant('MONITOR_MEMORY_USAGE'),
        current: this.translate.instant('MONITOR_CURRENT_VAL'),
        threshold: this.translate.instant('MONITOR_THRESHOLD'),
        tip: this.user.platform === platform.ARM ? this.translate.instant('MONITOR_MEM_ARM_TIP')
            : this.translate.instant('MONITOR_MEM_INFO'),
        noData: this.translate.instant('COMMON_NO_DATA'),
        totalMemory: this.translate.instant('MONITOR_TOTAL_SYSTEM_MEMORY_GIB'),
        freeMemory: this.translate.instant('MONITOR_SYSTEM_MEMORY_FREE_GIB'),
        cacheMemory: this.translate.instant('MONITOR_SYSTEM_MEMORY_CACHED_GIB'),
        bufferMemory: this.translate.instant('MONITOR_SYSTEM_MEMORY_BUFFERS_GIB'),
        state: false,
        editor: () => {
            this.monitor.editorMemoryThresholdPercent = this.monitor.memoryThresholdPercent.toString();
        },
        cancel: () => {
            this.monitor.editorMemoryThresholdPercent =
                this.monitor.memoryThresholdPercent === null ? null : `${this.monitor.memoryThresholdPercent}%`;
        },
        confirm: (value) => {
            const currentMemory = parseInt(value, 10);
            const param = {
                ThresholdPercent: currentMemory
            };
            this.monitor.editorMemoryThresholdPercent = `${value}%`;
            this.setThresholdPercent(param, 'memory');
        }
    };
    public network: IMoitorThresholdPercent = {
        title: this.translate.instant('MONITOR_BANDWIDTH_USAGE'),
        threshold: this.translate.instant('MONITOR_THRESHOLD'),
        tip: this.translate.instant('MONITOR_NET_INFO'),
        state: false,
        editor: () => {
            this.monitor.editorNetBandwidthThresholdPercent = this.monitor.netBandwidthThresholdPercent.toString();
        },
        cancel: () => {
            this.monitor.editorNetBandwidthThresholdPercent =
                this.monitor.netBandwidthThresholdPercent === null ? null : `${this.monitor.netBandwidthThresholdPercent}%`;
        },
        confirm: (value) => {
            const currentNetwork = parseInt(value, 10);
            const param = {
                ThresholdPercent: currentNetwork
            };
            this.monitor.editorNetBandwidthThresholdPercent = `${value}%`;
            this.setThresholdPercent(param, 'network');
        }
    };
    public hardDisk: IMoitorThresholdPercent = {
        title: this.translate.instant('MONITOR_DISK_USAGE'),
        threshold: this.translate.instant('MONITOR_THRESHOLD'),
        tip: this.translate.instant('MONITOR_DISK_INFO'),
        state: false,
        editor: () => {
            this.monitor.editorHardDiskThresholdPercent = this.monitor.hardDiskThresholdPercent.toString();
        },
        cancel: () => {
            this.monitor.editorHardDiskThresholdPercent =
                this.monitor.hardDiskThresholdPercent === null ? null : `${this.monitor.hardDiskThresholdPercent}%`;
        },
        confirm: (value) => {
            const currentNetwork = parseInt(value, 10);
            const param = {
                ThresholdPercent: currentNetwork
            };
            this.monitor.editorHardDiskThresholdPercent = `${value}%`;
            this.setThresholdPercent(param, 'hardDisk');
        }
    };

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public refresh(type: string): void {
        switch (type) {
            case monitorType.cpu:
                this.cpu.state = true;
                this.service.getProcesserUsage().subscribe((response) => {
                    this.setCPUInfo(response.body);
                    this.cpu.state = false;
                }, (error) => {
                    this.cpu.state = false;
                });
                break;
            case monitorType.hardDisk:
                this.hardDisk.state = true;
                this.service.getDiskUsage().subscribe((response) => {
                    this.setDiskInfo(response.body);
                    this.hardDisk.state = false;
                }, (error) => {
                    this.hardDisk.state = false;
                });
                break;
            case monitorType.memory:
                this.memory.state = true;
                this.service.getMemoryUsage().subscribe((response) => {
                    this.setMemoryInfo(response.body);
                    this.memory.state = false;
                }, (error) => {
                    this.memory.state = false;
                });
                break;
            case monitorType.network:
                this.network.state = true;
                this.service.getNetworkBandwidthUsage().subscribe((response) => {
                    this.setNetworkInfo(response.body);
                    this.network.state = false;
                }, (error) => {
                    this.network.state = false;
                });
                break;
            default:
                break;
        }
    }
    public networkClear(): void {
        if (this.isSystemLock) {
            return;
        }
        this.monitorModal = this.tiMessage.open({
            id: 'networkClear',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                autofocus: false,
                primary: true
            },
            cancelButton: { },
            close: () => {
                const currentNetwork = this.monitor.networkType.getLabel();
                const param = {
                    BWUWaveTitle: currentNetwork
                };
                this.service.deleteMonitorNetwork(param).subscribe((response) => {
                    this.refresh('network');
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                });
            },
            dismiss(): void { }
        });
    }

    private setThresholdPercent(param: {ThresholdPercent: number}, type: string): void {
        switch (type) {
            case monitorType.cpu:
                this.cpu.state = true;
                this.service.setProcesserUsage(param).subscribe((response) => {
                    this.setCPUInfo(response.body);
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                    this.cpu.state = false;
                }, (error) => {
                    this.cpu.state = false;
                });
                break;
            case monitorType.hardDisk:
                this.hardDisk.state = true;
                this.service.setDiskUsage(param).subscribe((response) => {
                    this.setDiskInfo(response.body);
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                    this.hardDisk.state = false;
                }, (error) => {
                    this.hardDisk.state = false;
                });
                break;
            case monitorType.memory:
                this.memory.state = true;
                this.service.setMemoryUsage(param).subscribe((response) => {
                    this.setMemoryInfo(response.body);
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                    this.memory.state = false;
                }, (error) => {
                    this.memory.state = false;
                });
                break;
            case monitorType.network:
                this.network.state = true;
                this.service.setNetworkBandwidthUsage(param).subscribe((response) => {
                    this.setNetworkInfo(response.body);
                    this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                    this.network.state = false;
                }, (error) => {
                    this.network.state = false;
                });
                break;
            default:
                break;
        }
    }

    private setCPUInfo(data: any): void {
        const {
            usagePercent,
            thresholdPercent,
            editorThresholdPercent,
            history
        } = this.service.getMonitorCPUInfo(data);
        this.monitor.cpuUsagePercent = usagePercent;
        this.monitor.cpuThresholdPercent = thresholdPercent;
        this.monitor.editorCPUThresholdPercent = editorThresholdPercent;
        this.cpuChartsData = history;
    }

    private setMemoryInfo(data: any): void {
        const {
            history,
            memoryUsagePercent,
            memoryThresholdPercent,
            editorMemoryThresholdPercent,
            totalSystemMemoryGiB,
            systemMemoryCachedGiB,
            systemMemoryFreeGiB,
            systemMemoryBuffersGiB
        } = this.service.getMonitorMemoryInfo(data);
        this.memoryChartsData = history;
        this.monitor.memoryUsagePercent = memoryUsagePercent;
        this.monitor.memoryThresholdPercent = memoryThresholdPercent;
        this.monitor.editorMemoryThresholdPercent = editorMemoryThresholdPercent;
        this.monitor.totalSystemMemoryGiB = totalSystemMemoryGiB;
        this.monitor.systemMemoryCachedGiB = systemMemoryCachedGiB;
        this.monitor.systemMemoryFreeGiB = systemMemoryFreeGiB;
        this.monitor.systemMemoryBuffersGiB = systemMemoryBuffersGiB;
    }

    private setNetworkInfo(data: any): void {
        const {
            editorNetBandwidthThresholdPercent,
            netBandwidthThresholdPercent,
            history
        } = this.service.getMonitorNetWorkInfo(data);
        this.monitor.editorNetBandwidthThresholdPercent = editorNetBandwidthThresholdPercent;
        this.monitor.netBandwidthThresholdPercent = netBandwidthThresholdPercent;
        this.monitor.networkTypeList = history;
        this.monitor.networkType = this.monitor.networkTypeList[0];
    }

    private setDiskInfo(data: any): void {
        const {
            partitionLists,
            hardDiskThresholdPercent,
            editorHardDiskThresholdPercent,
        } = this.service.getMonitorDiskInfo(data);
        this.monitor.diskInfoLists = partitionLists;
        this.monitor.hardDiskThresholdPercent = hardDiskThresholdPercent;
        this.monitor.editorHardDiskThresholdPercent = editorHardDiskThresholdPercent;
    }

    private init(): void {
        this.monitor.iBMARunningStatus = this.user.iBMARunning;
        forkJoin([this.service.getProcesserUsage(), this.service.getMemoryUsage(),
            this.service.getNetworkBandwidthUsage(), this.service.getDiskUsage()]).subscribe((response) => {
                // CPU信息
                this.setCPUInfo(response[0].body);
                // memory信息
                this.setMemoryInfo(response[1].body);
                // 网口信息
                this.setNetworkInfo(response[2].body);
                // 硬盘信息
                this.setDiskInfo(response[3].body);
                this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
        });
    }

    ngOnInit(): void {
        this.loading.state.next(true);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.monitorModal) {
                this.monitorModal.dismiss();
            }
       });
        this.init();
    }

}
