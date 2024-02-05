import { IResourceInfo, IResource } from '../home.datatype';
import { Pangen } from '../../../../models/pangen.model';

export class ResourceInfo {
    private resources: IResource[];

    constructor(resourceInfo: IResourceInfo) {
        const resources: IResource[] = [
            {
                title: 'HOME_PROCESSOR',
                backgroundClassName: 'cpuImg',
                id: 'Cpu',
                targetNav: ['/navigate/system/info/processor'],
                hide: Pangen.ocean === 'PangeaV6_Atlantic_Smm',
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: resourceInfo.proMaxNum
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: resourceInfo.proCurrentNum
                }]
            },
            {
                title: 'HOME_MEMORY',
                backgroundClassName: 'memoryImg',
                id: 'Memory',
                targetNav: ['/navigate/system/info/memory'],
                hide: Pangen.ocean === 'PangeaV6_Atlantic_Smm',
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: resourceInfo.memMaxNum
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: resourceInfo.memCurrentNum
                }, {
                    label: 'COMMON_CAPACITY',
                    infor: `${resourceInfo.capacityGiB}GB`
                }]
            },
            {
                title: 'HOME_POWER',
                backgroundClassName: 'powerImg',
                id: 'Power',
                targetNav: ['/navigate/system/power'],
                hide: !resourceInfo.powMaxNum || Pangen.ocean === 'PangeaV6_Atlantic',
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: resourceInfo.powMaxNum
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: resourceInfo.powCurrentNum
                }]
            },
            {
                title: 'HOME_FAN',
                backgroundClassName: 'fanImg',
                id: 'Fan',
                targetNav: ['/navigate/system/fans'],
                hide: !resourceInfo.fanMaxNum,
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: resourceInfo.fanMaxNum
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: resourceInfo.fanCurrentNum
                }]
            },
            {
                title: 'COMMON_NETWORK',
                backgroundClassName: 'internetImg',
                id: 'Internet',
                targetNav: ['/navigate/system/info/net'],
                hide: Pangen.ocean === 'PangeaV6_Atlantic_Smm',
                details: [{
                    label: 'HOME_HEALTH_TOTALNUM',
                    infor: resourceInfo.netMaxNum || 0
                }, {
                    label: 'HOME_HEALTH_CURRENTNUM',
                    infor: resourceInfo.netCurrentNum || 0
                }]
            }
        ];

        this.resources = resources.filter((resource: IResource) => !resource.hide);
    }

    get getResources(): IResource[] {
        return this.resources;
    }
}
