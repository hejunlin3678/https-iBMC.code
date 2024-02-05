import { Component, OnInit } from '@angular/core';
import { VolumeCreateService } from '../../volume-create.service';
import { IOptions } from 'src/app/common-app/models';
import { VolumeMembersModel, VolumeCreateModel, DriveCheckBox, AvailableCapacity, VolumePolicyModel, RaidType } from '../../../../models';
import { unitChange, capacityChange, driveAndCapacityCount, relateFalse, changeDriveDisable, relateTrue, spanCount } from '../../volume-create.utils';
import { TiValidationConfig } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormControl } from '@angular/forms';
import { computeCapacity, transCapacity } from '../../../../utils';
import { StorageVaild } from '../../../../vaild/storage.vaild';
import { formatEntry } from 'src/app/common-app/utils';

@Component({
    selector: 'app-volume-members',
    templateUrl: './volume-members.component.html',
    styleUrls: ['./volume-members.component.scss']
})
export class VolumeMembersComponent implements OnInit {

    public volumeMembers: VolumeMembersModel;
    public volumeCreate: VolumeCreateModel;
    public volumePolicy: VolumePolicyModel;
    public driveArr: DriveCheckBox[];
    public spanValid: UntypedFormControl;
    public validaTip: string = this.i18n.instant('STORE_INTEGER3TO32', [3, 32]);

    // 判定spancount是否为用户输入
    public userInput: boolean = false;

    public validation = this.setValidaTip();
    public isPMCCard: boolean = false;
    public isAriesCard: boolean = false;
    public isSpecials8iBoard: boolean = false;
    public raidLevel: string[] = [];

    constructor(
        private volumeCreateService: VolumeCreateService,
        private i18n: TranslateService
    ) {
        this.volumeMembers = VolumeMembersModel.getInstance();
        this.volumeCreate = VolumeCreateModel.getInstance();
        this.volumePolicy = VolumePolicyModel.getInstance();
        this.spanValid = new UntypedFormControl(null, StorageVaild.spanCount(0, 32));
    }

    ngOnInit(): void {
        this.volumeCreateService.raidNode$.subscribe(
            (raidNode) => {
                const raidLevel = raidNode.getRaid.getRAIDLevels;
                if (raidLevel.value instanceof Array) {
                    raidLevel.value.forEach(item => {
                        this.raidLevel.push(item);
                    });
                }
                if (raidNode.getRaidType === RaidType.PMC || raidNode.getRaidType === RaidType.HBA) {
                    this.isPMCCard = true;
                } else if (raidNode.getRaidType === RaidType.ARIES) {
                    this.isAriesCard = true;
                }
                this.isSpecials8iBoard = raidNode.getSpecials8iBoard;
                VolumeMembersModel.initInstance(
                    {
                        disable: false,
                        levels: raidLevel ? raidLevel.value as [] : null,
                        towState: false,
                        raidType: raidNode.getRaidType,
                        specials8iBoard: this.isSpecials8iBoard
                    },
                    raidNode.getRaid.getDriveArr,
                    raidNode.getRaid.getName

                );
                driveAndCapacityCount();
                this.driveArr = this.volumeMembers.getCheckArr;
                this.setSpanValid(this.volumeMembers.getSpanNum, this.volumeMembers.getSpanValid.validator);
                // 只在校验为enabled的时候获取校验状态
                this.volumeMembers.validStatus = this.spanValid.enabled ? this.spanValid.valid : true;
            }
        );
    }

