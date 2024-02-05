import { VolumeMembersModel } from '../../models/volume-create/volume-members.model';
import { computeCapacity, unitConversion } from '../../utils/storage.utils';
import { DriveCheckBox, AvailableCapacity, ICapacity, ISpanCount, RaidType, StorageTree } from '../../models';

/**
 * 勾选物理盘触发--计算容量、容量单位、最大容量、最小容量
 */
export function autoCalculateByte(checkArr: any[], capacityNum: number, capacityOrigin) {
    let capacity: ICapacity = null;
    // 计算容量
    const autoSize = autoMinSize();
    const capacityValue = capacityNum > autoSize ? autoSize : capacityNum;
    if (checkArr && checkArr.length > 0) {
        const capacityObj = computeCapacity(capacityValue);
        capacity = {
            value: capacityObj.capacity,
            unit: capacityObj.unit,
            options: capacityOrigin.options,
            batyValue: capacityNum
        };
    } else {
        capacity = {
            value: null,
            unit: capacityOrigin.unit,
            options: capacityOrigin.options,
            batyValue: null
        };
    }
    capacity.oldValue = capacity.batyValue;
    // 计算和赋值最大容量、最小容量
    VolumeMembersModel.getInstance().setMinSize = autoSize;
    VolumeMembersModel.getInstance().setMaxSize = capacity.value;
    return capacity;
}

/**
 * 最小容量计算方法
 * a.Raid0所有盘容量较小的那个*盘个数
 * b.Raid1两个盘中容量较小的那个
 * c.Raid5勾选盘中容量较小的那个*(盘个数-1)
 * d.Raid6勾选盘中容量较小的那个*(盘个数-2)
 * e.Raid10勾选盘中容量较小的那个*(盘个数/2)
 * f.Raid50勾选盘中容量较小的那个*(盘个数 - 盘个数/每个Span的成员盘数)
 * g.Raid60勾选盘中容量较小的那个*(盘个数 - 盘个数*2/每个Span的成员盘数)
 * h.勾选“二级缓存”后，容量不可手动改变；不勾选可以修改容量，范围[64MB替换上面容量较小盘计算的值，以上计算的值]
 */
function autoMinSize(): number {
    const driveArr = VolumeMembersModel.getInstance().getCheckArr;
    const capacityArr = driveArr.filter(
        (driveCheckBox) => driveCheckBox.checked
    );
    const selectCount = capacityArr.length;
    if (selectCount === 0) {
        return 0;
    }
    // 获取勾选物理盘中的最小容量
    let minSizeBytes = Math.min.apply(Math, capacityArr.map((o) => o.getCapacity));
    const raidLevel = VolumeMembersModel.getInstance().getSelectData.volumeRaidLevel;
    const spanNum = VolumeMembersModel.getInstance().getSpanNum.value;

    switch (raidLevel.label) {
        case '0':
            minSizeBytes = minSizeBytes * selectCount;
            break;
        case '5':
            minSizeBytes = minSizeBytes * (selectCount - 1);
            break;
        case '6':
            minSizeBytes = minSizeBytes * (selectCount - 2);
            break;
        case '10':
            minSizeBytes = minSizeBytes * (selectCount / 2);
            break;
        case '50':
            minSizeBytes = minSizeBytes * (selectCount - selectCount / spanNum);
            break;
        case '60':
            minSizeBytes = minSizeBytes * (selectCount - (selectCount * 2) / spanNum);
            break;
        case '1(ADM)': case '10(ADM)':
            minSizeBytes = minSizeBytes * selectCount / 3;
            break;
        default:
            break;
    }
    minSizeBytes = Math.max(0, minSizeBytes);
    return minSizeBytes;
}

/**
 * 创建逻辑盘--容量单位变化，计算容量的方法。
 * @param unit 容量单位
 */
export function unitChange(unit: string): number {
    const available = VolumeMembersModel.getInstance().getAvailableCapacity;
    let minSizeBytes: number = null;
    let capacity: number = null;
    if (available && available.length > 0) {
        minSizeBytes = VolumeMembersModel.getInstance().availableSelect.getBatyValue;
    } else {
        minSizeBytes = autoMinSize();
        capacity = conversions(minSizeBytes, unit, 2);
    }
    return capacity;
}
/**
 * 创建逻辑盘--用户手动改变容量
 * @param capacityInput 用户输入的容量值
 * @param unit 容量单位
 */
