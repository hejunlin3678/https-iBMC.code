import { DriveCheckBox } from './drive-checkBox.model';
import { AvailableCapacity } from './available-capacity.model';
import { IOptions } from 'src/app/common-app/models';
import { IVolumeMembersData, ICapacity, ISelect, ISpanNum, ISpanCount } from '../storage-interface';
import { VolumeStaticModel } from '../static-data';
import { UntypedFormControl } from '@angular/forms';
import { StorageVaild } from '../../vaild/storage.vaild';
import { RaidType } from '..';
import { raidLevelCount } from '../../components';



/**
 * 创建逻辑盘
 * 二级缓存、RAID级别、span数量、物理盘和容量之间作用关系。
 */
export class VolumeMembersModel {
    // raid卡的名称，用来判断卡的类型
    static raidName: string;
    static raidType: string;
    static specials8iBoard: boolean;

    private constructor() {
        this.selectData = {
            volumeRaidLevel: null,
            spanNumber: null,
            capacityMBytes: null,
            drives: null,
        };
        this.availableCapacity = [];
    }

    get getSpanNum() {
        return this.spanNum;
    }

    set setCapacity(value: number) {
        const capacity = this.capacity;
        capacity.value = value;
        this.capacity = capacity;
        this.selectData.capacityMBytes = value;
    }

    set setRaidLevelDisable(disable: boolean) {
        const raidLevel = this.raidLevel;
        raidLevel.disable = disable;
        this.raidLevel = raidLevel;
    }


    set setSpanNumDisable(disable: boolean) {
        const spanNum = this.spanNum;
        this.spanNum.disable = disable;
        this.spanNum = spanNum;
    }

    set setCapacityObj(capacity: ICapacity) {
        this.capacity = capacity;
        this.selectData.capacityMBytes = capacity.batyValue;
    }

    get getCapacity() {
        return this.capacity;
    }

    get getSelectData(): IVolumeMembersData {
        return this.selectData;
    }

    get getCheckArr(): DriveCheckBox[] {
        return this.checkArr;
    }

    get getRaidLevel() {
        return this.raidLevel;
    }

    get getAvailableCapacity() {
        return this.availableCapacity;
    }

    get getSpanValid() {
        return this.spanValid;
    }

    get getSelectCount() {
        return this.selectCount;
    }

    set setAvailableCapacity(availableCapacity: AvailableCapacity[]) {
        this.availableCapacity = availableCapacity;
        this.availableSelect = availableCapacity ? availableCapacity[0] : null;
    }

    set setMinSize(minSize: number) {
        this.minSize = minSize;
    }

    set setMaxSize(maxSize: number) {
        this.maxSize = maxSize;
    }

    get getMinSize() {
        return this.minSize;
    }

    get getMaxSize() {
        return this.maxSize;
    }

    set setSpanValid(spanNum: ISpanNum) {
        this.spanValid = new UntypedFormControl(spanNum.value,
            StorageVaild.spanCount(
                this.spanNum.rang.min, this.spanNum.rang.max
            )
        );
    }

    set setSpanNum(spanNum: number) {
        this.spanNum.value = spanNum;
        if (!VolumeMembersModel.getInstance().createRelated) {
            const ariesState = VolumeMembersModel.raidType === RaidType.ARIES;
        this.selectCount = VolumeStaticModel.getSelectCount(
            VolumeMembersModel.getInstance().getSelectData.volumeRaidLevel,
                this.spanNum.value, ariesState
        );
        }
        this.setSpanValid = this.spanNum;
        this.selectData.spanNumber = this.spanNum.value;
    }

    set setRAIDLevelSelect(raidLevel: IOptions) {
        if (!VolumeMembersModel.getInstance().createRelated) {
        this.spanNum = VolumeStaticModel.createSpanNum(raidLevel);
            const ariesCard = VolumeMembersModel.raidType === RaidType.ARIES;
            this.selectCount = VolumeStaticModel.getSelectCount(raidLevel, this.spanNum.value, ariesCard);
        this.setSpanValid = this.spanNum;
        this.selectData.spanNumber = this.spanNum.value;
        }
        this.selectData.volumeRaidLevel = raidLevel.id !== 'noData' ? raidLevel : null;
    }

