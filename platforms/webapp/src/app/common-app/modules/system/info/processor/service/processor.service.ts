import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { IRow, ICpuTableRow, INpuTableRow } from '../processor.datatype';

@Injectable({
    providedIn: 'root'
})
export class ProcessorService {

    public getData() {
        return this.getProcessors();
    }

    private getProcessors(): Observable<IRow> {
        const url = '/UI/Rest/System/Processor';
        return this.http.get(url).pipe(
            pluck('body'),
            map((data) => this.factoryProcessor(data))
        );
    }

    private factoryProcessor({CPU: cpus, NPU: npus}): IRow {
        const responseData: IRow = {
            cpu: [],
            npu: [],
            totalCpuLength: 0,
            totalNpuLength: 0,
        };
        if (cpus && cpus.length) {
            cpus.forEach(cpu => {
                responseData.totalCpuLength++;
                const cpuData = this.parseCpuData(cpu);
                if (cpuData) {
                    responseData.cpu.push(cpuData);
                }
            });
        }
        if (npus && npus.length) {
            npus.forEach(npu => {
                responseData.totalNpuLength++;
                const npuData = this.parseNpuData(npu);
                if (npuData) {
                    responseData.npu.push(npuData);
                }
            });
        }

        return responseData;
    }

    private parseCpuData(data): ICpuTableRow {
        if (data.State === 'Absent') {
            return null;
        }
        const {
            L1CacheKiB: l1CacheKiB,
            L2CacheKiB: l2CacheKiB,
            L3CacheKiB: l3CacheKiB,
            PartNumber: partNumber,
            FrequencyMHz: frequencyMHz,
            DeviceLocator: deviceLocator,
            Manufacturer: manufacturer,
            Model: model,
            Identification: processorId,
            OtherParameters: otherParameters,
            SerialNumber: serialNumber,
            TotalCores: totalCores,
            TotalThreads: totalThreads,
            EnabledSetting: enabledSetting,
        } = data;

        const noDataStr = '--';
        const thread = totalThreads === null ? noDataStr : `${totalThreads} threads`;
        const processorState = enabledSetting ? 'CPU_COMMON_ENABLE' : 'CPU_COMMON_DISABLE';
        let cacheKiB = `${l1CacheKiB}/${l2CacheKiB}/${l3CacheKiB} KB`;
        cacheKiB = cacheKiB.replace(/null/g, noDataStr);
        const cores = (totalCores !== null && totalCores !== undefined) ? `${totalCores} cores/${thread}` : (noDataStr + '/' + noDataStr);
        return {
            name: deviceLocator || noDataStr,
            manufacturer: manufacturer || noDataStr,
            model: model || noDataStr,
            id: processorId || noDataStr,
            frequencyMHz: frequencyMHz ? `${frequencyMHz} MHz` : noDataStr,
            cacheKiB,
            otherParameters: (otherParameters !== null && otherParameters !== undefined) ? otherParameters : noDataStr,
            partNumber: (partNumber !== null && partNumber !== 'N/A') ? partNumber : noDataStr,
            serialNumber: serialNumber || noDataStr,
            totalCores: cores,
            processorState
        };
    }

    private parseNpuData(data): INpuTableRow {
        if (data.State === 'Absent') {
            return null;
        }
        const {
            Name: name,
            Manufacturer: manufacturer,
            Model: model,
            PowerWatts: powerWatts,
            FirmwareVersion: firmwareVersion,
            SerialNumber: serialNumber,
        } = data;
        const noDataStr = '--';
        return {
            name,
            manufacturer: manufacturer || noDataStr,
            model: model || noDataStr,
            powerWatts: powerWatts == null ? noDataStr : `${powerWatts} W`,
            firmwareVersion: firmwareVersion || noDataStr,
            serialNumber: serialNumber || noDataStr,
        };
    }

    constructor(
        private http: HttpService,
    ) { }

}
