import { Injectable } from '@angular/core';
import { Subject } from 'rxjs/internal/Subject';
import { HttpService, UserInfoService, GlobalDataService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';
import { IMonitorInfo, IResourceInfo, IAccessInfo, ICollectProgress } from '../home.datatype';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { ServerDetail, ResourceInfo, EchartsOption, Accesses } from '../models';
import { mergeMap } from 'rxjs/internal/operators/mergeMap';
import { from } from 'rxjs/internal/observable/from';
import { catchError } from 'rxjs/internal/operators/catchError';
import { of } from 'rxjs/internal/observable/of';
import { IOverview } from '../models/overview.datatype';

@Injectable({
    providedIn: 'root'
})
export class HomeService {

    private addressIPv4: string;

    private msgSubject = new Subject();
    public downloadKVMLink: string;
    public monitorInfo: any;
    private hardShow: boolean;

    constructor(
        private http: HttpService,
        private userService: UserInfoService,
        private translate: TranslateService,
        private globalData: GlobalDataService,
    ) { }

    // 发送消息
    public sendMessage(type: any) {
        this.msgSubject.next(type);
    }

    // 清理信息:
    public clearMessage() {
        this.msgSubject.next(true);
    }

    // 获取信息，返回Observable
    public getMessage(): Observable<any> {
        return this.msgSubject.asObservable();
    }

    public getData() {
        return this.getOverView();
    }

    private getOverView(): Observable<IOverview> {
        return this.http.get('/UI/Rest/Overview').pipe(
            pluck('body'),
            mergeMap(
                (data) => {
                    return this.getHardSupport().pipe(
                        map(() => {
                            return data;
                        })
                    );
                }
            ),
            map((data) => this.factoryOverviewInfo(data))
        );
    }

    private getHardSupport(): Observable<boolean> {
        // 是否显示硬分区，接口字段来决定
        return from(this.globalData.partitySupport).pipe(
            map((res: boolean) => {
                if (this.hardShow !== res) {
                    this.hardShow = res;
                }
                return this.hardShow;
            },
                catchError(() => {
                    return of(this.hardShow);
                }))
        );
    }

    private factoryOverviewInfo(overviewData): IOverview {
        const {
            DeviceInfo: {
                ProductName: productName,
                ProductAlias: productAlias,
                ProductSN: productSN,
                HostName: serverName,
                BMCVersion: bMCVersion,
                BIOSVersion: biosVersion,
                GUID,
                MAC,
                DefaultIPAddress: {
                    Mode: mode,
                    Address: address
                }
            },
            KVMClientUrl,
            RemoteBMCIPv4Address: nodeIp,
            ProductImage: productImage,
            FusionPartition: fusionPartition,
            VGAUSBDVDEnabled: vgausbdvdEnabled,
        } = overviewData;

        this.downloadKVMLink = KVMClientUrl;

        const resultData = {
            monitorInfo: this.factoryMonitorInfo(overviewData),
            resourceInfo: this.factoryResourceInfo(overviewData),
            deviceInfo: {
                productName,
                aliasName: productAlias,
                productPicture: productImage,
                productSN,
                biosVersion,
                systemSerialNumber: productSN,
                serverName,
                firmwareVersion: bMCVersion,
                permanentMACAddress: MAC,
                GUID,
            },
            accessInfo: {
                nodeIp,
                FusionPartition: fusionPartition,
                VGAUSBDVDEnabled: vgausbdvdEnabled,
                mode,
                address,
            }
        };
        // 补充UserService的部分信息
        sessionStorage.setItem('productName', resultData.deviceInfo.productName);
        localStorage.setItem('productName', resultData.deviceInfo.productName);
        localStorage.setItem('productSN', resultData.deviceInfo.productSN);

        this.monitorInfo = resultData.monitorInfo;
        const deviceInfo = new ServerDetail(resultData.deviceInfo);
        const resourceInfo = new ResourceInfo(resultData.resourceInfo);
        const accessInfo = this.factoryAccess(resultData.accessInfo);
        const monitorInfo = new EchartsOption(resultData.monitorInfo, this.userService.ibmaSupport, this.translate);

        return { deviceInfo, monitorInfo, resourceInfo, accessInfo };
    }

    private factoryMonitorInfo(overviewData): IMonitorInfo {
        let monitorInfo: IMonitorInfo = {};
        const {
            Summary: {
                Processor: processor,
                Memory: memory,
                Storage: storage,
                Power: power,
                Thermal: thermal,
            },
        } = overviewData;

        const defaultValue = 0;
        if (processor) {
            const {
                UseagePercent: proUseagePercent,
                ThresholdPercent: proThresholdPercent,
            } = processor;
            monitorInfo = {
                ...monitorInfo,
                proUseagePercent: proUseagePercent || defaultValue,
                proThresholdPercent
            };
        }
        if (memory) {
            const {
                UseagePercent: memUseagePercent,
                ThresholdPercent: memThresholdPercent,
            } = memory;
            monitorInfo = {
                ...monitorInfo,
                memUseagePercent: memUseagePercent || defaultValue,
                memThresholdPercent
            };
        }
        if (storage) {
            const {
                DiskUsagePercent: diskUsagePercent,
                DiskThresholdPercent: diskThresholdPercent,
            } = storage;
            monitorInfo = {
                ...monitorInfo,
                diskUsagePercent: diskUsagePercent || defaultValue,
                diskThresholdPercent
            };
        }
        if (power) {
            const {
                PowerConsumedWatts: powerConsumedWatts,
                PowerCapacityWatts: powerCapacityWatts,
                UpperPowerThresholdWatts: upperPowerThresholdWatts,
            } = power;
            monitorInfo = {
                ...monitorInfo,
                powerConsumedWatts: powerConsumedWatts || defaultValue,
                powerCapacityWatts,
                upperPowerThresholdWatts,
            };
        }
        if (thermal) {
            const {
                InletTemperature: inletTemperature,
                InletTempMinorAlarmThreshold: inletTempMinorThreshold,
                InletTempMajorAlarmThreshold: inletTempMajorThreshold
            } = thermal;
            monitorInfo = {
                ...monitorInfo,
                inletTemperature: inletTemperature || defaultValue,
                inletTempMinorThreshold,
                inletTempMajorThreshold,
            };
        }
        return monitorInfo;
    }

    private factoryResourceInfo(overviewData): IResourceInfo {
        let resourceInfo: IResourceInfo = {};
        const {
            Summary: {
                Processor: processor,
                Memory: memory,
                Storage: storage,
                Power: power,
                Fan: fan,
                Netadapter: netadapter,
            },
        } = overviewData;

        if (processor) {
            const {
                MaxNum: proMaxNum,
                CurrentNum: proCurrentNum,
            } = processor;
            resourceInfo = {
                ...resourceInfo,
                proMaxNum,
                proCurrentNum,
            };
        }
        if (memory) {
            const {
                MaxNum: memMaxNum,
                CurrentNum: memCurrentNum,
                CapacityGiB: capacityGiB
            } = memory;
            resourceInfo = {
                ...resourceInfo,
                memMaxNum,
                memCurrentNum,
                capacityGiB,
            };
        }
        if (storage) {
            const {
                RaidControllerNum: raidControllerNum,
                LogicalDriveNum: logicalDriveNum,
                PhysicalDriveNum: physicalDriveNum,
            } = storage;
            resourceInfo = {
                ...resourceInfo,
                raidControllerNum,
                logicalDriveNum,
                physicalDriveNum,
            };
        }
        if (power) {
            const {
                MaxNum: powMaxNum,
                CurrentNum: powCurrentNum,
                PresentPSUNum: presentPSUNum,
            } = power;
            resourceInfo = {
                ...resourceInfo,
                powMaxNum,
                powCurrentNum,
                presentPSUNum,
            };
        }
        if (fan) {
            const {
                MaxNum: fanMaxNum,
                CurrentNum: fanCurrentNum,
            } = fan;
            resourceInfo = {
                ...resourceInfo,
                fanMaxNum,
                fanCurrentNum,
            };
        }
        if (netadapter) {
            const {
                MaxNum: netMaxNum,
                CurrentNum: netCurrentNum,
            } = netadapter;
            resourceInfo = {
                ...resourceInfo,
                netMaxNum,
                netCurrentNum,
            };
        }

        return resourceInfo;
    }
    // 生成快捷操作目录
    private factoryAccess(accessInfo: IAccessInfo): Accesses {
        const hasConfigureUserRight = this.userService.hasPrivileges(['ConfigureUsers']);
        const hasConfigureComponentsRight = this.userService.hasPrivileges(['ConfigureComponents']);
        const hasOemSecurityMgmtRight = this.userService.hasPrivileges(['OemSecurityMgmt']);
        const hasLoginRight = this.userService.hasPrivileges(['Login']);

        const accessesObj = new Accesses(
            accessInfo,
            hasLoginRight,
            hasConfigureComponentsRight,
            hasOemSecurityMgmtRight,
            hasConfigureUserRight,
            this.hardShow
        );
        this.setAddressIPv4(accessInfo);
        return accessesObj;
    }

    private setAddressIPv4(accessInfo: IAccessInfo) {
        // 恢复默认地址
        let replaceStr = '--';
        try {
            if (accessInfo.mode.indexOf('DHCP') > -1) {
                this.addressIPv4 = this.translate.instant('HOME_IPDEFAULDHCP');
            } else {
                replaceStr = accessInfo.address || '--';
                this.addressIPv4 = this.translate.instant('HOME_IPDEFAULVALUE').replace('N/A', replaceStr);
            }
        } catch (e) {
            this.addressIPv4 = this.translate.instant('HOME_IPDEFAULVALUE').replace('N/A', replaceStr);
        }
    }

    public openKVM(resourceId: string) {
        const url = `/UI/Rest/KeepAlive`;
        const postData = {
            Mode: 'Activate'
        };
        const headers = {
            'Content-Type': 'application/json;charset=utf-8',
            'From': 'WebUI'
        };
        return this.http.post(url, postData, headers);
    }

    public exportKvmStartupFile(postData: object) {
        const url = '/UI/Rest/Services/KVM/GenerateStartupFile';
        const headers = {
            'Content-Type': 'application/json;charset=utf-8',
            'Accept': 'application/json'
        };
        return this.http.post(url, postData, {
            headers,
            responseType: 'blob'
        });
    }

    public searchKvmActiveStatus(timingRequest: boolean) {
        const url = '/UI/Rest/Services/KVM';
        const header = {
            timingRequest,
            ignoreKeepAlive: true
        };
        return this.http.get(url, header);
    }

    public getFun() {
        return this.http.get(`/UI/Rest/GenericInfo`).pipe(
            pluck('body')
        );
    }

    // 一键收集
    public oneKeyCollect(fileName: string): Observable<string> {
        const url = '/UI/Rest/Dump';
        return this.http.post(url, {}).pipe(
            pluck('body'),
            map(res => this.factoryCollect(res))
        );
    }

    private factoryCollect(res): string {
        return res.url;
    }

    public getAddressIPv4() {
        return this.addressIPv4;
    }

    public collectProgress(url: string): Observable<ICollectProgress> {
        return this.http.get(url).pipe(
            pluck('body'),
            map(data => this.factoryCollectProgress(data))
        );
    }

    private factoryCollectProgress({ state, prepare_progress: taskPercentage, downloadurl: downloadUrl }): ICollectProgress {
        return {
            state,
            taskPercentage: taskPercentage + '%',
            downloadUrl,
        };
    }

    // 恢复配置
    public restoreSet(params) {
        const url = '/UI/Rest/RestoreFactory';
        return this.http.post(url, params).pipe(
            pluck('body')
        );
    }

}
