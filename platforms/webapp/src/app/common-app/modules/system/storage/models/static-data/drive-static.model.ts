import { IOptions, IRadio, ISwitch } from 'src/app/common-app/models';
import { Store } from '../storage-interface';
import { IStorageRadio } from '../storage-interface';

export class DriveStaticModel {

    private constructor() { }

    get getFirmwareOptions(): IOptions[] {
        return this.firmwareOptions;
    }

    private static instance: DriveStaticModel;

    public static readonly INDICATOR = {
        indicatorOn: {
            id: 'Blinking',
            label: 'STORE_ENABLED',
            state: true,
            name: 'ISwitch'
        },
        indicatorOff: {
            id: 'Off',
            label: 'STORE_DISABLED',
            state: false,
            name: 'ISwitch'
        }
    };

    public static readonly HOTSPARERADIOS = {
        none: {
            id: 'None',
            label: 'STORE_NONE',
            name: 'IRadio',
            disable: false
        },
        dedicated: {
            id: 'Dedicated',
            label: 'STORE_DEDICATED',
            name: 'IRadio',
            disable: false
        },
        global: {
            id: 'Global',
            label: 'STORE_GLOBAL',
            name: 'IRadio',
            disable: false
        },
        autoReplace: {
            id: 'AutoReplace',
            label: 'STORE_AUTOREPLACE',
            name: 'IRadio',
            disable: false
        }
    };

    // 固件状态
    public static readonly FIRMWARESTATUS = {
        online: {
            id: 'Online',
            label: 'ONLINE',
            name: 'IOptions'
        },
        offline: {
            id: 'Offline',
            label: 'OFFLINE',
            name: 'IOptions'
        },
        unconfiguredGood: {
            id: 'UnconfiguredGood',
            label: 'UNCONFIGURED GOOD',
            name: 'IOptions'
        },
        unconfiguredBad: {
            id: 'UnconfiguredBad',
            label: 'UNCONFIGURED BAD',
            name: 'IOptions'
        },
        jbod: {
            id: 'JBOD',
            label: 'JBOD',
            name: 'IOptions'
        },
        hotSpareDrive: {
            id: 'HotSpareDrive',
            label: 'HOT SPARE',
            name: 'IOptions'
        },
        failed: {
            id: 'Failed',
            label: 'FAILED',
            name: 'IOptions'
        },
        rebuilding: {
            id: 'Rebuilding',
            label: 'REBUILD',
            name: 'IOptions'
        },
        gettingCopied: {
            id: 'GettingCopied',
            label: 'COPYBACK',
            name: 'IOptions'
        },
        unconfiguredShielded: {
            id: 'UnconfiguredShielded',
            label: 'UNCONFIGURED (Shielded)',
            name: 'IOptions'
        },
        hotSpareShielded: {
            id: 'HotSpareShielded',
            label: 'HOT SPARE (Shielded)',
            name: 'IOptions'
        },
        configuredShielded: {
            id: 'ConfiguredShielded',
            label: 'CONFIGURED (Shielded)',
            name: 'IOptions'
        },
        foreign: {
            id: 'Foreign',
            label: 'FOREIGN',
            name: 'IOptions'
        },
        active: {
            id: 'Active',
            label: 'Active',
            name: 'IOptions'
        },
        standby: {
            id: 'Standby',
            label: 'Stand-by',
            name: 'IOptions'
        },
        sleep: {
            id: 'Sleep',
            label: 'Sleep',
            name: 'IOptions'
        },
        dstInProgress: {
            id: 'DSTInProgress',
            label: 'DST executing in background',
            name: 'IOptions'
        },
        smartOfflineDataCollection: {
            id: 'SMARTOfflineDataCollection',
            label: 'SMART Off-line Data Collection executing in background',
            name: 'IOptions'
        },
        sctCommand: {
            id: 'SCTCommand',
            label: 'SCT command executing in background',
            name: 'IOptions'
        },
        ready: {
            id: 'Ready',
            label: 'Ready',
            name: 'IOptions'
        },
        notSupported: {
            id: 'NotSupported',
            label: 'Not Supported',
            name: 'IOptions'
        },
        predictiveFailure: {
            id: 'PredictiveFailure',
            label: 'Predictive Failure',
            name: 'IOptions'
        },
        raw: {
            id: 'Raw',
            label: 'Raw',
            name: 'IOptions'
        },
        eraseInProgress: {
            id: 'EraseInProgress',
            label: 'Erase In Progress',
            name: 'IOptions'
        },
    };

    // 是否为启动盘
    public static readonly isBootDisk: IStorageRadio[] = [
        {
            label: 'COMMON_YES',
            id: true,
            disable: false,
            name: 'IRadio',
            attrName: 'BootDisk'
        }, {
            label: 'COMMON_NO',
            id: false,
            disable: false,
            name: 'IRadio',
            attrName: 'BootDisk'
        }
    ];

