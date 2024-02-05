import { Component, OnInit, Input, AfterViewInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { VolumeStaticModel, IFromSet, VolumeEditModel, VolumeNode, IVolumnDisabled, StorageTree, RaidNode, Raid, RaidType } from '../../../models';
import { VolumeEditService } from './volume-pmc-edit.service';
import { TiValidationConfig, TiMessageService } from '@cloud/tiny3';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { RaidService } from '../../raid/raid.service';
import { VolumePMCEditModel } from '../../../models/volume/volume-pmc-edit.model';
import { PMCEditModel } from '../../../models/volume/pmc-edit.model';
import { AlertMessageService, LoadingService } from 'src/app/common-app/service';
import { formatEntry, getMessageId, errorTip } from 'src/app/common-app/utils';

@Component({
    selector: 'app-volumn-pmc-edit',
    templateUrl: './volumn-pmc-edit.component.html',
    styleUrls: ['./volumn-pmc-edit.component.scss'],
})
export class VolumnPMCEditComponent implements OnInit, AfterViewInit {

    @Input() volumeNode: VolumeNode;
    @Input() cardType: string;
    @Input() isCache: boolean;
    @Input() pmcEditData: PMCEditModel;
    public volumePMCEditModel: VolumePMCEditModel;
    public buttonState: boolean;
    public volumnDisable: IVolumnDisabled;
    public volumeStaticModel = VolumeStaticModel;
    public maxCacheCount = VolumeStaticModel.logicDriverList.maxCacheCount;
    public isMaxCacheMethod = false;

    public formSet: IFromSet = {
        lineHeight: '22px',
        fieldVerticalAlign: 'middle',
        formType: 'type'
    };

    public validation: TiValidationConfig = {
        tip: this.i18n.instant('STORE_ASSIC15', {param: 64}),
        tipMaxWidth: '700px',
        tipPosition: 'top-left',
        errorMessage: {
            pattern: this.i18n.instant('COMMON_FORMAT_ERROR')
        }
    };
    public capacityValidation: TiValidationConfig = {
        tip: '',
        tipMaxWidth: '700px',
        tipPosition: 'top-left',
        errorMessage: {
            required: '',
            pattern: ''
        }
    };

    constructor(
        private i18n: TranslateService,
        private volumeEditService: VolumeEditService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private loading: LoadingService,
        private raidService: RaidService
    ) {
        this.volumePMCEditModel = VolumePMCEditModel.getInstance();
    }
    ngAfterViewInit(): void {
        VolumePMCEditModel.getInstance().setOriginData = JSON.parse(JSON.stringify(VolumePMCEditModel.getInstance().getSelectData));
    }

    ngOnInit() {
        const raidNode: RaidNode = StorageTree.getInstance().getParentRaid(this.volumeNode.getRaidIndex);
        let post$ = of('').pipe(map(() => raidNode.getRaid));
        if (!raidNode.getRaid) {
            post$ = this.raidService.factory(raidNode);
        }
        post$.subscribe(
            (raid: Raid) => {
                raidNode.setRaid = raid;
                VolumePMCEditModel.initVolumePMCEdit(
                    this.pmcEditData.getName,
                    this.pmcEditData.getBootPriority,
                    this.pmcEditData.getStripeSizeBytes,
                    this.pmcEditData.getSpeedMethod,
                    this.pmcEditData.getCapacity,
                    this.pmcEditData.getCapacityUnit,
                    this.pmcEditData.getPMCWritePolicy
                );
                this.volumePMCEditModel = VolumePMCEditModel.getInstance();
                this.isMaxCacheMethod = this.volumePMCEditModel.getSelectData.accelerationMethod.id === 'maxCache';
            }
        );
        if (this.pmcEditData.getModifyingCapacity) {
            // PMC卡中逻辑盘当前容量小于最大容量时，提示容量取值范围
            this.capacityValidation.tip = formatEntry(this.i18n.instant('VALID_VALUE_RANGE_TIP'),
                [this.pmcEditData.getCapacity + String(this.pmcEditData.getCapacityUnit.id),
                this.pmcEditData.getMaxCapacity + String(this.pmcEditData.getMaxCapacityUnit.id)]);
        }
        // 当PMC卡中有maxCache逻辑盘时，普通逻辑盘和maxCache逻辑盘都禁止设置容量, 普通逻辑盘禁止设置条带大小；PMC卡中逻辑盘当前容量不小于最大容量时禁止设置容量
        if (this.maxCacheCount > 0 || !this.pmcEditData.getModifyingCapacity) {
            this.volumePMCEditModel.getCapacityFormControl.disable();
        } else {
            this.volumePMCEditModel.getCapacityFormControl.enable();
        }
    }

    /**
     * 模板中实际调用的是Modal服务提供的close和dismiss方法，并非此处定义的方法；
     * 在此处定义close和dismiss方法只是为了避免生产环境打包时报错
     */
    close(): void {
    }
    dismiss(): void {
    }

    save(): void {
        const msgInstance = this.tiMessage.open({
            id: 'driverCreatMessage',
            type: 'prompt',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('COMMON_ASK_OK'),
            okButton: {
                text: this.i18n.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    msgInstance.close();
                    this.loading.state.next(true);
                    this.volumeEditService.updatePMCEditData().subscribe(
                        () => {
                            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            this.loading.state.next(false);
                            this.close();
                        },
                        (error) => {
                            this.errorTip(error);
                            this.close();
                        }
                    );
                }
            },
            cancelButton: {
                text: this.i18n.instant('COMMON_CANCEL')
            }
        });
    }

    private errorTip(error: any): void {
        const msgId = error.error ? getMessageId(error.error)[0].errorId : 'COMMON_FAILED';
        this.alert.eventEmit.emit(errorTip(msgId, this.i18n));
        this.loading.state.next(false);
    }

    public getSpeedOption (type: string) {
        let resultOption = [];
        if (type === RaidType.HBA) {
          resultOption = VolumeStaticModel.speedMethod.filter( item => item.id !== 'ControllerCache');
        } else {
          resultOption = VolumeStaticModel.speedMethod;
        }
        return resultOption;
    }

}
