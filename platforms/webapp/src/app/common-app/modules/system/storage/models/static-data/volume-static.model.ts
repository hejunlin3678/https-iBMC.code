import { IOptions } from 'src/app/common-app/models';
import { ISpanNum, IStorageRadio, ISpanCount, RaidType } from '../storage-interface';


export class VolumeStaticModel {

    private constructor() { }

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

    // SSCD功能
    public static readonly sscdFunction: IStorageRadio[] = [
        {
            label: 'STORE_ENABLED',
            id: true,
            disable: false,
            name: 'IRadio',
            attrName: 'sscd'
        }, {
            label: 'STORE_DISABLED',
            id: false,
            disable: false,
            name: 'IRadio',
            attrName: 'sscd'
        }
    ];

    // 物理盘缓存状态
    public static readonly driverCachePolicy: IOptions[] = [
        {
            id: 'Unchanged',
            label: 'Disk\'s Default',
            name: 'IOptions'
        }, {
            id: 'Enabled',
            label: 'Enabled',
            name: 'IOptions'
        }, {
            id: 'Disabled',
            label: 'Disabled',
            name: 'IOptions'
        }
    ];

    // 访问策略
    public static readonly accessPolicy: IOptions[] = [
        {
            id: 'ReadWrite',
            label: 'Read Write',
            name: 'IOptions'
        }, {
            id: 'ReadOnly',
            label: 'Read Only',
            name: 'IOptions'
        }, {
            id: 'Blocked',
            label: 'Blocked',
            name: 'IOptions'
        }
    ];

    // BGI状态
    public static readonly bgiState: IOptions[] = [
        {
            id: true,
            label: 'STORE_ENABLED',
            name: 'IOptions'
        }, {
            id: false,
            label: 'STORE_DISABLED',
            name: 'IOptions'
        }
    ];

    // IO策略
    public static readonly ioPolicy: IOptions[] = [
        {
            id: 'DirectIO',
            label: 'Direct IO',
            name: 'IOptions'
        }, {
            id: 'CachedIO',
            label: 'Cached IO',
            name: 'IOptions'
        }
    ];

    // 写策略
    public static readonly writePolicy: IOptions[] = [
        {
            id: 'WriteThrough',
            label: 'Write Through',
            name: 'IOptions'
        }, {
            id: 'WriteBackWithBBU',
            label: 'Write Back With BBU',
            name: 'IOptions'
        }, {
            id: 'WriteBack',
            label: 'Write Back',
            name: 'IOptions'
        }
    ];

    // 读策略
    public static readonly readPolicy: IOptions[] = [
        {
            id: 'NoReadAhead',
            label: 'No Read Ahead',
            name: 'IOptions'
        }, {
            id: 'ReadAhead',
            label: 'Read Ahead',
            name: 'IOptions'
        }
    ];

    // 初始化类型
    public static readonly initMode: IOptions[] = [
        {
            id: 'UnInit',
            label: 'No Init',
            name: 'IOptions',
        },
        {
            id: 'QuickInit',
            label: 'Quick Init',
            name: 'IOptions',
        },
        {
            id: 'FullInit',
            label: 'Full Init',
            name: 'IOptions',
        },
        {
            id: 'RPI',
            label: 'RPI',
            name: 'IOptions',
        },
        {
            id: 'OPO',
            label: 'OPO',
            name: 'IOptions',
        },
        {
            id: 'Front',
            label: 'Front',
            name: 'IOptions',
        },
        {
            id: 'Background',
            label: 'Background',
            name: 'IOptions',
        }
    ];

    // 加速方法选项
    public static readonly accelerationMethod: IOptions[] = [
        {
            id: 'None',
            label: 'None',
            name: 'IOptions',
        },
        {
            id: 'ControllerCache',
            label: 'Controller Cache',
            name: 'IOptions',
        },
        {
            id: 'IOBypass',
            label: 'IO Bypass',
            name: 'IOptions',
        }
    ];

