import { IOptions } from 'src/app/common-app/models';
import { VolumeMembersModel } from './volume-members.model';

export class DriveCheckBox {
    private boxId: number;
    private driveId: string;
    private name: string;
    public checked: boolean = true;
    // html控件disable判定字段
    private disable: boolean;
    // 接口返回初始disable判定字段
    private isDisable: boolean;
    private capacity: number;
    // 驱动器阵列的成员
    private related: DriveCheckBox[];
    // 指定驱动器阵列中的剩余容量（单位：MB）
    private availableCapacity: number[];
    // 驱动器阵列的成员
    private relatedName: string[];
    private mediaType: string;
    // 接口类型
    private protocol: string;
    private ssdCachecadeVolume: boolean;
    private firmware: string;
    private raidLevel: IOptions;
    private spanNumber: number;
    // enhanced physical drive, true：不能创建逻辑盘
    private isEPD: boolean;
    // 剩余可用总容量
    private totalFreeSpaceMiB: number;
    // 物理盘缓存策略
    private diskCachePolicy: string;
    // 初始化是否选择了opo
    private opoInitState: boolean = false;

    constructor(
        id: number,
        driveId: string,
        name: string,
        capacity: number,
        relatedName: string[],
        mediaType: string,
        protocol: string,
        ssdCachecadeVolume: boolean,
        firmware: string,
        raidLevel: IOptions,
        spanNumber: number,
        availableCapacity: number[],
        isEPD: boolean,
        totalFreeSpaceMiB: number,
        diskCachePolicy?: string
    ) {
        this.disable = isEPD;
        this.checked = false;
        this.boxId = id;
        this.driveId = driveId;
        this.name = name;
        this.capacity = capacity;
        this.relatedName = relatedName;
        this.mediaType = mediaType;
        this.protocol = protocol;
        this.ssdCachecadeVolume = ssdCachecadeVolume;
        this.firmware = firmware;
        this.raidLevel = raidLevel;
        this.spanNumber = spanNumber;
        this.availableCapacity = availableCapacity;
        this.isEPD = isEPD;
        this.totalFreeSpaceMiB = totalFreeSpaceMiB;
        this.diskCachePolicy = diskCachePolicy;
    }

    set setRelated(related: DriveCheckBox[]) {
        this.related = related;
        this.checkDisableState(false);
    }

    set setDisable(disable: boolean) {
        this.disable = this.isEPD ? true : disable;
    }

    set setOpoInitState(state: boolean) {
        this.opoInitState = state;
    }

    /**
     * 选中物理盘
     * @param checked 物理盘选中状态
     * @param origin
     */
    set setChecked(checked: boolean) {
        this.checked = checked;
        /**
         * 选中拥有盘组的物理盘
         * 一个勾选本组均勾选，
         * raid级别、Span的成员盘数继承之前的逻辑盘
         * 清空非盘组成员的选中状态并禁用
         */
        if (this.relatedName && this.relatedName.length > 0) {
            VolumeMembersModel.getInstance().getCheckArr.forEach(
                (drive: DriveCheckBox) => {
                    if (this.relatedName.indexOf(drive.getDriveID) > -1) {
                        drive.checked = checked;
                    } else {
                        drive.checked = false;
                        drive.disable = checked ? true : this.isDisable;
                    }
                }
            );
        }
    }

     // 清空物理盘选中状态而不增加其它操作
     set setCheckedAllClear(checked: boolean) {
        this.checked = checked;
    }

    get getRaidLevel(): IOptions {
        return this.raidLevel;
    }

    get getRelatedName() {
        return this.relatedName;
    }

    get getID() {
        return this.boxId;
    }

    get getDriveID() {
        return this.driveId;
    }

    get getName() {
        return this.name;
    }

    get getDisable() {
        return this.disable;
    }

    get getIsDisable() {
        return this.isDisable;
    }

    get getChecked() {
        return this.checked;
    }

    get getCapacity() {
        return this.capacity;
    }

    get getRelated() {
        return this.related;
    }

    get getMediaType() {
        return this.mediaType;
    }

    get getProtocol() {
        return this.protocol;
    }

    get getSSDCachecadeVolume() {
        return this.ssdCachecadeVolume;
    }
    get getFirmware() {
        return this.firmware;
    }

    get getSpanNum(): number {
        return this.spanNumber;
    }

    get getAvailableCapacity() {
        return this.availableCapacity;
    }
    get getTotalFreeSpaceMiB() {
        return this.totalFreeSpaceMiB;
    }
    get getDiskCachePolicy() {
        return this.diskCachePolicy;
    }
    get getOpoInitState() {
        return this.opoInitState;
    }

    /**
     * 判定disable状态
     * @param checked 二级缓存是否选中
     */
    public checkDisableState(checked: boolean) {
        if (checked) {
            if (['UnconfiguredGood', 'Rebuilding', 'Ready'].indexOf(this.firmware) > -1 && this.mediaType === 'SSD' ||
                (this.getSSDCachecadeVolume && this.getTotalFreeSpaceMiB)) {
                this.judgeRelated();
            } else {
                this.disable = true;
            }
        } else {
            if (['UnconfiguredGood', 'Online', 'Ready'].indexOf(this.firmware) > -1 && !this.getSSDCachecadeVolume) {
                this.judgeRelated();
            } else {
                this.disable = true;
            }
        }
        this.isDisable = this.isEPD ? true : this.disable;
    }

    private judgeRelated() {
        if (this.related && this.capacity === 0) {
            this.disable = true;
        } else {
            this.disable = this.isEPD;
        }
    }
}