    // 使用剩余容量创建逻辑盘时根据选中的盘数来设置raid级别下拉参数
    set setRAIDLevelOptions(options) {
        const {relatedDriveNum, raidLevelList} = options;
        const raidLevelOptions = [];
        let matchList = [];
        if (relatedDriveNum === 1) {
            matchList = ['0'];
        } else if (relatedDriveNum === 2) {
            matchList = ['0', '1'];
        } else if (relatedDriveNum === 3) {
            matchList = ['0', '5', '1(ADM)', '1Triple'];
        } else if (relatedDriveNum === 4) {
            matchList = ['0', '5', '6', '10'];
        } else if (relatedDriveNum === 5) {
            matchList = ['0', '5', '6'];
        } else if (relatedDriveNum === 6) {
            matchList = ['0', '5', '6', '10', '50', '10(ADM)', '10Triple'];
        } else if (relatedDriveNum === 7) {
            matchList = ['0', '5', '6', '10', '50'];
        } else if (relatedDriveNum > 7 && relatedDriveNum < 129 && relatedDriveNum % 2 === 0 && relatedDriveNum % 3 !== 0) {
            // 偶数且不是3的倍数
            matchList = ['0', '5', '6', '10', '50', '60'];
        } else if (relatedDriveNum > 7 && relatedDriveNum < 129 && relatedDriveNum % 2 !== 0 && relatedDriveNum % 3 === 0) {
            // 奇数且为3的倍数
            matchList = ['0', '5', '6', '50', '60', '10(ADM)', '10Triple'];
        } else if (relatedDriveNum > 7 && relatedDriveNum < 129 && relatedDriveNum % 2 !== 0 && relatedDriveNum % 3 !== 0) {
            // 奇数且不是3的倍数
            matchList = ['0', '5', '6', '50', '60'];
        } else if (relatedDriveNum > 7 && relatedDriveNum < 129 && relatedDriveNum % 2 === 0 && relatedDriveNum % 3 === 0) {
            // 偶数且是3的倍数
            matchList = ['0', '5', '6', '10', '50', '60', '10(ADM)', '10Triple'];
        } else {
            if (VolumeMembersModel.raidType === RaidType.PMC) {
                matchList = ['0', '1', '5', '6', '10', '50', '60', '1(ADM)', '10(ADM)', '1Triple', '10Triple'];
            } else {
                matchList = ['0', '1', '5', '6', '10', '50', '60'];
            }
        }
        // 支持的raid级别与按照成员盘数算出来的raid级别取交集
        const raidLevelOption = matchList.filter(item => {
            return raidLevelList.includes(item);
        });
        raidLevelOption.forEach(item => {
            raidLevelOptions.push({
                name: 'IOptions',
                id: `RAID${item}`,
                label: item
            });
        });
        this.raidLevel.options = raidLevelOptions;
    }

    private static instance: VolumeMembersModel;

    private spanValid: any;
    // RAID级别
    private raidLevel: ISelect = {
        disable: false,
        options: []
    };

    // 每个span的成员盘数
    private spanNum: ISpanNum = {
        value: null,
        disable: false,
        rang: null
    };

    // 物理盘可选择数量
    private selectCount: ISpanCount;

    // 容量
    private capacity: ICapacity = {
        value: null,
        unit: VolumeStaticModel.capacityUnit[2],
        options: VolumeStaticModel.capacityUnit,
        batyValue: null,
        oldValue: null
    };

    private minSize: number;
    private maxSize: number;

    public validStatus: boolean;

    // 勾选的物理盘
    private checkArr: DriveCheckBox[];

    public availableSelect: AvailableCapacity;