    // cacheLineSize 选项
    public static readonly cacheLineSize: IOptions[] = [
        {
            id: '64K',
            label: '64K',
            name: 'IOptions',
        },
        {
            id: '256K',
            label: '256K',
            name: 'IOptions',
        }
    ];

    // 容量单位
    public static readonly capacityUnit: IOptions[] = [
        {
            id: 'MB',
            label: 'MB',
            name: 'IOptions'
        },
        {
            id: 'GB',
            label: 'GB',
            name: 'IOptions'
        },
        {
            id: 'TB',
            label: 'TB',
            name: 'IOptions'
        }
    ];

    // PMC条带大小，当前为固定列表状态
    public static readonly stripSize: IOptions[] = [
        {
            id: 16 * 1024,
            label: '16 KB',
            name: 'IOptions'
        },
        {
            id: 32 * 1024,
            label: '32 KB',
            name: 'IOptions'
        },
        {
            id: 64 * 1024,
            label: '64 KB',
            name: 'IOptions'
        },
        {
            id: 128 * 1024,
            label: '128 KB',
            name: 'IOptions'
        },
        {
            id: 256 * 1024,
            label: '256 KB',
            name: 'IOptions'
        },
        {
            id: 512 * 1024,
            label: '512 KB',
            name: 'IOptions'
        },
        {
            id: 1024 * 1024,
            label: '1 MB',
            name: 'IOptions'
        }
    ];

    // PMC加速方法
    public static readonly speedMethod: IOptions[] = [
        {
            id: 'None',
            label: 'None',
            name: 'IOptions'
        },
        {
            id: 'ControllerCache',
            label: 'Controller Cache',
            name: 'IOptions'
        },
        {
            id: 'IOBypass',
            label: 'IO Bypass',
            name: 'IOptions'
        }
    ];

    // PMC写策略
    public static readonly pmcWritePolicy: IOptions[] = [
        {
            id: 'WriteThrough',
            label: 'Write Through',
            name: 'IOptions'
        }, {
            id: 'WriteBackWithBBU',
            label: 'Write Back With BBU',
            name: 'IOptions'
        }
    ];

    // 启动盘
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

    // 所有PMC卡的普通逻辑盘列表
    public static readonly logicDriverList: any = {};

    /**
     * 条带大小下拉选项生成方法
     * @param maxSize
     * @param minSize
     */
    public static createStripeSize(maxSize: number, minSize: number = 64 * 1024): IOptions[] {
        const sizeArr: IOptions[] = [];
        for (let i = 16; i * 1024 <= maxSize; i = i * 2) {
            if (i * 1024 >= minSize && i !== 1024) {
                sizeArr.push({
                    name: 'IOptions',
                    id: i * 1024,
                    label: i + ' KB'
                });
            } else if (i === 1024) {
                sizeArr.push({
                    name: 'IOptions',
                    id: 1024 * 1024,
                    label: '1 MB'
                });
            }
        }
        return sizeArr;
    }

    /**
     * RAID级别下拉选项生成逻辑
     * @param raidLevels
     * @param isCerVerfication
     */
    public static createRAIDLevel(raidLevels: [], isCerVerfication: boolean, raidType: string): IOptions[] {
        const levelArr: IOptions[] = [];
        if (raidLevels && raidLevels.length > 0) {
            raidLevels.forEach((raidLevel) => {
                if (isCerVerfication && (Number(raidLevel) === 1 || Number(raidLevel) === 0)) {
                    // 只能选Raid0、Raid1
                    levelArr.push({
                        name: 'IOptions',
                        id: `RAID${raidLevel}`,
                        label: raidLevel
                    });
                } else if (isCerVerfication && raidType === RaidType.PMC && [0, 1, 10, 5].includes(Number(raidLevel))) {
                    // 选择二级缓存的pmc卡只能选Raid0、Raid1，Raid10、Raid5
                    levelArr.push({
                        name: 'IOptions',
                        id: `RAID${raidLevel}`,
                        label: raidLevel
                    });
                } else if (!isCerVerfication) {
                    levelArr.push({
                        name: 'IOptions',
                        id: `RAID${raidLevel}`,
                        label: raidLevel
                    });
                }
            });
        } else {
            levelArr.push({
                name: 'IOptions',
                id: `noData`,
                label: 'COMMON_NO_DATA'
            });
        }
        return levelArr;
    }