    /**
     * 选中物理盘触发逻辑
     * @param checkDrive 选中物理盘对象
     * @param state 选中物理盘状态
     */
    public changeCheckState(checkDrive: DriveCheckBox, state: boolean) {
        // 更改物理盘的选中状态
        checkDrive.setChecked = state;
        const raidLevelOption = {
            relatedDriveNum: null,
            raidLevelList: this.raidLevel
        };
        // 如果选中的物理盘拥有关联对象
        if (checkDrive.getRelated) {
            if (state) {
                if (this.isPMCCard) {
                    this.volumeMembers.isHaveRelated = true;
                    this.volumeMembers.createRelated = true;
                    // 盘容量未用完的物理盘为一个盘组，一个勾选本组均勾选，且级别、每个Span的成员盘数继承之前的逻辑盘
                    const raidLevel = checkDrive.getRaidLevel;
                    if (!String(raidLevel.id).includes('RAID')) {
                        raidLevel.id = 'RAID' + raidLevel.id;
                    }
                    this.volumeMembers.setRAIDLevelSelect = raidLevel;
                    this.volumeMembers.setSpanNum = checkDrive.getSpanNum;
                    this.spanValid.enable();
                    // 新增可用容量
                    this.volumeMembers.addAvailableCapacity(new AvailableCapacity(checkDrive.getAvailableCapacity[0]));
                    const checkedDrivesList = [];
                    this.volumeMembers.getCheckArr.forEach(item => {
                        if (item.checked) {
                            checkedDrivesList.push(item.getID);
                        }
                    });
                    this.volumeMembers.getSelectData.drives = checkedDrivesList;
                    raidLevelOption.relatedDriveNum = checkedDrivesList.length;
                    this.volumeMembers.setRAIDLevelOptions = raidLevelOption;
                    this.paramChanged();
                    // 更改span成员数校验范围和校验提示词
                    this.spanValid.setValue(this.volumeMembers.getSpanNum.value);
                } else {
                    relateTrue(checkDrive);
                    // 物理盘缓存策略的值设置为已选物理盘对应的缓存策略值
                    const driveCachePolicy = VolumePolicyModel.getInstance().getDriverCachePolicy.options.filter(item =>
                        item.id === checkDrive?.getDiskCachePolicy);
                    this.volumePolicy.selectData.driveCachePolicy = driveCachePolicy[0];
                    if (this.isAriesCard) {
                        this.volumePolicy.setDriverCachePolicy = true;
                    }
                    // 更改span成员数校验范围和校验提示词
                    this.spanValid.setValue(this.volumeMembers.getSpanNum.value);
                    this.spanValid.disable();
                }
            } else {
                relateFalse();
                this.volumePolicy.selectData.driveCachePolicy = VolumePolicyModel.getInstance().getDriverCachePolicy.options[0];
                if (this.isAriesCard) {
                    // 取消物理盘缓存策略置灰，置为初始值
                    this.volumePolicy.setDriverCachePolicy = false;
                }
                // 重置raid级别可选项
                this.volumeMembers.setRAIDLevelOptions = raidLevelOption;
                // 更改span成员数校验范围和校验提示词
                this.spanValid.setValue(null);
                this.setSpanValid(this.volumeMembers.getSpanNum, StorageVaild.spanCount(0, 32));
                // 子组盘不可用
                this.spanValid.disable();
            }
        } else {
            driveAndCapacityCount();
            spanCount();
            if (this.volumePolicy.getTwoCache) {
                // 创建maxCache逻辑盘容量处理
                this.setMaxChaceCapacity(true);
            }
            this.diskMixedGroup();
        }
        this.driveArr = this.volumeMembers.getCheckArr;
    }
    public diskMixedGroup() {
        const checkedDisk = this.volumeMembers.getCheckArr.filter(item => item.checked);
        if (checkedDisk.length === 0) { return; }
        this.volumeMembers.getCheckArr.forEach(item => {
            // 博通卡：当介质类型是SSD时，SAS和SATA不能混组
            const disableRuleBT = !this.isAriesCard && !this.isPMCCard && (checkedDisk[0].getMediaType === 'SSD') &&
                (item.getProtocol !== checkedDisk[0].getProtocol);
            // pmc卡：SAS和SATA不能混组
            const disableRulePmc = this.isPMCCard && (item.getProtocol !== checkedDisk[0].getProtocol);
            // 普通盘和SSD盘不能混组
            const disableRuleType = (item.getMediaType !== checkedDisk[0].getMediaType);
            if (disableRuleBT || disableRulePmc || disableRuleType) {
                item.setDisable = true;
            }
        });
    }

    // 创建maxChace逻辑盘时限制可设置的最大容量
    public setMaxChaceCapacity(isInit: boolean = false) {
        let allowedMaxCapacity = 1.7 * 1024 * 1024 * 1024 * 1024;
        const currentCapacity = this.volumeMembers.getSelectData.capacityMBytes;
        const logicalMaxCapacity = Number(this.volumePolicy.getselectData.associationLogical.maxCapacity);
        if (this.volumePolicy.getselectData.cacheLineSize.id === '64K') {
            allowedMaxCapacity = 1.7 * 1024 * 1024 * 1024 * 1024;
        } else if (this.volumePolicy.getselectData.cacheLineSize.id === '256K') {
            allowedMaxCapacity = 6.8 * 1024 * 1024 * 1024 * 1024;
        }
        const maxCapacityMBytes = Math.min(allowedMaxCapacity, currentCapacity, logicalMaxCapacity);
        const result = computeCapacity(maxCapacityMBytes);
        let capacityInfo = null;
        if (isInit) {
            // 初始化时容量赋值有‘oldValue’属性表示容量值未变更，以便参数处理为null
            capacityInfo = {
                value: result.capacity,
                unit: result.unit,
                options: this.volumeMembers.getCapacity.options,
                batyValue: maxCapacityMBytes,
                oldValue: maxCapacityMBytes
            };
        } else {
            capacityInfo = {
                value: result.capacity,
                unit: result.unit,
                options: this.volumeMembers.getCapacity.options,
                batyValue: maxCapacityMBytes,
            };
        }
        this.volumeMembers.setCapacityObj = capacityInfo;
    }