export function capacityChange(capacityInput: number, unit: string): number {
    const capacityNum = conversions(capacityInput, unit, 1);
    let capacity = 0;
    let allCapacity = 0;
    const available = VolumeMembersModel.getInstance().getAvailableCapacity;
    if (available && available.length > 0) {
        allCapacity = VolumeMembersModel.getInstance().availableSelect.getBatyValue;
    } else {
        allCapacity = autoMinSize();
    }
    capacity = capacityNum >= allCapacity ? allCapacity : capacityNum;
    capacity = conversions(capacity, unit, 2);
    return capacity;
}

/**
 * 容量换算方法
 * @param value 容量数值
 * @param unit 容量单位
 * @param algorithm 算法方式，1是乘法，2是除法
 */
function conversions(value: number, unit: string, algorithm: number) {
    let capacity: number = null;
    switch (unit) {
        case 'MB':
            capacity = unitConversion(value, 2, algorithm, true);
            break;
        case 'GB':
            capacity = unitConversion(value, 3, algorithm, true);
            break;
        case 'TB':
            capacity = unitConversion(value, 4, algorithm, true);
            break;
        default:
            break;
    }
    return capacity;
}

/**
 * span成员组判定逻辑
 */
export function spanCount() {
    const selectCount = VolumeMembersModel.getInstance().getSelectData.drives.length;
    const spanRange = VolumeMembersModel.getInstance().getSelectCount;
    if (selectCount < spanRange.max) {
        changeDriveDisable(false);
        if (selectCount > 0) {
            const checkDrive = VolumeMembersModel.getInstance().getCheckArr.filter(
                driveBox => driveBox.getChecked
            )[0];
            if (checkDrive.getMediaType === 'SSD') {
                VolumeMembersModel.getInstance().getCheckArr.forEach((item) => {
                    if (item.getMediaType !== 'SSD') {
                        item.setDisable = true;
                    }
                });
            } else {
                VolumeMembersModel.getInstance().getCheckArr.forEach((item) => {
                    if (item.getMediaType === 'SSD') {
                        item.setDisable = true;
                    }
                });
            }
        }
    } else {
        changeDriveDisable(true);
    }
}
/**
 * 物理盘选中和容量计算方法
 * 1）遍历物理盘获取当前选中的物理盘数组
 * 2) 根据当前选中的物理盘数组初始化容量
 */
export function driveAndCapacityCount() {
    const volumeMembers = VolumeMembersModel.getInstance();
    const data = forEachDriveArr(volumeMembers.getCheckArr);
    volumeMembers.getSelectData.drives = data.checkArr || [];
    // 计算容量
    volumeMembers.setCapacityObj = autoCalculateByte(data.checkArr, data.capacityNum, volumeMembers.getCapacity);
}

/**
 * 物理盘disable状态赋值方法
 * @param state 状态
 */
export function changeDriveDisable(state: boolean) {
    const raidType = StorageTree.getInstance().getCheckedNode.getRaidType;
    const driveArr = VolumeMembersModel.getInstance().getCheckArr;
    driveArr.forEach(
        (drive: DriveCheckBox) => {
            if ((drive.getFirmware === 'Online' && raidType === RaidType.ARIES && !drive.getAvailableCapacity) ||
                (drive.getOpoInitState && raidType === RaidType.PMC)) {
                    drive.setDisable = true;
                    return;
            }
            drive.setDisable = state && !drive.checked ? state : drive.getIsDisable;
        }
    );
}

/**
 * 物理盘的遍历方法，返回选中的物理盘数量、名称和容量
 * @param driveCheckArr 创建逻辑盘界面--物理盘数组
 */
export function forEachDriveArr(driveCheckArr: DriveCheckBox[]) {
    let capacityNum = 0;
    let selectCount = 0;
    let checkArr = [];
    driveCheckArr.forEach(
        (item) => {
            if (item.getChecked) {
                checkArr.push(item.getID);
                selectCount++;
                capacityNum += item.getCapacity;
            }
        }
    );
    if (checkArr.length === 0) {
        checkArr = null;
    }
    return { checkArr, capacityNum, selectCount };
}

/**
 * 取消拥有盘组的物理盘选中状态
 * 1）isHaveRelated为true
 * 2）raid级别选中RAID0,不再置灰
 * 3）放开所有物理盘的Disablefang
 * 4) 计算物理盘和容量
 */
export function relateFalse() {
    const volumeMembers = VolumeMembersModel.getInstance();

    volumeMembers.isHaveRelated = false;
    volumeMembers.createRelated = false;
    // raid级别选中RAID0
    volumeMembers.setRAIDLevelSelect = volumeMembers.getRaidLevel.options[0];
    volumeMembers.setRaidLevelDisable = false;
    // 放开所有物理盘的Disablefang
    changeDriveDisable(false);
    // 计算物理盘和容量
    driveAndCapacityCount();

    // 清空可用容量
    volumeMembers.setAvailableCapacity = [];
}