    /**
     * 根据RAID级别生成span的成员逻辑
     * @param raidLevel
     */
    public static createSpanNum(raidLevel: IOptions): ISpanNum {
        const spanNum: ISpanNum = {
            value: null,
            disable: true,
            rang: { min: 1, max: 32 },
        };
        switch (Number(raidLevel.label)) {
            case 10:
                spanNum.value = 2;
                break;
            case 50:
                spanNum.value = 3;
                spanNum.disable = false;
                spanNum.rang = { min: 3, max: 32 };
                break;
            case 60:
                spanNum.value = 4;
                spanNum.disable = false;
                spanNum.rang = { min: 4, max: 32 };
                break;
            default:
                break;
        }
        return spanNum;
    }

    // 根据RAID级别判定可以选择物理盘的个数
    public static getSelectCount(raidLevel: IOptions, spanNum: number, ariesCard: boolean = false): { min: number, max: number } {
        // 默认支持的可勾选物理盘个数范围
        const selectCount: ISpanCount = {
            min: 0,
            max: 32,
        };
        // span个数（支持子组数）范围
        const spanNumRange: ISpanCount = {
            min: 2,
            max: 8
        };
        switch (Number(raidLevel.label)) {
            case 1:
                selectCount.min = 2;
                selectCount.max = 2;
                break;
            case 5:
            case 6:
                selectCount.min = 3;
                break;
            case 0:
            case 5:
            case 6:
                selectCount.min = 0;
                selectCount.max = 32;
                break;
            case 1:
                selectCount.min = 2;
                selectCount.max = 32;
                break;
            case 10:
                selectCount.min = spanNum * spanNumRange.min;
                selectCount.max = spanNum * spanNumRange.max;
                // hi1880卡raid10最多允许选30个物理盘
                if (ariesCard) {
                    selectCount.max = 30;
                }
                break;
            case 50:
            case 60:
                selectCount.min = spanNum * spanNumRange.min;
                selectCount.max = spanNum * spanNumRange.max;
                break;
            default:
                break;
        }
        return selectCount;
    }

    public static getIsBootDisk(key: boolean): IStorageRadio {
        const result = VolumeStaticModel.isBootDisk.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getSSCDFunction(key: boolean): IStorageRadio {
        const result = VolumeStaticModel.sscdFunction.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getDriverCachePolicy(key: string): IOptions {
        const result = VolumeStaticModel.driverCachePolicy.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getAccessPolicy(key: string): IOptions {
        const result = VolumeStaticModel.accessPolicy.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getBgiState(key: boolean): IOptions {
        const result = VolumeStaticModel.bgiState.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getIOPolicy(key: string): IOptions {
        const result = VolumeStaticModel.ioPolicy.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getWritePolicy(key: string): IOptions {
        const result = VolumeStaticModel.writePolicy.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    public static getReadPolicy(key: string): IOptions {
        const result = VolumeStaticModel.readPolicy.filter((opt) => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }


    // PMC条带大小
    public static getStripeSizeBytes(key: number): IOptions {
        const result = VolumeStaticModel.stripSize.filter(opt => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    // PMC加速方法
    public static getSpeedMethod(key: string): IOptions {
        const result = VolumeStaticModel.speedMethod.filter(opt => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    // PMC启动盘
    public static getBootPriority(key: string): IOptions {
        const result = VolumeStaticModel.pmcBootPriority.filter(opt => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    // PMC容量单位
    public static getCapacityUnit(key: string): IOptions {
        const result = VolumeStaticModel.capacityUnit.filter(opt => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }

    // PMC写策略
    public static getPmcWritePolicy(key: string): IOptions {
        const result = VolumeStaticModel.pmcWritePolicy.filter(opt => opt.id === key);
        return result.length > 0 ? result[0] : null;
    }
}
