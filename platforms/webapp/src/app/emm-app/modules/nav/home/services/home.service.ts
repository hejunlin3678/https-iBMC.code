import { Injectable } from '@angular/core';
import { Subject, Observable } from 'rxjs';
import { map } from 'rxjs/internal/operators/map';
import { pluck } from 'rxjs/internal/operators/pluck';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { ProductName } from 'src/app/emm-app/models/common.datatype';
import { ICollectProgress, ResourceHealth } from '../home.datatype';
import { CommonData } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class HomeService {
    public subjectObj = new Subject();

    private addressIPv4: string;

    private msgSubject = new Subject();

    public downloadKVMLink: string;

    constructor(
        private http: HttpService,
    ) {
    }

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

    private getOverView(): Observable<any> {
        return this.http.get('/UI/Rest/Enclosure/Overview').pipe(
            pluck('body'),
            map((data) => this.factoryOverview(data))
        );
    }

    // 处理Overview接口数据
    public factoryOverview(data) {
        const {
            Chasis: {
                BackPlanePcbVersion: backPlanePcbVersion,
                ChassisID: chassisID,
                ChassisLocation: chassisLocation,
                ChassisName: chassisName,
                Model: chassisModel,
                PartNumber: chassisPartNumber,
                ProductName: chassisProductName,
                SerialNumber: chassisSerialNumber,
                Type: chassisType,
            },
        } = data;
        const fanLists = data?.Fan;
        const bladeLists = [];
        const swiLists = [];
        const hmmLists = [];
        localStorage.setItem('productName', chassisProductName);
        localStorage.setItem('productSN', chassisSerialNumber);
        // 把产品类型存储起来('TCE8080','TCE8040','E9000')
        sessionStorage.setItem('productModel', chassisModel);
        data?.Component.forEach(item => {
            if (item.Id.startsWith('Blade')) {
                bladeLists.push(item);
            } else if (item.Id.startsWith('Swi')) {
                swiLists.push(item);
            } else if (item.Id.startsWith('HMM')) {
                hmmLists.push(item);
            }
        });

        const {
            Power: {
                PowerCapacityWatts: powerCapacityWatts,
                PowerConsumedWatts: powerConsumedWatts,
                SupplyList: powerLists,
            }
        } = data;

        // 健康状态
        const resourceData = {
            bladeLists,
            powerLists,
            fanLists,
            swiLists,
            hmmLists,
        };
        // 系统监控用到的值
        const monitorData = {
            powerCapacityWatts,
            powerConsumedWatts,
            fanLists
        };
        // 首页的数据
        const homeData = {
            chassisInfo: {
                backPlanePcbVersion: chassisProductName + backPlanePcbVersion,
                chassisID,
                chassisLocation,
                chassisName,
                chassisModel,
                chassisPartNumber,
                chassisHeight: this.calculatedHeight(chassisModel),
                chassisSerialNumber,
                chassisType,
            },
            resourceInfo: this.factoryResourceData(resourceData, chassisModel),
            monitorInfo: this.factoryMonitorData(monitorData),
            // 前视图和后视图数据
            frontViewAndRearViewInfo: {...data},
        };
        return homeData;
    }

    // 根据机箱类型计算机箱高度
    public calculatedHeight (chassisModel: string): string {
        let chassisHeight = '';
        switch (chassisModel) {
            case ProductName.Emm12U:
                chassisHeight = '12U';
                break;
            case ProductName.Tce8U:
                chassisHeight = '8U';
                break;
            case ProductName.Tce4U:
                chassisHeight = '4U';
                break;
            default:
                chassisHeight = CommonData.isEmpty;
        }
        return chassisHeight;
    }

    // 更新机箱信息
    public updateChassisInfo(param: any): Promise<any> {
        return new Promise((resolve, reject) => {
            const url = `/UI/Rest/Enclosure/Overview`;
            const params: { [key: string]: any } = {
                'ChassisID': param.chassisID,
                'ChassisLocation': param.chassisLocation,
                'ChassisName': param.chassisName
            };

            // 去除无效属性
            for (const key in params) {
                if (Object.prototype.hasOwnProperty.call(params, key)) {
                    const value = params[key];
                    if (value === undefined) {
                        delete params[key];
                    }
                }
            }

            this.http.patch(url, params).subscribe({
                next: (res) => {
                    resolve(res);
                },
                error: (error) => {
                    reject(error);
                }
            });
        });
    }

    // 处理健康状态的数据
    public factoryResourceData(resource, chassisModel) {
        const resourceInfo = {
            bladeHealth: '',
            bladeStatus: ResourceHealth.OK,
            installedBlade: 0,
            totalBlade: 0,
            showBladeHealth: false,
            powerHealth: '',
            powerStatus: ResourceHealth.OK,
            installedPower: 0,
            totalPower: 0,
            showPowerHealth: false,
            fanHealth: '',
            fanStatus: ResourceHealth.OK,
            installedFan: 0,
            totalFan: 0,
            showFanHealth: false,
            swiHealth: '',
            swiStatus: ResourceHealth.OK,
            installedSwi: 0,
            totalSwi: 0,
            showSwiHealth: false,
            hmmHealth: '',
            hmmStatus: ResourceHealth.OK,
            installedHmm: 0,
            totalHmm: 2,
            showHmmHealth: false,
        };
        if (resource.bladeLists?.length > 0) {
            const {health, status} = this.statusJudgment(resource.bladeLists);
            resourceInfo.bladeHealth = health;
            resourceInfo.bladeStatus = status;
            resourceInfo.showBladeHealth = true;
            resourceInfo.installedBlade = resource.bladeLists.length;
        }
        if (resource.powerLists?.length > 0) {
            const {health, status} = this.statusJudgment(resource.powerLists);
            resourceInfo.powerHealth = health;
            resourceInfo.powerStatus = status;
            resourceInfo.showPowerHealth = true;
            resourceInfo.installedPower = resource.powerLists.length;
        }
        if (resource.fanLists?.length > 0) {
            const {health, status} = this.statusJudgment(resource.fanLists);
            resourceInfo.fanHealth = health;
            resourceInfo.fanStatus = status;
            resourceInfo.showFanHealth = true;
            resourceInfo.installedFan = resource.fanLists.length;
        }
        if (resource.swiLists?.length > 0) {
            const {health, status} = this.statusJudgment(resource.swiLists);
            resourceInfo.swiHealth = health;
            resourceInfo.swiStatus = status;
            resourceInfo.showSwiHealth = true;
            resourceInfo.installedSwi = resource.swiLists.length;
        }
        if (resource.hmmLists?.length > 0) {
            const {health, status} = this.statusJudgment(resource.hmmLists);
            resourceInfo.hmmHealth = health;
            resourceInfo.hmmStatus = status;
            resourceInfo.showHmmHealth = true;
            resourceInfo.installedHmm = resource.hmmLists.length;
        }
        // 根据不同的产品显示不同的总数
        switch (chassisModel) {
            case ProductName.Emm12U:
                resourceInfo.totalBlade = 16;
                resourceInfo.totalPower = 6;
                resourceInfo.totalFan = 14;
                resourceInfo.totalSwi = 4;
                break;
            case ProductName.Tce8U:
                resourceInfo.totalBlade = 8;
                resourceInfo.totalPower = 8;
                resourceInfo.totalFan = 16;
                break;
            case ProductName.Tce4U:
                resourceInfo.totalBlade = 4;
                resourceInfo.totalPower = 4;
                resourceInfo.totalFan = 8;
                break;
            default:
                break;
        }
        return resourceInfo;
    }

    // 健康状态各个值的状态判断
    private statusJudgment(list) {
        const result = {
            health: '',
            status: ResourceHealth.OK
        };
        if (list.some((item) => item.Health === ResourceHealth.Critical)) {
            result.health = 'HOME_SERVERE_STATUS';
            result.status = ResourceHealth.Critical;
            return result;
        } else if (list.some((item) => item.Health === ResourceHealth.Warning)) {
            result.health = 'HOME_WARMINGS_STATUS';
            result.status = ResourceHealth.Warning;
            return result;
        } else if (list.every((item) => item.Health === ResourceHealth.OK)) {
            result.health = '';
            result.status = ResourceHealth.OK;
            return result;
        }
    }

    // 系统监控值处理
    private factoryMonitorData(monitor) {
        const monitorInfo = {
            powerUsage: 0,
            fanAverageSpeed: 0,
        };
        let speed = 0;
        if (monitor?.fanLists?.length > 0 ) {
            monitor?.fanLists.forEach(item => {
                speed += item.Speed;
            });
            monitorInfo.fanAverageSpeed = Math.floor(speed / (monitor.fanLists.length));
            monitorInfo.powerUsage = Math.floor((monitor.powerConsumedWatts / monitor.powerCapacityWatts) * 100);
        }
        return monitorInfo;
    }

    public getFun() {
        return this.http.get(`/UI/Rest/GenericInfo`).pipe(
            pluck('body')
        );
    }

    // 一键收集
    public oneKeyCollect(): Observable<string> {
        const url = '/UI/Rest/Dump';
        return this.http.post(url, null).pipe(
            pluck('body'),
            map(res => this.factoryCollect(res))
        );
    }

    private factoryCollect(res): string {
        return res.url;
    }

    public setAddressIPv4(value: string) {
        this.addressIPv4 = value;
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
    public restoreSet() {
        const url = '/UI/Rest/RestoreFactory';
        return this.http.post(url, null).pipe(
            pluck('body')
        );
    }

}
