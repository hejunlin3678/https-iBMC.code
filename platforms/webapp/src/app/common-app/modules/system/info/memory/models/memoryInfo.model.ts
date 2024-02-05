import { ITitle } from './memory.interface';
import { Memory } from './memory.model';

export class MemoryInfo {
    public memoryArr: Memory[];

    // 提供给html的TiTable
    private columns: ITitle[];

    private maxCount: number;
    private activedCount: number;

    constructor(
        platform: string,
        maxCount: number,
        activedCount: number,
        memoryArr: Memory[]
    ) {
        this.setTitles(platform);
        this.maxCount = maxCount;
        this.activedCount = activedCount;
        this.memoryArr = memoryArr;
    }

    get getColumns() {
        return this.columns;
    }

    get getMemoryArr() {
        return this.memoryArr;
    }

    /**
     * 显示或隐藏某些内存标题方法
     * 1、Arm机型不显示当前频率
     * @param platform 架构型号：x86或Arm
     */
    private setTitles(platform: string) {
        this.columns = [
            {
                title: '',
                width: '8%',
                show: true
            },
            {
                title: 'COMMON_NAME',
                width: '15%',
                show: true
            },
            {
                title: 'COMMON_MANUFACTURER',
                width: '15%',
                show: true
            },
            {
                title: 'COMMON_CAPACITY',
                width: '15%',
                show: true
            },
            {
                title: 'MEMORY_CONFIGURED_SPEED_MTS',
                width: '15%',
                show: platform !== 'Arm' ? true : false
            },
            {
                title: 'MEMORY_MAX_SPEED_MTS',
                width: '15%',
                show: true
            },
            {
                title: 'COMMON_TYPE',
                width: '10%',
                show: true
            },
            {
                title: 'MEMORY_POSITION',
                width: '15%',
                show: true
            }
        ];
    }
}
