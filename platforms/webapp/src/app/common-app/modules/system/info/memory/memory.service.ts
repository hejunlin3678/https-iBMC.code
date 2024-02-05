import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { isEmptyProperty } from 'src/app/common-app/utils';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { MemoryInfo } from './models/memoryInfo.model';
import { of, forkJoin, Observable } from 'rxjs';
import { Memory } from './models/memory.model';
import { DetailInfo } from './models/detailInfo.model';
import { AEPInfo } from './models/aepInfo.model';
import { GlobalDataService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class MemoryService {
    private readonly DCPMM_DEVICE = 'DCPMM';
    private isEmpty = '--';
    constructor(
        private http: HttpService,
        private globalData: GlobalDataService
    ) { }

    factory(): Observable<MemoryInfo> {
        const getMemoryInfo$ = this.http.get('/UI/Rest/System/Memory').pipe(pluck('body'));
        const getPlatform$ = of('').pipe(map(() => 'x86' ));
        return forkJoin([getMemoryInfo$, getPlatform$]).pipe(
            map(
                ([memoryInfo, platForm]) => {
                    return this.factoryMemoryInfo(memoryInfo, platForm);
                }
            )
        );
    }
    private factoryMemoryInfo(data: any, platForm: any): MemoryInfo {
        const maxNum = data.MaxNumber;
        const lists = data.List;
        return new MemoryInfo(
            platForm,
            maxNum,
            lists.length,
            this.transformTableData(lists, platForm)
        );
    }

    private transformTableData(lists: any[], platForm): Memory[] {
        return lists.map((list) => {
            const detailInfo = new DetailInfo(
                list.General.DeviceLocator,
                list.General.Position || this.isEmpty,
                list.General.Manufacturer || this.isEmpty,
                list.General.CapacityMiB === null || list.General.CapacityMiB === undefined ? this.isEmpty : `${list.General.CapacityMiB} MB`,
                list.General.OperatingSpeedMHz === null || list.General.OperatingSpeedMHz === undefined ? this.isEmpty : `${list.General.OperatingSpeedMHz} MT/s`,
                list.General.AllowedSpeedsMHz === null || list.General.AllowedSpeedsMHz === undefined  ? this.isEmpty : `${list.General.AllowedSpeedsMHz} MT/s`,
                list.General.DeviceType || this.isEmpty,
                list.General.MinVoltageMillivolt === null || list.General.MinVoltageMillivolt === undefined  ? this.isEmpty : `${list.General.MinVoltageMillivolt} mV`,
                list.General.TypeDetail || this.isEmpty,
                list.General.DataWidthBits === null || list.General.DataWidthBits === undefined  ? this.isEmpty : `${list.General.DataWidthBits} bit`,
                list.General.PartNumber,
                list.General.RankCount === null || list.General.RankCount === undefined  ? this.isEmpty : `${list.General.RankCount} rank`,
                list.General.BomNumber,
                list.General.SerialNumber === null || list.General.SerialNumber === undefined  ? this.isEmpty : list.General.SerialNumber,
                list.General.DimmTemp === null  ? this.isEmpty : `${list.General.DimmTemp} ℃`,
                platForm,
                this.globalData
            );
            const isAEP = list.DCPMM;
            let aepInfo = null;
            if (isAEP) {
                aepInfo = new AEPInfo(
                    list.DCPMM.RemainingServiceLifePercent === null || list.DCPMM.RemainingServiceLifePercent === undefined
                        ? this.isEmpty : `${list.DCPMM.RemainingServiceLifePercent} %`,
                    list.DCPMM.MediumTemperatureCelsius === null || list.DCPMM.MediumTemperatureCelsius === undefined
                        ? this.isEmpty : `${list.DCPMM.MediumTemperatureCelsius} ℃`,
                    list.DCPMM.FirmwareRevision === null || list.DCPMM.FirmwareRevision === undefined
                        ? this.isEmpty : list.DCPMM.FirmwareRevision,
                    list.DCPMM.VolatileRegionSizeLimitMiB === null || list.DCPMM.VolatileRegionSizeLimitMiB === undefined
                        ? this.isEmpty : `${list.DCPMM.VolatileRegionSizeLimitMiB} MB`,
                    list.DCPMM.ControllerTemperatureCelsius === null || list.DCPMM.ControllerTemperatureCelsius === undefined
                        ? this.isEmpty : `${list.DCPMM.ControllerTemperatureCelsius} ℃`,
                    list.DCPMM.PersistentRegionSizeLimitMiB === null || list.DCPMM.PersistentRegionSizeLimitMiB === undefined
                        ? this.isEmpty : `${list.DCPMM.PersistentRegionSizeLimitMiB} MB`,
                );
            }
            const memory = new Memory(
                isEmptyProperty(list.General.DeviceLocator + (isAEP ? '(PMem)' : '') === '' ? '--' : list.General.DeviceLocator + (isAEP ? '(PMem)' : '')) || '--',
                isEmptyProperty(list.General.Manufacturer) || '--',
                list.General.CapacityMiB === null || list.General.CapacityMiB === undefined ? this.isEmpty : `${list.General.CapacityMiB} MB`,
                list.General.OperatingSpeedMHz === null || list.General.OperatingSpeedMHz === undefined ? this.isEmpty : `${list.General.OperatingSpeedMHz} MT/s`,
                list.General.AllowedSpeedsMHz === null || list.General.AllowedSpeedsMHz === undefined ? this.isEmpty : `${list.General.AllowedSpeedsMHz} MT/s`,
                isEmptyProperty(list.General.DeviceType) || '--',
                isEmptyProperty(list.General.Position) || '--',
                detailInfo,
                aepInfo,
                platForm,
            );
            const DCPMM = null;
            return memory;
        });
    }
}
