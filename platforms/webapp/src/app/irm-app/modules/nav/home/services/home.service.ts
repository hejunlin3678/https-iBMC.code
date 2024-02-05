import { Injectable } from '@angular/core';
import { Subject, Observable } from 'rxjs';
import { map } from 'rxjs/internal/operators/map';
import { pluck } from 'rxjs/internal/operators/pluck';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { ICollectProgress, IMonitorInfo, IOverview, IResourceInfo } from '../model/home.datatype';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models';

@Injectable()
export class HomeService {
    private addressIPv4: string;

    private msgSubject = new Subject();

    constructor(
        private http: HttpService,
        private translate: TranslateService,
        private user: UserInfoService,
    ) {
    }

    public ethenetId = null;

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

    public getAddressIPv4() {
        return this.addressIPv4;
    }

    public getData() {
        return this.getOverView();
    }

    private getOverView(): Observable<IOverview> {
        return this.http.get('/UI/Rest/Overview', { ignoreKeepAlive: true }).pipe(
            pluck('body'),
            map((data) => this.factoryOverviewInfo(data))
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
            RackInfo: {
                Building: building,
                City: city,
                Country: country,
                Floor: floor,
                HouseNumber: houseNumber,
                LoadCapacityKg: loadCapacityKg,
                LocationInfo: locationInfo,
                Name: name,
                PostCode: postCode,
                Rack: rack,
                Room: room,
                Row: row,
                Street: street,
                Territory: territory,
                TotalUCount: tTotalUCount,
                UcountUsed: ucountUsed
            },
            Summary,
            KVMClientUrl,
            RemoteBMCIPv4Address: nodeIp,
            ProductImage: productImage,
            FusionPartition: fusionPartition,
            VGAUSBDVDEnabled: vgausbdvdEnabled,
        } = overviewData;

        const productAliasName = productAlias ? ' (' + productAlias + ')' : '';
        const productAllName = productName ? productName + productAlias : CommonData.isEmpty;
        const cabinetPosition = name !== null ? (name + '(' + (row || '') + ',' + (rack || '') + ')') : CommonData.isEmpty;
        const resultData = {
            monitorInfo: this.factoryMonitorInfo(overviewData),
            resourceInfo: this.factoryResourceInfo(overviewData),
            deviceInfo: {
                productName: productAllName,
                position: cabinetPosition,
                capacity: loadCapacityKg !== null ? (loadCapacityKg + '') : CommonData.isEmpty,
                serialNumber: productSN || CommonData.isEmpty,
                firmwareVersion: bMCVersion || CommonData.isEmpty
            },
            accessInfo: {
                nodeIp,
                FusionPartition: fusionPartition,
                VGAUSBDVDEnabled: vgausbdvdEnabled,
                mode,
                address,
            },
            Summary
        };
        // 补充UserService的部分信息
        sessionStorage.setItem('productName', resultData.deviceInfo.productName);
        localStorage.setItem('productName', resultData.deviceInfo.productName);
        localStorage.setItem('productSN', productSN);
        this.setAddressIPv4(resultData.accessInfo);

        return resultData;
    }
    private setAddressIPv4(accessInfo: any) {
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

    private factoryMonitorInfo(overviewData): IMonitorInfo {
        const {
            RackInfo: {
                TotalUCount: tTotalUCount,
                UcountUsed: ucountUsed
            },
            Summary: {
                DeviceStatistics: deviceStatistics,
                Power: power
            }
        } = overviewData;

        let monitorInfo: IMonitorInfo = {
            totalU: tTotalUCount,
            usedU: ucountUsed,
            deviceTotalNumber: 0,
            serverNumber: 0,
            networkNumber: 0,
            storeNumber: 0,
            otherNumber: 0,
            maxRatedPower: 0,
            currentPower: 0
        };

        if (deviceStatistics) {
            const {
                NetworkCount: networkNumber,
                OtherCount: otherNumber,
                ServerCount: serverNumber,
                StorageCount: storeNumber
            } = deviceStatistics;
            monitorInfo = {
                ...monitorInfo,
                deviceTotalNumber: networkNumber + otherNumber + serverNumber + storeNumber,
                networkNumber,
                serverNumber,
                storeNumber,
                otherNumber
            };
        }
        if (power) {
            const {
                PowerCapacityWatts: maxRatedPower,
                PowerConsumedWatts: currentPower
            } = power;
            monitorInfo = {
                ...monitorInfo,
                maxRatedPower,
                currentPower
            };
        }

        return monitorInfo;
    }

    private factoryResourceInfo(overviewData): IResourceInfo {
        const {
            Summary: {
                Power: power,
                Fan: fan,
                SysBattery: battary,
            },
        } = overviewData;

        let resourceInfo: IResourceInfo = {
            totalPower: 0,
            currentPower: 0,
            totalBattary: 0,
            currentBattary: 0,
            totalFun: 0,
            currentFun: 0
        };

        if (power) {
            const {
                MaxNum: totalPower,
                CurrentNum: currentPower
            } = power;
            resourceInfo = {
                ...resourceInfo,
                totalPower,
                currentPower
            };
        }
        if (battary) {
            const {
                TotalBatteryCount: totalBattary,
                PresentBatteryCount: currentBattary,
            } = battary;
            resourceInfo = {
                ...resourceInfo,
                totalBattary,
                currentBattary,
            };
        }
        if (fan) {
            const {
                MaxNum: totalFun,
                CurrentNum: currentFun,
            } = fan;
            resourceInfo = {
                ...resourceInfo,
                totalFun,
                currentFun,
            };
        }

        return resourceInfo;
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
