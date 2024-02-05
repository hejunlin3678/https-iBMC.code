import { IResourceInfo, IResource } from '../home.datatype';

export class ResourceInfo {
    private resources: IResource[];

    constructor(resourceInfo: IResourceInfo) {
        const resources: IResource[] = [
            {
                title: 'HOME_PROCESSOR',
                backgroundClassName: 'cpuImg',
                healthStatus: resourceInfo.processorStatus,
                id: 'Cpu',
                targetNav: ['/navigate/system/info/processor'],
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
                healthStatus: resourceInfo.memoryStatus,
                id: 'Memory',
                targetNav: ['/navigate/system/info/memory'],
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
                title: 'HOME_STORE',
                backgroundClassName: 'storeImg',
                healthStatus: resourceInfo.storageStatus,
                id: 'Store',
                targetNav: ['/navigate/system/storage'],
                hide: resourceInfo.raidControllerNum === undefined ||
                    (resourceInfo.raidControllerNum + resourceInfo.logicalDriveNum + resourceInfo.physicalDriveNum <= 0),
                details: [{
                    label: 'OTHER_RAID_CARD',
                    infor: resourceInfo.raidControllerNum
                }, {
                    label: 'HOME_LOGICAL_DISK',
                    infor: resourceInfo.logicalDriveNum
                }, {
                    label: 'HOME_PHYSICAL_DISK',
                    infor: resourceInfo.physicalDriveNum
                }]
            },
            {
                title: 'HOME_POWER',
                backgroundClassName: 'powerImg',
                healthStatus: resourceInfo.powerStatus,
                id: 'Power',
                targetNav: ['/navigate/system/power'],
                hide: !resourceInfo.powMaxNum,
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
                healthStatus: resourceInfo.fanStatus,
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
                healthStatus: resourceInfo.netadapterStatus,
                id: 'Internet',
                targetNav: ['/navigate/system/info/net'],
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
