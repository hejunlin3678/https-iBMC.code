import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { NetworkMonitor } from '../model/networkMonitor';
import { NetworkCharts } from '../model/networkCharts';
import { DiskCharts } from '../model/diskCharts';
import { IChartsData, ICPUData, IMemoryData, IDiskData, INetworkData } from '../model/monitor.datatype';
import { Observable } from 'rxjs/internal/Observable';

@Injectable({
    providedIn: 'root'
})
export class MonitorService {
    constructor(
        private http: HttpService
    ) { }

    public deleteMonitorNetwork(param: {BWUWaveTitle: string}): Observable<any> {
        return this.http.post( '/UI/Rest/System/NetworkBandwidthUsage/ClearHistory', param);
    }
    public getProcesserUsage(): Observable<any> {
        return this.http.get('/UI/Rest/System/ProcesserUsage');
    }
    public getMemoryUsage(): Observable<any> {
        return this.http.get('/UI/Rest/System/MemoryUsage');
    }
    public getDiskUsage(): Observable<any> {
        return this.http.get('/UI/Rest/System/DiskUsage');
    }
    public getNetworkBandwidthUsage(): Observable<any> {
        return this.http.get('/UI/Rest/System/NetworkBandwidthUsage');
    }
    public setProcesserUsage(param: {ThresholdPercent: number}): Observable<any> {
        return this.http.patch('/UI/Rest/System/ProcesserUsage', param);
    }
    public setMemoryUsage(param: {ThresholdPercent: number}): Observable<any> {
        return this.http.patch('/UI/Rest/System/MemoryUsage', param);
    }
    public setDiskUsage(param: {ThresholdPercent: number}): Observable<any> {
        return this.http.patch('/UI/Rest/System/DiskUsage', param);
    }
    public setNetworkBandwidthUsage(param: {ThresholdPercent: number}): Observable<any> {
        return this.http.patch('/UI/Rest/System/NetworkBandwidthUsage', param);
    }

    public getMonitorCPUInfo(data: any): ICPUData {
        const cpuInfo: ICPUData = {
            usagePercent: data.UsagePercent === null ? '--' : data.UsagePercent,
            thresholdPercent: data.ThresholdPercent,
            editorThresholdPercent: data.ThresholdPercent === null ? null : `${data.ThresholdPercent}%`,
            history: this.cpuChartsInfo(data)
        };
        return cpuInfo;
    }

    public getMonitorMemoryInfo(data: any): IMemoryData {
        const memoryInfo: IMemoryData = {
            memoryUsagePercent: data.UsagePercent === null ? '--' : data.UsagePercent,
            memoryThresholdPercent: data.ThresholdPercent,
            editorMemoryThresholdPercent: data.ThresholdPercent === null ? null : `${data.ThresholdPercent}%`,
            totalSystemMemoryGiB: data.CapacityGiB,
            systemMemoryCachedGiB: data.CachedGiB,
            systemMemoryFreeGiB: data.FreeGiB,
            systemMemoryBuffersGiB: data.BuffersGiB,
            history: this.memoryChartsInfo(data)
        };
        return memoryInfo;
    }

    public getMonitorNetWorkInfo(data: any): INetworkData {
        const netWorkInfo = {
            netBandwidthThresholdPercent: data.ThresholdPercent,
            editorNetBandwidthThresholdPercent: data.ThresholdPercent === null ? null : `${data.ThresholdPercent}%`,
            history: this.networkChartsInfo(data)
        };
        return netWorkInfo;
    }

    public getMonitorDiskInfo(data: any): IDiskData {
        const partitionLists = data.Partitions;
        const partitionListsData = [];
        for (const item of partitionLists) {
            const partitionData = new DiskCharts(item.PartitionName, item.CapacityGB, item.UsagePercent, item.UsedGB);
            partitionListsData.push(partitionData);
        }
        const diskInfo = {
            partitionLists: partitionListsData,
            hardDiskThresholdPercent: data.ThresholdPercent,
            editorHardDiskThresholdPercent: data.ThresholdPercent === null ? null : `${data.ThresholdPercent}%`
        };
        return diskInfo;
    }

    public cpuChartsInfo(data: any): IChartsData {
        if (!data) {
            return;
        }
        const timeData = [];
        const datas = [];

        if (data.History.length > 0) {
            for (const item of data.History) {
                timeData.push(item.Time);
                datas.push(item.UtilisePercent);
            }
        }
        const targetData: IChartsData = {
            xData: timeData,
            seriesData: datas,
            limitVal: data.ThresholdPercent
        };
        return targetData;
    }

    public memoryChartsInfo(data: any): IChartsData {
        if (!data) {
            return;
        }
        const xData = [];
        const seriesData = [];

        if (data.History.length > 0) {
            for (const item of data.History) {
                xData.push(item.Time);
                seriesData.push(item.UtilisePercent);
            }
        }
        const targetData: IChartsData = {
            xData,
            seriesData,
            limitVal: data.ThresholdPercent
        };
        return targetData;
    }

    public networkChartsInfo(data: any): NetworkMonitor[] {
        const tempArr = [];
        const historys = data.History;
        for (let j = 0; j < historys.length; j++) {
            const history = historys[j];
            const ports = [];
            const times = [];
            const datas = [];
            // 是否存在当前利用率
            if (history.CurrentUtilisePercents) {
                const currentUtilisePercents = history.CurrentUtilisePercents;
                for (let i = 1; i <= currentUtilisePercents.length; i++) {
                    const current = currentUtilisePercents[i - 1];
                    if (current !== null) {
                        ports.push(`Port${i} ( ${parseInt(current, 10)}% )`);
                    } else {
                        ports.push(`Port${i} ( - - )`);
                    }
                }
            } else {
                for (let i = 1; i <= history.PortCount; i++) {
                    ports.push(`Port${i} ( - - )`);
                }
            }
            const utilisePercents = history.UtilisePercents;
            for (const utilisePercent of utilisePercents) {
                times.push(utilisePercent.Time);
                const utilisePercentPort = utilisePercent.UtilisePercent;
                // 构建echarts对象数组拼接
                for (let i = 0; i < utilisePercentPort.length; i++) {
                    // 存在数组添加值，不存在则创建数组项则添加默认值
                    if (datas[i]) {
                        datas[i].push(utilisePercentPort[i]);
                    } else {
                        datas[i] = [utilisePercentPort[i]];
                    }
                }
            }
            const networkCharts = new NetworkCharts(ports, times, datas);
            const networkMonitor = new NetworkMonitor(j, history.BWUWaveTitle, networkCharts);
            tempArr.push(networkMonitor);
        }
        return tempArr;
    }
}
