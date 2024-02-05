import { Injectable } from '@angular/core';
import { map } from 'rxjs/internal/operators/map';
import { isEmptyProperty } from 'src/app/common-app/utils';
import { isEmptyBoolean, isEmptySwitch, getUnitConversionInt } from '../../utils/storage.utils';
import { Observable } from 'rxjs/internal/Observable';
import { IValue, IOptions } from 'src/app/common-app/models';
import { IChildData, Raid, DriveCheckBox, BBUModel, RaidNode, StorageTree } from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { HttpService, GlobalDataService } from 'src/app/common-app/service';
import { from } from 'rxjs/internal/observable/from';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { pluck } from 'rxjs/operators';

@Injectable({
    providedIn: 'root'
})
export class RaidService {

    private empty = '--';
    constructor(
        private http: HttpService,
        private i18n: TranslateService,
        private global: GlobalDataService
    ) { }


    factory(raidNode: RaidNode, flag?: boolean): Observable<Raid> {
        const post$ = [from(this.global.ibmaRunning), this.getData(raidNode.getUrl)];
        return forkJoin(post$).pipe(
            map(([ibmaRunning, bodyData]) => {
                if (!bodyData) {
                    return null;
                }
                const name = isEmptyProperty(bodyData.Name) || this.empty;
                const type = isEmptyProperty(bodyData.Type);
                const firmwareVersion = isEmptyProperty(bodyData.FirmwareVersion) || this.empty;
                const bandManag = isEmptyBoolean(bodyData.OOBSupport, 'COMMON_YES', 'COMMON_NO') || this.empty;
                let health = isEmptyProperty(bodyData.Health) || this.empty;
                const severity = bodyData.Health;
                if (!severity) {
                    health = this.empty;
                } else if (severity === 'OK') {
                    health = 'STORE_NORMAL';
                } else {
                    health = `${this.i18n.instant('STORE_ABNORMAL')}(${bodyData.FaultDetails})`;
                }
                let levels = null;
                if (isEmptyProperty(bodyData.SupportedRAIDTypes) && bodyData.SupportedRAIDTypes.length > 0) {
                    levels = bodyData.SupportedRAIDTypes.filter(
                        (level: any) => {
                            if (level.indexOf('RAID') > -1) {
                                return level.substring(4, level.length);
                            }
                        }
                    );
                }
                const raidLevels: IValue = {
                    name: 'IValue',
                    label: levels ? 'RAID(' + levels.join('/').replace(/RAID/g, '') + ')' : this.empty,
                    value: levels ? levels.join('/').replace(/RAID/g, '').split('/') : null
                };
                const mode = isEmptyProperty(bodyData.Mode) || this.empty;
                const configVersion = isEmptyProperty(bodyData.ConfigurationVersion) || this.empty;
                const memorySize = isEmptyProperty(bodyData.MemorySizeMiB, ' MB') || this.empty;
                const deviceProtocols = isEmptyProperty(bodyData.SupportedDeviceProtocols) || 0;
                const speedGbps = isEmptyProperty(bodyData.SpeedGbps) || 0;
                const deviceInter = (deviceProtocols && speedGbps) ? deviceProtocols + ' ' + speedGbps + ' GB' : this.empty;
                const sasAddress = isEmptyProperty(bodyData.SASAddress) || this.empty;
                const stripSizeRange = (this.isEmptyNumber(bodyData.MinStripeSizeBytes) &&
                    this.isEmptyNumber(bodyData.MaxStripeSizeBytes)) ?
                    getUnitConversionInt(bodyData.MinStripeSizeBytes || 0) +
                    ' - ' +
                    getUnitConversionInt(bodyData.MaxStripeSizeBytes || 0) :
                    this.empty;
                const cachePinned = isEmptyBoolean(bodyData.CachePinnedState, 'COMMON_YES', 'COMMON_NO') || this.empty;
                const faultMemory = isEmptyBoolean(bodyData.MaintainPDFailHistory, 'STORE_ENABLED', 'STORE_DISABLED') || this.empty;
                const driverName = bodyData.DriverInfo ? isEmptyProperty(bodyData.DriverInfo.DriverName) : null;
                const driverVersion = bodyData.DriverInfo ? isEmptyProperty(bodyData.DriverInfo.DriverVersion) : null;
                const copyback = isEmptySwitch(bodyData.CopyBackState,
                    { name: 'ISwitch', id: true, label: 'STORE_ENABLED', state: false },
                    { name: 'ISwitch', id: false, label: 'STORE_DISABLED', state: false }
                ) || { name: 'ISwitch', id: false, label: '--', state: false };
                const smartErrors = isEmptySwitch(bodyData.SmarterCopyBackState,
                    { name: 'ISwitch', id: true, label: 'STORE_ENABLED', state: false },
                    { name: 'ISwitch', id: false, label: 'STORE_DISABLED', state: false }
                ) || { name: 'ISwitch', id: false, label: '--', state: false };
                const jbodModel = isEmptySwitch(
                    bodyData.JBODState,
                    { name: 'ISwitch', id: true, label: 'STORE_ENABLED', state: false },
                    { name: 'ISwitch', id: false, label: 'STORE_DISABLED', state: false }
                ) || { name: 'ISwitch', id: false, label: '--', state: false };

                const childData: IChildData = {
                    volumeSupport: bodyData.VolumeSupported,
                    oobSupport: bodyData.OOBSupport,
                    maxSize: bodyData.MaxStripeSizeBytes,
                    minSize: bodyData.MinStripeSizeBytes,
                    readPolicyConfig: bodyData.ReadPolicy.Configurable,
                    writePolicyConfig: bodyData.WritePolicy.Configurable,
                    IOPolicyConfig: bodyData.CachePolicy.Configurable,
                    drivePolicyConfig: bodyData.DriveCachePolicy.Configurable,
                    accessPolicyConfig: bodyData.AccessPolicy.Configurable,
                };

                const driveArr: DriveCheckBox[] = this.getCheckBoxArr(bodyData.Drives);
                const foreignConfigStatus = !this.getForeignConfigStatus(bodyData.Drives);

                const noBBUWrite = isEmptyBoolean(bodyData.NoBatteryWriteCacheEnabled, 'true', 'false');
                const pcieLinkWidth = this.isEmptyNumber(bodyData.PCIeLinkWidth) ? bodyData.PCIeLinkWidth : this.empty;
                const readCache = isEmptyProperty(bodyData.ReadCachePercent, '%');
                const spareActivationMode = isEmptyProperty(bodyData.SpareActivationMode);
                const writeCachePolivy = `${isEmptyProperty(bodyData.WriteCachePolicy?.ConfiguredDrive) || this.empty}/
                    ${isEmptyProperty(bodyData.WriteCachePolicy?.UnconfiguredDrive) || this.empty}/
                    ${isEmptyProperty(bodyData.WriteCachePolicy?.HBADrive) || this.empty}`;
                const bootDevices = bodyData.BootDevices?.length > 0 ? bodyData.BootDevices?.join('/') : null;
                const writeCacheData = bodyData.WriteCachePolicy || null;

                let autoRepairEnabled = null;
                let completedVolumeCount = null;
                let delayForStart = null;
                let enabled = null;
                let period = null;
                let rate = null;
                let runningStatus = null;
                let totalVolumeCount = null;

                if (flag && bodyData.ConsisCheckInfo) {
                    autoRepairEnabled = bodyData.ConsisCheckInfo.AutoRepairEnabled;
                    completedVolumeCount = bodyData.ConsisCheckInfo.CompletedVolumeCount;
                    delayForStart = bodyData.ConsisCheckInfo.DelayForStart.toString();
                    enabled = bodyData.ConsisCheckInfo.Enabled;
                    period = bodyData.ConsisCheckInfo.Period;
                    rate = bodyData.ConsisCheckInfo.Rate;
                    runningStatus = bodyData.ConsisCheckInfo.RunningStatus;
                    totalVolumeCount = bodyData.ConsisCheckInfo.TotalVolumeCount;
                }

                let bbuModel: BBUModel = null;
                if (bodyData.BBU) {
                    const position = bodyData.BBU.State === 'Absent' ? 'STORE_NO_IN_POSITION' : 'STORE_IN_POSITION';
                    let bbuHealth = bodyData.BBU.Health;
                    if (bodyData.BBU.State === 'Absent') {
                        bbuHealth = null;
                    } else if (!bbuHealth) {
                        bbuHealth = this.empty;
                    } else if (bbuHealth === 'OK' || bbuHealth === 'Informational') {
                        bbuHealth = 'STORE_NORMAL';
                    } else {
                        const detail = bodyData.BBU.FaultDetails || this.empty;
                        bbuHealth = `${this.i18n.instant('STORE_ABNORMAL')}(${detail})`;
                    }
                    bbuModel = new BBUModel(
                        bodyData.BBU.Name || this.empty,
                        position,
                        bbuHealth
                    );
                }

                const _raidNode = StorageTree.getInstance().getParentRaidById(bodyData.ID);

                return new Raid(
                    name,
                    type,
                    firmwareVersion,
                    bandManag,
                    health,
                    raidLevels,
                    mode,
                    configVersion,
                    memorySize,
                    deviceInter,
                    sasAddress,
                    stripSizeRange,
                    cachePinned,
                    faultMemory,
                    driverName,
                    driverVersion,
                    copyback,
                    smartErrors,
                    jbodModel,
                    childData,
                    driveArr,
                    bbuModel,
                    ibmaRunning,
                    foreignConfigStatus,
                    this.getModes(bodyData.SupportedModes),
                    _raidNode.epdSupported,
                    _raidNode.jbodStateSupported,
                    noBBUWrite,
                    pcieLinkWidth,
                    readCache,
                    spareActivationMode,
                    raidNode.getRaidType,
                    writeCachePolivy,
                    bootDevices,
                    writeCacheData,
                    autoRepairEnabled,
                    completedVolumeCount,
                    delayForStart,
                    enabled,
                    period,
                    rate,
                    runningStatus,
                    totalVolumeCount
                );
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

    private isEmptyNumber(val: number): boolean {
        if (val === 0 || isEmptyProperty(val)) {
            return true;
        }
        return false;
    }

    private getCheckBoxArr(drives: any[]): DriveCheckBox[] {
        const driveArr: DriveCheckBox[] = [];
        if (drives && drives.length > 0) {
            drives.forEach((drive: any) => {
                const members = drive.Members ? drive.Members : null;
                const level = drive.VolumeRaidLevel ? {
                    name: 'IOptions',
                    id: drive.VolumeRaidLevel,
                    label: drive.VolumeRaidLevel.replace(/RAID/g, '')
                } : null;
                driveArr.push(
                    new DriveCheckBox(
                        drive.ID,
                        drive.DriveID,
                        drive.Name,
                        drive.FreeSpaceMiBPerDrive === 0 || drive.FreeSpaceMiBPerDrive ?
                            drive.FreeSpaceMiBPerDrive * 1024 * 1024 : drive.CapacityBytes,
                        members,
                        drive.MediaType,
                        drive.Protocol,
                        drive.SSDCachecadeVolume,
                        drive.FirmwareStatus,
                        level,
                        drive.NumDrivePerSpan,
                        drive.FreeBlocksSpaceMiB?.length === 0 ? [drive.TotalFreeSpaceMiB] : drive.FreeBlocksSpaceMiB,
                        drive.IsEPD,
                        drive.TotalFreeSpaceMiB || 0,
                        drive.DiskCachePolicy
                    )
                );
            });
        }
        driveArr.forEach(
            (driveCheck: DriveCheckBox) => {
                let related: DriveCheckBox[] = [];
                if (driveCheck.getRelatedName && driveCheck.getRelatedName.length > 0) {
                    driveCheck.getRelatedName.map(
                        item => {
                            related.push(
                                driveArr.filter(
                                    (obj) => obj.getDriveID === item
                                )[0]
                            );
                        }
                    );
                } else {
                    related = null;
                }
                driveCheck.setRelated = related;
            }
        );

        return driveArr;
    }

    private getModes(supportedModes: any[]): IOptions[] {
        const options: IOptions[] = supportedModes && supportedModes.length > 0 ? supportedModes.map(
            (_mode: any) => {
                return {
                    name: 'IOptions',
                    id: String(_mode),
                    label: String(_mode)
                };
            }
        ) : null;
        return options;
    }

    private getForeignConfigStatus(drives: any[]): boolean {
        if (drives && drives.length > 0) {
            const state = drives.filter(
                (drive) => drive.FirmwareStatus === 'Foreign'
            );
            return state[0] ? true : false;
        }
        return false;
    }
    // 一键收集
    public oneKeyCollect(fileName: string, urlString: string): Observable<string> {
        const url = urlString + '/DumpControllerLog';
        const data = {
            Type: 'URI',
            Content: `/tmp/web/${fileName}`
        };
        return this.http.post(url, data).pipe(
            pluck('body'),
            map(res => this.factoryCollect(res))
        );
    }
    private factoryCollect(res): string {
        return res.url;
    }
}
