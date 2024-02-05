import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { interNameArr, IOptions } from 'src/app/common-app/models';
import { HttpService } from 'src/app/common-app/service';
import { RaidNode, VolumePolicyModel, VolumeMembersModel, StorageTree, AvailableCapacity, RaidType } from '../../models';
import { deleteEmptyProperty } from '../../utils';

@Injectable({
    providedIn: 'root'
})
export class VolumeCreateService {

    public raidNode$: Observable<RaidNode>;
    constructor(
        private http: HttpService
    ) { }

    private save(data: any) {
        const obj = {};
        for (const attr in data) {
            if (Object.prototype.hasOwnProperty.call(data, attr)) {
                const key: string = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                let value: any = null;
                if (data[attr] && data[attr].name && interNameArr.indexOf(data[attr].name) > -1) {
                    value = data[attr].id;
                } else {
                    value = data[attr];
                }
                obj[key] = value;
            }
        }
        const param = deleteEmptyProperty(obj);
        return (param && JSON.stringify(param) !== '{}') ? param : null;
    }
    // 获取可以关联的逻辑盘列表
    public getLogicList() {
        const logicList: IOptions[] = [];
        StorageTree.getInstance().getCheckedNode?.children?.forEach(item => {
            const logicOptions: IOptions = {
                id: '',
                label: '',
                maxCapacity: 0,
                name: 'IOptions',
            };
            // 仅逻辑盘的SSDCachecadeVolume属性为false,getSSDCachingEnabled属性为false才能关联
            if (item.componentName === 'VolumeNode' && item?.getSSDCachingEnabled === false && item?.getSSDCachecadeVolume === false) {
                logicOptions.id = item.id;
                logicOptions.label = item.label;
                logicOptions.maxCapacity = item.capacityBytes;
                logicList.push(logicOptions);
            }
        });
        return logicList;
    }
    public createVolumn() {
        const raidNode = StorageTree.getInstance().getCheckedNode as RaidNode;
        const url = `/UI/Rest/System/Storage/${raidNode.id}/Volumes`;
        const param1 = this.save(VolumePolicyModel.getInstance().selectData);
        const param2 = this.save(VolumeMembersModel.getInstance().getSelectData);
        let param = Object.assign(param1, param2);
        // Name必须下发，没有设置下发为''
        let selectDataName = VolumePolicyModel.getInstance().selectData.name;
        if (selectDataName) {
            selectDataName = selectDataName.trim();
        }
        param['Name'] = selectDataName || '';
        // 计算SpanNumber
        let spanNum = 0;
        const selectData = VolumeMembersModel.getInstance().getSelectData;
        spanNum = getSpanNumer(selectData.volumeRaidLevel, selectData.drives, selectData.spanNumber);
        if (VolumeMembersModel.getInstance().createRelated) {
            // 换算传递到后台的SpanNumber的值：选中的总盘数除以子组成员盘数
            spanNum = selectData.drives.length / selectData.spanNumber;
        }
        // 子组盘数不存在为0的情况
        param['SpanNumber'] = spanNum === 0 ? null : spanNum;
        if (VolumePolicyModel.getInstance().selectData.createCacheCadeFlag) {
            if  (raidNode.getRaidType === RaidType.PMC) {
                // pmc卡的maxCache逻辑盘参数下发
                param = capacityValue(pmcTwoCacheTrue(param),
                VolumeMembersModel.getInstance().getSelectData.capacityMBytes,
                VolumeMembersModel.getInstance().getMinSize);
            } else {
                param = twoCacheTrue(param);
            }
        } else {
            param = othersRule(
                param,
                VolumeMembersModel.getInstance().isHaveRelated,
                VolumeMembersModel.getInstance().createRelated,
                VolumeMembersModel.getInstance().availableSelect
            );
            param = capacityValue(param,
                VolumeMembersModel.getInstance().getSelectData.capacityMBytes,
                VolumeMembersModel.getInstance().getMinSize
            );
        }
        // 在创建逻辑盘且未改动容量时，下发容量数据值为"null"方便后端处理
        if (VolumeMembersModel.getInstance().getCapacity?.oldValue) {
            param['CapacityMBytes'] = null;
        }
        return this.http.post(url, param);
    }
}

/**
 * 创建逻辑盘--spanNumber下发逻辑
 * @param raidLevel raid级别
 * @param drives 物理盘数组
 * @param spanNum 每个Span的成员盘数
 */
function getSpanNumer(raidLevel: IOptions, drives: number[], spanNum: number): number {
    let spanNumer: number = 0;
    const len = drives.length;
    switch (Number(raidLevel.label)) {
        case 0: case 1: case 5: case 6:
            spanNumer = 1;
            break;
        case 10: case 50: case 60:
            spanNumer = len / spanNum;
            break;
        default:
            break;
    }
    return spanNumer;
}

/**
 * 创建逻辑盘--二级缓存开启，需下发的的参数
 * @param param 所有下发参数
 */
function twoCacheTrue(param: any): any {
    const obj: any = {
        CreateCacheCadeFlag: true,
        Drives: param.Drives,
        VolumeRaidLevel: param.VolumeRaidLevel,
        Name: param.Name,
        DefaultWritePolicy: param.DefaultWritePolicy,
    };
    return obj;
}

/**
 * PMC卡创建逻辑盘--二级缓存开启，需下发的的参数
 * @param param 所有下发参数
 */
 function pmcTwoCacheTrue(param: any): any {
    const obj: any = {
        CreateCacheCadeFlag: true,
        Drives: param.Drives,
        VolumeRaidLevel: param.VolumeRaidLevel,
        Name: param.Name,
        DefaultWritePolicy: param.DefaultWritePolicy,
        CacheLineSize: param.CacheLineSize,
        AssociatedVolumes: [param.AssociationLogical]
    };
    return obj;
}

/**
 * 创建逻辑盘--物理盘组合场景、可用容量下发逻辑
 * @param param 所有下发参数
 * @param isHaveRelated 是否有物理盘组合场景
 * @param createRelated 是否为剩余容量创建场景
 * @param availableCapacity 可用容量
 */
function othersRule(param: any, isHaveRelated: boolean, createRelated: boolean, availableCapacity: AvailableCapacity): any {
    const obj = param;
    // 存在组合场景，且不使用剩余容量时不需要下发spanNum和raid级别
    if (isHaveRelated && !createRelated) {
        delete obj['SpanNumber'];
        delete obj['VolumeRaidLevel'];
    }
    // 如果存在可用容量，需要将可用容量下发BolckIndex
    if (availableCapacity) {
        // BlockIndex: 指定存储资源信息中FreeBlocksSpaceMib的数组下标
        obj['BlockIndex'] = 0;
    }
    // 二级缓存为false
    param['CreateCacheCadeFlag'] = false;
    return obj;
}

/**
 * 创建逻辑盘--下发容量必须是MB的倍数
 * @param param 所有下发参数
 * @param value 容量
 * @param unit 容量单位
 * @param minSize 较小的容量
 */
function capacityValue(param: any, value: number, minSize: number) {
    const obj = param;
    let capacity: number = value;
    // 和可用容量做比较
    const volumeMember = VolumeMembersModel.getInstance();
    if (volumeMember.availableSelect) {
        const available = volumeMember.availableSelect.getBatyValue;
        capacity = (capacity && capacity <= available) ? capacity : available;
    } else {
        // 不存在可用容量时，和最小容量作比较
        capacity = (capacity && capacity <= minSize) ? capacity : minSize;
    }
    obj['CapacityMBytes'] = Number(Number(capacity / 1024 / 1024).toFixed(0));
    return obj;
}