/**
 * 选中拥有盘组的物理盘
 * 1）isHaveRelated为false
 * 2) raid级别、span成员盘数继承盘组的逻辑盘
 * 3）raid级别、span成员置灰不可编辑
 * 4）创建可用容量
 * 5) 容量初始化--可用容量的选中项
 * @param driveCheckBox 选中的物理盘
 */
export function relateTrue(driveCheckBox: DriveCheckBox) {
    const volumeMembers = VolumeMembersModel.getInstance();

    volumeMembers.isHaveRelated = true;
    volumeMembers.createRelated = true;

    // 盘容量未用完的物理盘为一个盘组，一个勾选本组均勾选，且级别、每个Span的成员盘数继承之前的逻辑盘
    volumeMembers.setRAIDLevelSelect = driveCheckBox.getRaidLevel;
    volumeMembers.setSpanNum = driveCheckBox.getSpanNum;
    volumeMembers.setRaidLevelDisable = true;
    volumeMembers.setSpanNumDisable = true;

    // 创建可用容量
    if (driveCheckBox.getAvailableCapacity && driveCheckBox.getAvailableCapacity.length > 0) {
        createAvailable(driveCheckBox.getAvailableCapacity);
    }
    volumeMembers.getAvailableCapacity[0].checked = true;
    // 计算物理盘和容量
    driveAndCapacityCount();
    // 容量初始化--可用容量的选中项
    volumeMembers.setCapacityObj = {
        value: volumeMembers.availableSelect.getValue,
        unit: volumeMembers.availableSelect.getUnit,
        options: volumeMembers.getCapacity.options,
        batyValue: volumeMembers.availableSelect.getBatyValue
    };
}

/**
 * 创建可用容量
 * @param availableCapacity 指定驱动器阵列中的剩余容量（单位：MB）
 */
export function createAvailable(availableCapacity: number[]) {
    const volumeMembers = VolumeMembersModel.getInstance();
    availableCapacity.forEach(
        (item) => {
            volumeMembers.addAvailableCapacity(
                new AvailableCapacity(
                    item
                )
            );
        }
    );
}

export function raidLevelCount(raidLevel: string, spanNum: number, driveNum: number, selectCount: ISpanCount,
    radType: string = RaidType.PMC, specials8iBoard: boolean = false) {
    let result: boolean = true;
    if (driveNum > selectCount.max || driveNum < selectCount.min || driveNum === 0) {
        return true;
    }
    const aguArr = [];
    const resultArr = [
        {
            raidLevel: '0',
            getResult: () => true
        },
        {
            raidLevel: '1',
            getResult: () => driveNum % 2 === 0
        },
        {
            raidLevel: '5',
            getResult: () => true
        },
        {
            raidLevel: '6',
            // 非95608i/16i的博通卡至少需要三个硬盘，其余的至少四个
            getResult: () => ((radType !== RaidType.BRCM || specials8iBoard) && driveNum >= 4) ||
                             ((radType === RaidType.BRCM && !specials8iBoard) && driveNum >= 3)
        },
        {
            raidLevel: '10',
            getResult: () => driveNum % 2 === 0
        },
        {
            raidLevel: '50',
            getResult: () => driveNum % spanNum === 0 && driveNum >= 6
        },
        {
            raidLevel: '60',
            // 非95608i/16i的博通卡至少需要6个硬盘，其余的（1880、pmc、9560-8i/16i）至少8个
            getResult: () => driveNum % spanNum === 0 &&
                (((radType === RaidType.BRCM && !specials8iBoard) && driveNum >= 6) ||
                ((radType !== RaidType.BRCM || specials8iBoard) && driveNum >= 8))
        },
        {
            raidLevel: '1(ADM)',
            getResult: () => driveNum === 3
        },
        {
            raidLevel: '10(ADM)',
            getResult: () => driveNum >= 6 && driveNum % 3 === 0
        },
        {
            raidLevel: '1Triple',
            getResult: () => driveNum === 3
        },
        {
            raidLevel: '10Triple',
            getResult: () => driveNum >= 6 && driveNum % 3 === 0
        },
    ];
    aguArr.push(raidLevel, spanNum, driveNum, radType);
    resultArr.forEach((item: any) => {
        if (item.raidLevel === aguArr[0]) {
            item.spanNum = aguArr[1];
            item.driveNum = aguArr[2];
            item.radType = aguArr[3];
            result = !item.getResult();
        }
    });
    return result;
}
