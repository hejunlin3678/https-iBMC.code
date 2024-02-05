import { Component, OnInit, Input, AfterViewInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { VolumeStaticModel, IFromSet, VolumeEditModel, VolumeNode, IVolumnDisabled, StorageTree, RaidNode, Raid, RaidType } from '../../../models';
import { VolumeEditService } from './volume-edit.service';
import { AlertMessageService } from 'src/app/common-app/service';
import { LoadingService } from 'src/app/common-app/service';
import { TiValidationConfig, TiMessageService } from '@cloud/tiny3';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { RaidService } from '../../raid/raid.service';
import { errorTip, getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-volumn-edit',
    templateUrl: './volumn-edit.component.html',
    styleUrls: ['./volumn-edit.component.scss'],
})
export class VolumnEditComponent implements OnInit, AfterViewInit {

    @Input() volumeNode: VolumeNode;
    public volumeEditModel: VolumeEditModel;
    public buttonState: boolean;
    public volumnDisable: IVolumnDisabled;
    public volumeStaticModel = VolumeStaticModel;

    public formSet: IFromSet = {
        lineHeight: '22px',
        fieldVerticalAlign: 'middle',
        formType: 'type'
    };

    public validation: TiValidationConfig = {
        tip: this.i18n.instant('STORE_ASSIC15', {param: 15}),
        tipMaxWidth: '700px',
        tipPosition: 'top-left',
        errorMessage: {
            pattern: this.i18n.instant('COMMON_FORMAT_ERROR')
        }
    };
    // 启动设备置灰控制（博通卡逻辑盘为启动设备时置灰）
    public isBootDiskDisabled: boolean = false;
    constructor(
        private i18n: TranslateService,
        private volumeEditService: VolumeEditService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private loading: LoadingService,
        private raidService: RaidService
    ) {
        this.volumeEditModel = VolumeEditModel.getInstance();
        this.volumnDisable = this.volumeEditModel.getVolumnDisable;
    }
    ngAfterViewInit(): void {
        VolumeEditModel.getInstance().setOriginData = JSON.parse(JSON.stringify(VolumeEditModel.getInstance().getSelectData));
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
                if (raidNode.getRaidType === RaidType.BRCM && this.volumeNode.getVolume.getBootDisk.id) {
                   this.isBootDiskDisabled = true;
                }
                VolumeEditModel.initVolumeEdit(
                    this.volumeNode.getVolume.getName,
                    this.volumeNode.getVolume.getBootDisk,
                    this.volumeNode.getVolume.getSSCDFunc,
                    this.volumeNode.getVolume.getDriveCache,
                    this.volumeNode.getVolume.getAccessPolicy,
                    this.volumeNode.getVolume.getBGIState,
                    this.volumeNode.getVolume.getIOPolicy,
                    this.volumeNode.getVolume.getWritePolicy,
                    this.volumeNode.getVolume.getReadPolicy,
                    this.volumeNode.getVolume.getCacheEnableState,
                    this.volumeNode.getVolume.getTwoCache,
                    this.volumeNode.getRaidIndex
                );
                this.volumeEditModel = VolumeEditModel.getInstance();
                this.volumnDisable = this.volumeEditModel.getVolumnDisable;
            }
        );
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
                    this.volumeEditService.updateEditData().subscribe(
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

}
