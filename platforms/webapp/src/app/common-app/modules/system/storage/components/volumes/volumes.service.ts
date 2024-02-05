import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { isEmptyProperty } from 'src/app/common-app/utils';
import { isEmptyBoolean, getUnitConversionByte, computeCapacity } from '../../utils/storage.utils';
import { Observable } from 'rxjs/internal/Observable';
import { Volume, VolumeNode, VolumeStaticModel, RaidType} from '../../models';
import { SpanService } from '../span/span.service';
import { PMCEditModel } from '../../models/volume/pmc-edit.model';
import { TranslateService } from '@ngx-translate/core';

@Injectable({
    providedIn: 'root'
})

export class VolumesService {

    constructor(
        private http: HttpService,
        private i18n: TranslateService
    ) { }

    factory(node: VolumeNode): Observable<{volume: Volume, isCache: boolean}> {
        return this.getData(node.getUrl).pipe(
            map((bodyData) => {
                const name = bodyData.Name || '--';
                const raidLevel = bodyData.VolumeRaidLevel || '--';
                let ioSize = bodyData.OptimumIOSizeBytes === null ? '--' : (bodyData.OptimumIOSizeBytes / 1024).toFixed(0);
                ioSize = isEmptyProperty(ioSize, ' KB');
                const readPolicy = bodyData.DefaultReadPolicy;
                const writePolicy = bodyData.DefaultWritePolicy;
                const cachePolicy = bodyData.DefaultCachePolicy;
                const bgiEnable = bodyData.BGIEnabled;
                const initType = bodyData.InitializationMode;
                const cacheEnable = {
                    name: 'IValue',
                    label: isEmptyBoolean(bodyData.SSDCachecadeVolume, 'COMMON_YES', 'COMMON_NO'),
                    value: bodyData.SSDCachecadeVolume
                };
                const consistency = isEmptyBoolean(bodyData.ConsistencyCheck, 'STORE_ENABLED', 'STORE_DISABLED') || 'STORE_DISABLED';
                const status = bodyData.State || '--';
                const capacityBytes = getUnitConversionByte(bodyData.CapacityBytes);
                const sscd = bodyData.SSDCachingEnabled;
                const currentRead = bodyData.CurrentReadPolicy;
                const currentWrite = bodyData.CurrentWritePolicy;
                const currentCache = bodyData.CurrentCachePolicy;
                const accessPolicy = bodyData.AccessPolicy;
                const driveCache = bodyData.DriveCachePolicy;
                const bootEnable = bodyData.BootEnabled;
                const osDriveName = isEmptyProperty(bodyData.OSDriveName);
                const isFGIProgress = bodyData.FGIProgress && bodyData.FGIProgress <= 100 && bodyData.FGIProgress > 0;
                const fgiProgress =  isFGIProgress ? isEmptyProperty(bodyData.FGIProgress, ' %') : '';
                const twoCache = bodyData.SSDCachecadeVolume;
                const associatedVolumes = bodyData.AssociatedVolumes ? bodyData.AssociatedVolumes.join('/') : '';
                const cacheLineSize = bodyData.CacheLineSize;
                const accelerationMethod = bodyData.AccelerationMethod;
                const isCache = bodyData.SSDCachecadeVolume || false;
                const rebuildState = bodyData.RebuildState;
                let tmpRebuildState = null;
                if (rebuildState === 'DoneOrNotRebuilt') {
                    tmpRebuildState = 'FDM_STOPPED';
                } else if (rebuildState === 'Rebuilding') {
                    const rebuildProgress = bodyData.RebuildProgress == null ? '' : bodyData.RebuildProgress;
                    tmpRebuildState = this.i18n.instant('FDM_RECONSTRUCTION') + '  ' + rebuildProgress;
                }
                const construction = isEmptyProperty(tmpRebuildState);

                SpanService.creatSpan(bodyData.Spans || [], node.labelId);
                const volume: Volume = new Volume(
                    name,
                    raidLevel,
                    ioSize,
                    readPolicy,
                    writePolicy,
                    cachePolicy,
                    bgiEnable,
                    initType,
                    cacheEnable,
                    consistency,
                    status,
                    capacityBytes,
                    sscd,
                    currentRead,
                    currentWrite,
                    currentCache,
                    accessPolicy,
                    driveCache,
                    bootEnable,
                    osDriveName,
                    fgiProgress,
                    twoCache,
                    node.getRaidType,
                    associatedVolumes,
                    cacheLineSize,
                    accelerationMethod,
                    construction
                );

                if (node.getRaidType === RaidType.PMC || node.getRaidType === RaidType.HBA) {
                    let capacityUnit = VolumeStaticModel.capacityUnit[0];
                    // 容量单位是比特，需转成MB或GB、TB
                    let capacity = bodyData.CapacityBytes || 0;
                    if (capacity < 1024 * 1024 * 1024) {
                        capacity = (capacity / (1024 * 1024)).toFixed(3);
                    } else if (capacity < 1024 * 1024 * 1024 * 1024) {
                        capacity = (capacity / (1024 * 1024 * 1024)).toFixed(3);
                        capacityUnit = VolumeStaticModel.capacityUnit[1];
                    } else if (capacity < 1024 * 1024 * 1024 * 1024 * 1024) {
                        capacity = (capacity / (1024 * 1024 * 1024 * 1024)).toFixed(3);
                        capacityUnit = VolumeStaticModel.capacityUnit[2];
                    }
                    const {capacity: maxCapacity , unit: maxCapacityUnit} = computeCapacity(bodyData.MaxResizableBytes || 0);
                    const modifyingCapacity  = bodyData.CapacityBytes < bodyData.MaxResizableBytes;
                    // PMC普通逻辑盘支持的属性
                    const stripeSizeBytes = bodyData.OptimumIOSizeBytes ? bodyData.OptimumIOSizeBytes : 16 * 1024;
                    const speedMethod = bodyData.AccelerationMethod;

                    /**
                     * 加速方法取值：ControllerCache、IOBypass、None、maxCache
                     * 普通逻辑盘只有前3项
                     * 被其他maxChe盘所关联后，则有4项
                     */
                    if (speedMethod === 'maxCache') {
                        if (VolumeStaticModel.speedMethod.length < 4) {
                            VolumeStaticModel.speedMethod.push({id: 'maxCache', label: 'maxCache', name: 'IOptions'});
                        }
                    } else {
                        if (VolumeStaticModel.speedMethod.length >= 4) {
                            VolumeStaticModel.speedMethod.length = 3;
                        }
                    }
                    const bootPriority = bodyData.BootPriority || null;
                    const pmcEditModel = new PMCEditModel(
                        name,
                        bootPriority,
                        capacity,
                        accelerationMethod,
                        stripeSizeBytes,
                        capacityUnit,
                        writePolicy,
                        maxCapacity,
                        maxCapacityUnit,
                        modifyingCapacity
                    );
                    return {volume, isCache, pmcEditModel};
                } else {
                    return {volume, isCache};
                }
            })
        );
    }

    private getData(url: string) {
        return this.http.get(url).pipe(
            map(
                (data: any) => data.body
            )
        );
    }
}
