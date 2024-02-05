import { NetworkMonitor } from './networkMonitor';
import { DiskCharts } from './diskCharts';

export enum monitorType {
    cpu = 'cpu',
    hardDisk = 'hardDisk',
    memory = 'memory',
    network = 'network'
}

export enum platform {
    ARM = 'Arm'
}

export interface IMoitorThresholdPercent {
    title: string;
    current?: string;
    threshold: string;
    tip: string;
    state: boolean;
    noData?: string;
    totalMemory?: string;
    freeMemory?: string;
    cacheMemory?: string;
    bufferMemory?: string;
    editor(): void;
    cancel(): void;
    confirm(value: string): void;
}

export interface IChartsData {
    xData: number[];
    seriesData: number[];
    limitVal: number;
}

export interface ICPUData {
    usagePercent: number | string;
    thresholdPercent: number;
    editorThresholdPercent: string;
    history: IChartsData;
}

export interface IMemoryData {
    memoryUsagePercent: string | number;
    memoryThresholdPercent: number;
    editorMemoryThresholdPercent: string;
    totalSystemMemoryGiB: number;
    systemMemoryCachedGiB: number;
    systemMemoryFreeGiB: number;
    systemMemoryBuffersGiB: number;
    history: IChartsData;
}

export interface INetworkData {
    netBandwidthThresholdPercent: number;
    editorNetBandwidthThresholdPercent: string;
    history: NetworkMonitor[];
}

export interface IDiskData {
    partitionLists: DiskCharts[];
    hardDiskThresholdPercent: number;
    editorHardDiskThresholdPercent: string;
}
