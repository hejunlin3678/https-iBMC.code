import { NetworkMonitor } from './networkMonitor';
import { DiskCharts } from './diskCharts';

export class Monitor {

    constructor() { }
    public iBMARunningStatus: boolean;
    public cpuUsagePercent: number | string;
    public cpuThresholdPercent: number;
    public editorCPUThresholdPercent: string;
    public memoryUsagePercent: number | string;
    public memoryThresholdPercent: number;
    public editorMemoryThresholdPercent: string;
    public totalSystemMemoryGiB: number;
    public systemMemoryCachedGiB: number;
    public systemMemoryFreeGiB: number;
    public systemMemoryBuffersGiB: number;
    public networkTypeList: NetworkMonitor[];
    public networkType: NetworkMonitor;
    public netBandwidthThresholdPercent: number;
    public editorNetBandwidthThresholdPercent: string;
    public diskInfoLists: DiskCharts[];
    public hardDiskThresholdPercent: number;
    public editorHardDiskThresholdPercent: string;

}