    // 巡检开关状态STORE_PATROL_SWITCH
    public static readonly patrolSwitchStatus: IStorageRadio[] = [
        {
            label: 'FDM_ENABLED',
            id: 'Patrolling',
            disable: false,
            name: 'IRadio',
            attrName: 'PatrolSwitch'
        }, {
            label: 'FDM_DISABLED',
            id: 'DoneOrNotPatrolled',
            disable: false,
            name: 'IRadio',
            attrName: 'PatrolSwitch'
        }
    ];

    // 启动盘设置
    public static readonly pmcBootPriority: IOptions[] = [
        {
            id: 'Primary',
            label: 'Primary',
            name: 'IOptions'
        }, {
            id: 'Secondary',
            label: 'Secondary',
            name: 'IOptions'
        }, {
            id: 'All',
            label: 'All',
            name: 'IOptions'
        }, {
            id: 'None',
            label: 'None',
            name: 'IOptions'
        }
    ];

    private firmwareOptions: IOptions[];

    public static getInstance(): DriveStaticModel {
        if (!DriveStaticModel.instance) {
            DriveStaticModel.instance = new DriveStaticModel();
        }
        return DriveStaticModel.instance;
    }

    public static getIsBootDisk(key: boolean): IStorageRadio {
        const result = DriveStaticModel.isBootDisk.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }
    public static getPatrol(key: string): IStorageRadio {
        const result = DriveStaticModel.patrolSwitchStatus.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }
    /**
     * 根据接口返回值生成固件状态下拉选项
     * @param key 接口返回值
     * @param jbodState JBOD是否开启
     */
    public optionsFactory(key: string, jbodState: boolean, workMode: IOptions, isEPD: boolean, epdSupported: boolean): IOptions[] {
        let firmwareOptions: IOptions[] = [];
        switch (key) {
            case Store.Online:
                // EDP盘，可以从online设置成UG，否则只能从onlie设置成offline
                if (isEPD && epdSupported) {
                    firmwareOptions = [DriveStaticModel.FIRMWARESTATUS.unconfiguredGood, DriveStaticModel.FIRMWARESTATUS.online];
                } else {
                    firmwareOptions = [DriveStaticModel.FIRMWARESTATUS.online, DriveStaticModel.FIRMWARESTATUS.offline];
                }
                break;
            case Store.Offline:
                firmwareOptions = [DriveStaticModel.FIRMWARESTATUS.online, DriveStaticModel.FIRMWARESTATUS.offline];
                break;
            case Store.UnconfiguredGood:
                firmwareOptions = [DriveStaticModel.FIRMWARESTATUS.unconfiguredGood];
                if (epdSupported) {
                    firmwareOptions.push(DriveStaticModel.FIRMWARESTATUS.online);
                } else if (jbodState) {
                    firmwareOptions.push(DriveStaticModel.FIRMWARESTATUS.jbod);
                }
                break;
            case Store.JBOD:
                firmwareOptions = [DriveStaticModel.FIRMWARESTATUS.unconfiguredGood, DriveStaticModel.FIRMWARESTATUS.jbod];
                break;
            case Store.UnconfiguredBad:
                firmwareOptions = [DriveStaticModel.FIRMWARESTATUS.unconfiguredGood, DriveStaticModel.FIRMWARESTATUS.unconfiguredBad];
                break;
            default:
                for (const attr in DriveStaticModel.FIRMWARESTATUS) {
                    if (Object.prototype.hasOwnProperty.call(DriveStaticModel.FIRMWARESTATUS, attr) &&
                        DriveStaticModel.FIRMWARESTATUS[attr].id === key
                    ) {
                        firmwareOptions.push(DriveStaticModel.FIRMWARESTATUS[attr]);
                        break;
                    }
                }
                break;
        }
        return firmwareOptions;
    }

    /**
     * 获取选中的固件状态
     * @param key 接口返回值
     */
    public getFirmwareStatus(key: string): IOptions {
        for (const attr in DriveStaticModel.FIRMWARESTATUS) {
            if (Object.prototype.hasOwnProperty.call(DriveStaticModel.FIRMWARESTATUS, attr) &&
                DriveStaticModel.FIRMWARESTATUS[attr].id === key
            ) {
                return DriveStaticModel.FIRMWARESTATUS[attr];
            }
        }
        return null;
    }

    /**
     * 获取选中的定位状态
     * @param key 接口返回值
     */
    public getIndicator(key: string): ISwitch {
        for (const attr in DriveStaticModel.INDICATOR) {
            if (Object.prototype.hasOwnProperty.call(DriveStaticModel.INDICATOR, attr) &&
                DriveStaticModel.INDICATOR[attr].id === key
            ) {
                return DriveStaticModel.INDICATOR[attr];
            }
        }
        return null;
    }

    public getHotspareType(key: string): IRadio {
        for (const attr in DriveStaticModel.HOTSPARERADIOS) {
            if (Object.prototype.hasOwnProperty.call(DriveStaticModel.HOTSPARERADIOS, attr) &&
                DriveStaticModel.HOTSPARERADIOS[attr].id === key
            ) {
                return DriveStaticModel.HOTSPARERADIOS[attr];
            }
        }
        return null;
    }

    public getBootPriority(key: string): IOptions {
        return DriveStaticModel.pmcBootPriority.filter(item => {
            return item.id === key;
        })[0] || null;
    }
}