    // 修改Raid级别、子组成员盘数及物理盘导致的参数变化
    public paramChanged() {
        const drives = this.volumeMembers.getSelectData.drives;
        const spanNumber = this.volumeMembers.getSelectData.spanNumber;
        const volumeRaidLevel = this.volumeMembers.getSelectData.volumeRaidLevel;
        const capacityMBytes = this.volumeMembers.getCheckArr.filter(item => item.getID === drives[0])[0].getTotalFreeSpaceMiB;
        if (volumeRaidLevel.label === '50') {
            this.volumeMembers.setSpanNumDisable = false;
            if (spanNumber < 3 || spanNumber > this.volumeMembers.getSelectData.drives.length / 2) {
                return;
            }
        } else if (volumeRaidLevel.label === '60') {
            this.volumeMembers.setSpanNumDisable = false;
            if (spanNumber < 4 || spanNumber > this.volumeMembers.getSelectData.drives.length / 2) {
                return;
            }
        } else {
            this.volumeMembers.setSpanNumDisable = true;
            this.volumeMembers.setSpanNum = null;
            this.spanValid.setValue(null);
            this.spanValid.disable();
        }
        let availableBate = 0;
        switch (volumeRaidLevel.label) {
            case '1': case '10':
                availableBate = capacityMBytes / 2;
                break;
            case '1(ADM)': case '10(ADM)': case '1Triple': case '10Triple':
                availableBate = capacityMBytes / 3;
                break;
            case '5':
                availableBate = capacityMBytes * (drives.length - 1) / drives.length;
                break;
            case '6':
                availableBate = capacityMBytes * (drives.length - 2) / drives.length;
                break;
            case '50':
                availableBate = capacityMBytes * (drives.length - drives.length / spanNumber) / drives.length;
                break;
            case '60':
                availableBate = capacityMBytes * (drives.length - 2 * drives.length / spanNumber) / drives.length;
                break;
            default:
                availableBate = capacityMBytes;
                break;
        }
        this.volumeMembers.setAvailableCapacity = [new AvailableCapacity(availableBate)];
        // 容量初始化--可用容量的选中项
        this.volumeMembers.setCapacityObj = {
            value: this.volumeMembers.availableSelect.getValue,
            unit: this.volumeMembers.availableSelect.getUnit,
            options: this.volumeMembers.getCapacity.options,
            batyValue: this.volumeMembers.availableSelect.getBatyValue,
            oldValue: this.volumeMembers.availableSelect.getBatyValue,
        };
        this.volumeMembers.checkSave();
    }

    /**
     * 用户手动更改容量触发方法
     * @param capacity 用户输入的容量
     */
    public changeCapacity(capacity) {
        if (capacity !== 0 && !capacity) {
            this.volumeMembers.setCapacity = null;
        } else {
            const capacityNum = capacityChange(Number(capacity), String(this.volumeMembers.getCapacity.unit.id));
            let capacityData = null;
            const chosedDiskData = this.volumeMembers.getCheckArr.filter(item => item.getChecked)[0];
            if (capacity > capacityNum) {
                // 输入值大于最大值时，容量使用原始数据，且增加“oldValue”属性使下发参数为null
                capacityData = {
                    value: capacityNum,
                    unit: this.volumeMembers.getCapacity.unit,
                    options: this.volumeMembers.getCapacity.options,
                    batyValue: chosedDiskData.getCapacity,
                    oldValue: chosedDiskData.getCapacity
                };
                setTimeout((() => {
                    this.volumeMembers.setCapacityObj = capacityData;
                }), 10);
            } else {
                capacityData = {
                    value: capacityNum,
                    unit: this.volumeMembers.getCapacity.unit,
                    options: this.volumeMembers.getCapacity.options,
                    batyValue: transCapacity(capacityNum, this.volumeMembers.getCapacity.unit.label)
                };
                this.volumeMembers.setCapacityObj = capacityData;
            }
            if (this.volumePolicy.getTwoCache) {
                this.setMaxChaceCapacity();
            }
        }
    }

    /**
     * 用户更改容量单位触发方法
     * @param unit 容量单位
     */
    public changeUnit(unit: IOptions) {
        this.volumeMembers.setCapacityObj = {
            value: unitChange(String(unit.id)),
            unit,
            options: this.volumeMembers.getCapacity.options,
            batyValue: this.volumeMembers.getCapacity.batyValue,
            oldValue: this.volumeMembers.getCapacity.batyValue
        };
    }

