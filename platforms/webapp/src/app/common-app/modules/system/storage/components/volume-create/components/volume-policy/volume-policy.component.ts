import { Component, OnInit } from '@angular/core';
import { VolumeCreateService } from '../../volume-create.service';
import { TiValidationConfig, TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { RaidNode, VolumePolicyModel, VolumeMembersModel, VolumeStaticModel, RaidType } from '../../../../models';
import { IOptions } from 'src/app/common-app/models';
import { computeCapacity } from '../../../../utils';
import { driveAndCapacityCount, relateFalse } from '../../..';

@Component({
    selector: 'app-volume-policy',
    templateUrl: './volume-policy.component.html',
    styleUrls: ['./volume-policy.component.scss']
})
export class VolumePolicyComponent implements OnInit {

    public volumePolicy: VolumePolicyModel;
    public volumeMembers: VolumeMembersModel;
    public raidNode: RaidNode;
    public isPMCCard: boolean = false;
    public isAriesCard: boolean = false;
    public nameDisabled: boolean = false;
    public availableLogicals: IOptions[];
    public nameMaxlength: number = 15;
    public isSmartHBA: boolean = false;
    public supportCacheCade: boolean = true;

    public validation: TiValidationConfig = {
        tip: this.i18n.instant('STORE_ASSIC15', {param: this.nameMaxlength}),
        tipMaxWidth: '700px',
        tipPosition: 'top-left',
        errorMessage: {
            pattern: this.i18n.instant('COMMON_FORMAT_ERROR')
        }
    };

    constructor(
        private volumeCreateService: VolumeCreateService,
        private i18n: TranslateService,
        private tiMessage: TiMessageService
    ) {
        this.volumePolicy = VolumePolicyModel.getInstance();
        this.volumeMembers = VolumeMembersModel.getInstance();
    }


    ngOnInit(): void {
        this.availableLogicals = this.volumeCreateService.getLogicList();
        this.volumeCreateService.raidNode$.subscribe(
            (raidNode: RaidNode) => {
                this.raidNode = raidNode;
                this.supportCacheCade = this.raidNode.getSupportCacheCade;
                if (this.raidNode.getRaidType === RaidType.PMC || this.raidNode.getRaidType === RaidType.HBA) {
                    if (this.raidNode.getRaidType === RaidType.HBA) {
                        this.isSmartHBA = true;
                    } else {
                        this.isSmartHBA = false;
                    }
                    this.isPMCCard = true;
                    // pmc 支持64位字符
                    this.nameMaxlength = 64;
                    VolumePolicyModel.initInstance(
                        raidNode.getRaid.getChidrenData.maxSize,
                        raidNode.getRaid.getChidrenData.minSize,
                        true,
                        true,
                        true,
                        true,
                        true,
                        false,
                        false,
                        this.raidNode.getRaidType
                    );
                } else {
                    if (this.raidNode.getRaidType === RaidType.ARIES) {
                        this.isAriesCard = true;
                        // 1880卡 支持23位字符
                        this.nameMaxlength = 20;
                    }
                    VolumePolicyModel.initInstance(
                        raidNode.getRaid.getChidrenData.maxSize,
                        raidNode.getRaid.getChidrenData.minSize,
                        !raidNode.getRaid.getChidrenData.readPolicyConfig,
                        !raidNode.getRaid.getChidrenData.writePolicyConfig,
                        !raidNode.getRaid.getChidrenData.IOPolicyConfig,
                        !raidNode.getRaid.getChidrenData.drivePolicyConfig,
                        !raidNode.getRaid.getChidrenData.accessPolicyConfig,
                        false,
                        false,
                        this.raidNode.getRaidType
                    );
                }
                this.validation = {
                    tip: this.i18n.instant('STORE_ASSIC15', {param: this.nameMaxlength}),
                    tipMaxWidth: '700px',
                    tipPosition: 'top-left',
                    errorMessage: {
                        pattern: this.i18n.instant('COMMON_FORMAT_ERROR')
                    }
                };
            }
        );
    }

    public twoCacheChecked(state: boolean) {
        if (this.isPMCCard) {
            this.setCheckBoxItem();
            if (state) {
                this.volumePolicy.getFormControl.disable();
            } else {
                this.volumePolicy.getFormControl.enable();
            }
            VolumePolicyModel.initInstance(
                this.raidNode.getRaid.getChidrenData.maxSize,
                this.raidNode.getRaid.getChidrenData.minSize,
                !this.raidNode.getRaid.getChidrenData.readPolicyConfig,
                !this.raidNode.getRaid.getChidrenData.writePolicyConfig,
                !this.raidNode.getRaid.getChidrenData.IOPolicyConfig,
                !this.raidNode.getRaid.getChidrenData.drivePolicyConfig,
                !this.raidNode.getRaid.getChidrenData.accessPolicyConfig,
                false,
                state,
                this.raidNode.getRaidType,
                this.availableLogicals
            );
        } else {
            VolumePolicyModel.initInstance(
                this.raidNode.getRaid.getChidrenData.maxSize,
                this.raidNode.getRaid.getChidrenData.minSize,
                !this.raidNode.getRaid.getChidrenData.readPolicyConfig,
                !this.raidNode.getRaid.getChidrenData.writePolicyConfig,
                !this.raidNode.getRaid.getChidrenData.IOPolicyConfig,
                !this.raidNode.getRaid.getChidrenData.drivePolicyConfig,
                !this.raidNode.getRaid.getChidrenData.accessPolicyConfig,
                false,
                state
            );
        }
        const raidLevel = this.raidNode.getRaid.getRAIDLevels;
        VolumeMembersModel.initInstance(
            {
                disable: false,
                levels: raidLevel ? raidLevel.value as [] : null,
                towState: state,
                raidType: this.raidNode.getRaidType
            },
            this.raidNode.getRaid.getDriveArr,
            this.raidNode.getRaid.getName
        );
    }

    public initChange(init: IOptions) {
        if (init.id !== 'UnInit') {
            const initInstance = this.tiMessage.open({
                id: 'initChange',
                type: 'prompt',
                title: this.i18n.instant('COMMON_CONFIRM'),
                content: this.i18n.instant('STORE_INIT_CONFIRM'),
                okButton: {
                    text: this.i18n.instant('COMMON_OK'),
                    autofocus: false,
                    click: () => {
                        initInstance.close();
                        if (init.id === 'OPO') {
                            this.setCheckBoxItem(true);
                        } else {
                            this.setCheckBoxItem();
                        }
                    }
                },
                cancelButton: {
                    text: this.i18n.instant('COMMON_CANCEL'),
                    click: () => {
                        this.setCheckBoxItem();
                        this.volumePolicy.selectData.initializationMode =
                            this.isAriesCard ? VolumeStaticModel.initMode[1] : VolumeStaticModel.initMode[0];
                        initInstance.close();
                    }
                }
            });
        } else {
            this.setCheckBoxItem();
        }
    }
     // pmc初始化类型值改变时，设置HDD物理盘可用状态
     public setCheckBoxItem(state: boolean = false) {
        if (!this.isPMCCard) {return; }
        this.raidNode.getRaid.getDriveArr.forEach(item => {
            item.checkDisableState(false);
            item.setOpoInitState = false;
            item.setCheckedAllClear = false;
            if (item.getMediaType === 'HDD' && state) {
                item.setOpoInitState = state;
                item.setDisable = item.getIsDisable ? item.getIsDisable : state;
            }
        });
    }

    public slectedAssociationLogical(event) {
        driveAndCapacityCount();
        const currentCapacity = this.volumeMembers.getSelectData.capacityMBytes;
        const logicalMaxCapacity = event.maxCapacity;
        let allowedMaxCapacity = 0;
        if (this.volumePolicy.getselectData.cacheLineSize.id === '64K') {
            allowedMaxCapacity = 1.7 * 1024 * 1024 * 1024 * 1024;
        } else if (this.volumePolicy.getselectData.cacheLineSize.id === '256K') {
            allowedMaxCapacity = 6.8 * 1024 * 1024 * 1024 * 1024;
        }
        const maxCapacityMBytes = Math.min(allowedMaxCapacity, currentCapacity, logicalMaxCapacity);
        const result = computeCapacity(maxCapacityMBytes);
        this.volumeMembers.setCapacityObj = {
            value: result.capacity,
            unit: result.unit,
            options: this.volumeMembers.getCapacity.options,
            batyValue: maxCapacityMBytes,
            oldValue: maxCapacityMBytes
        };
    }
}