    // 勾选的物理盘中是否存在组合场景
    public isHaveRelated: boolean = false;

    // 是否使用剩余容量创建逻辑盘
    public createRelated: boolean = false;

    private selectData: IVolumeMembersData;

    private availableCapacity: AvailableCapacity[];
    public static getInstance(): VolumeMembersModel {
        if (!VolumeMembersModel.instance) {
            VolumeMembersModel.instance = new VolumeMembersModel();
        }
        return VolumeMembersModel.instance;
    }

    public static initInstance(
        raidLevel: {
            disable: boolean,
            levels: [],
            towState: boolean,
            raidType?: string,
            specials8iBoard?: boolean
        },
        drives: DriveCheckBox[],
        name?: string
    ) {
        const model = VolumeMembersModel.getInstance();
        model.raidLevel = {
            disable: raidLevel.disable,
            options: VolumeStaticModel.createRAIDLevel(raidLevel.levels, raidLevel.towState, raidLevel.raidType)
        };
        this.raidName = name;
        this.raidType = raidLevel.raidType;
        model.addCheckArr(drives, raidLevel.towState, this.raidType);
        this.specials8iBoard = raidLevel.specials8iBoard;

        model.selectData = {
            volumeRaidLevel: model.getRaidLevel.options[0],
            spanNumber: null,
            capacityMBytes: null,
            drives: null,
        };

        model.spanNum = VolumeStaticModel.createSpanNum(model.getRaidLevel.options[0]);
        model.selectCount = VolumeStaticModel.getSelectCount(
            VolumeMembersModel.getInstance().getSelectData.volumeRaidLevel,
            model.spanNum.value
        );
        // 清空可用容量和容量
        model.availableCapacity = [];

        model.setSpanValid = model.spanNum;
    }

    public static destroy(): void {
        if (VolumeMembersModel.instance) {
            VolumeMembersModel.instance = null;
        }
    }

    public addAvailableCapacity(availableCapacity: AvailableCapacity) {
        this.availableCapacity.push(availableCapacity);
        this.availableSelect = this.availableSelect ? this.availableSelect : this.availableCapacity[0];
    }

    public delAvailableCapacity(index: number) {
        this.availableCapacity.splice(index, 1);
    }

    public addCheckArr(checkBoxArr: DriveCheckBox[], twoState: boolean, raidType: string = RaidType.BRCM) {
        const checkArr: DriveCheckBox[] = [];
        checkBoxArr.forEach(
            (checkBox: DriveCheckBox) => {
                checkBox.checkDisableState(twoState);
                // hi1880卡物理盘裸盘（raid成员盘）固件状态为Online时需置灰
                if (raidType === RaidType.ARIES && checkBox.getFirmware === 'Online' && !checkBox.getAvailableCapacity) {
                    checkBox.setDisable = true;
                }
                if (!checkBox.getDisable) {
                    checkBox.checked = false;
                    checkArr.push(checkBox);
                }
            }
        );
        if (checkArr.length > 0) {
            this.checkArr = checkBoxArr;
        } else {
            // 如果所有的物理盘都disable，就显示当前无可操作物理盘
            this.checkArr = [];
        }
        this.selectData.drives = [];
    }

    public checkSave(): boolean {
        // 如果没有可操作物理盘、没有选中物理盘、span成员数没有校验通过，不能保存
        if (!this.checkArr ||
            this.checkArr.length < 1 ||
            !this.selectData.drives ||
            this.selectData.drives.length < 1 ||
            !this.selectData.capacityMBytes ||
            !this.validStatus ||
            !this.selectData.volumeRaidLevel
        ) {
            return true;
        } else {
            return raidLevelCount(
                this.selectData.volumeRaidLevel.label,
                this.selectData.spanNumber,
                this.selectData.drives.length,
                this.selectCount,
                VolumeMembersModel.raidType,
                VolumeMembersModel.specials8iBoard
            );
        }
    }
}