    /**
     * 可用容量选中改变
     * @param availableSelect 用户选中的可用容量
     */
    public availableChange(availableSelect: AvailableCapacity) {
        this.volumeMembers.setCapacityObj = {
            value: availableSelect.getValue,
            unit: availableSelect.getUnit,
            options: this.volumeMembers.getCapacity.options,
            batyValue: availableSelect.getBatyValue
        };
    }

    /**
     * raid级别选中项改变
     * @param raidLevel 用户选中的raid级别
     */
    public raidLevelChange(raidLevel: IOptions) {
        if (this.volumeMembers.createRelated) {
            this.volumeMembers.setRAIDLevelSelect = raidLevel;
            const maxRange = this.volumeMembers.getSelectData.drives.length / 2;
            if (raidLevel.label === '50') {
                this.spanValid.enable();
                this.validation = this.setValidaTip(3, maxRange);
                if (this.volumeMembers.getSelectData.spanNumber < 3) {
                    this.volumeMembers.setSpanNum = 3;
                    this.spanValid.setValue(3);
                }
                this.setSpanValid(this.volumeMembers.getSpanNum, StorageVaild.spanCount(3, maxRange));
                this.volumeMembers.validStatus = this.spanValid.valid;
            } else if (raidLevel.label === '60') {
                this.spanValid.enable();
                this.validation = this.setValidaTip(4, maxRange);
                if (this.volumeMembers.getSelectData.spanNumber < 4) {
                    this.volumeMembers.setSpanNum = 4;
                    this.spanValid.setValue(4);
                }
                this.setSpanValid(this.volumeMembers.getSpanNum, StorageVaild.spanCount(4, maxRange));
                this.volumeMembers.validStatus = this.spanValid.valid;
            }
            this.paramChanged();
        } else {
            this.volumeMembers.setRAIDLevelSelect = raidLevel;
            // 清空物理盘选中项和容量
            const raidType = this.isAriesCard ? RaidType.ARIES : null;
            this.volumeMembers.addCheckArr(this.volumeMembers.getCheckArr, this.volumePolicy.getTwoCache, raidType);
            this.volumeMembers.setCapacity = null;
            changeDriveDisable(false);
            if (raidLevel.label === '60') {
                // （PMC卡，9560-8i和9560-16i两种类型的板卡，1880卡）raid级别为60，成员盘数量最小数量默认为4，其他情况成员盘为3
                const minValue = (this.isPMCCard || this.isSpecials8iBoard || this.isAriesCard) ? 4 : 3;
                this.setSpanValid(this.volumeMembers.getSpanNum, StorageVaild.spanCount(minValue, 32));
                this.validation = this.setValidaTip(minValue);
                this.spanValid.setValue(minValue);
            } else {
                // 更改span成员数校验范围和校验提示词
                this.setSpanValid(this.volumeMembers.getSpanNum, this.volumeMembers.getSpanValid.validator);
                const minRange = this.volumeMembers.getSpanNum.rang.min;
                this.validation = this.setValidaTip(minRange);
            }
            // 只在校验为enabled的时候获取校验状态
            this.volumeMembers.validStatus = this.spanValid.enabled ? this.spanValid.valid : true;
        }
    }

    /**
     * span成员数改变方法
     * @param spanNum 用户输入的span成员数
     */
    public spanChange() {
        this.volumeMembers.setSpanNum = this.spanValid.value ? Number(this.spanValid.value) : null;
        this.volumeMembers.validStatus = this.spanValid.enabled ? this.spanValid.valid : true;
        if (!this.volumeMembers.createRelated) {
            if (this.userInput) {
                // 清空物理盘选中项和容量
                this.volumeMembers.addCheckArr(this.volumeMembers.getCheckArr, this.volumePolicy.getTwoCache);
                this.volumeMembers.setCapacity = null;
                changeDriveDisable(false);
            }
        } else if (this.spanValid.value && this.volumeMembers.createRelated && this.isPMCCard) {
            this.paramChanged();
        }
    }

    private setSpanValid(spanNum, validator) {
        this.spanValid.setValue(spanNum.value);
        this.spanValid.setValidators(validator);
        if (spanNum.disable) {
            this.spanValid.disable();
        } else {
            this.spanValid.enable();
        }
    }

    private setValidaTip(minRange: number = 3, maxRange: number = 32): TiValidationConfig {
        return {
            tip: formatEntry(this.i18n.instant('STORE_INTEGER3TO32'), [minRange, maxRange]),
            tipMaxWidth: '700px',
            tipPosition: 'top-left',
            errorMessage: {
                pattern: this.i18n.instant('VALIDATOR_INPUT_ERROR')
            }
        };

    }

    public isUserInput(userInput: boolean) {
        this.userInput = userInput;
    }
}
