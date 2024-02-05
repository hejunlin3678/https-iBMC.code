import { Component, OnInit, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { DriverEditService } from './driver-edit.service';
import { IFromSet, StorageTree, DriverEditModel, DriverNode, DriveStaticModel, RaidType } from '../../../models';
import { AlertMessageService, LoadingService } from 'src/app/common-app/service';
import { TiMessageService } from '@cloud/tiny3';
import { errorTip, getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-driver-edit',
    templateUrl: './driver-edit.component.html',
    styleUrls: ['./driver-edit.component.scss']
})
export class DriverEditComponent implements OnInit {

    @Input() driverNode: DriverNode;
    public driveEditModel: DriverEditModel;
    public driveStaticModel = DriveStaticModel;
    public isARIESrCard: boolean = false;
    public isOptimal: boolean = false;
    constructor(
        private translate: TranslateService,
        private loading: LoadingService,
        private driverEditService: DriverEditService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService
    ) { }

    public formSet: IFromSet = {
        lineHeight: '22px',
        fieldVerticalAlign: 'middle',
        formType: 'type'
    };
    public showBootDevice: boolean = false;
    public showPatrolASwitch: boolean = false;
    public showFirmwareStatus: boolean = false;
    public showAriesBootDisk: boolean = false;

    ngOnInit() {
        const loginData = this.driverEditService.getLoginOptions();
        const raidNode = StorageTree.getInstance().getParentRaid(this.driverNode.getRaidIndex);
        if (this.driverNode.getRaidType === RaidType.ARIES) {
            this.isARIESrCard = true;
            this.driverNode.parent?.forEach(item => {
                // "巡检开关"显示逻辑：1.父元素有VolumeNode;2.父元素的VolumeNode的raid级别不为RAID0；3.物理盘类型为HDD
                if (item.componentName === 'VolumeNode'  && item.getRaidLevel !== 'RAID0' &&
                    this.driverNode.getDriver.getMediaType === 'HDD') {
                    // 1880卡下的逻辑盘状态不为Optimal时，该逻辑盘下的物理盘的巡检状态置灰
                    this.isOptimal = (item?.getVolume?.status[1] === 'Optimal');
                    this.showPatrolASwitch = true;
                }
                // "固件状态"显示逻辑：父元素为RaidNode;
                if (this.driverNode.parent.length === 1 && item.componentName === 'RaidNode') {
                    this.showFirmwareStatus = true;
                }
            });
        }

        if ((this.driverNode.getRaidType === RaidType.PMC || this.driverNode.getRaidType === RaidType.HBA) &&
            this.driverNode.parent.length === 1 && this.driverNode.parent[0]?.getRaid?.getMode?.id !== 'RAID') {
            // 只有pmc中在raid卡下面的物理盘，且模式不为raid时才显示“启动设备”项
            this.showBootDevice = true;
        }
        if (this.driverNode.getRaidType === RaidType.ARIES && this.driverNode.parent.length === 1) {
            // 1880卡在raid卡下面的逻辑盘才显示“启动设备”
            this.showAriesBootDisk = true;
        }
        const indicator = this.driverNode.getDriver.getIndicator ?
            this.driverNode.getDriver.getIndicator.state as boolean : null;
        // hi1880卡热备盘只支持单选
        const logicSelected = this.isARIESrCard ? loginData.logicSelect[0] : loginData.logicSelect;
        if (this.driverNode.getRaidType === RaidType.ARIES) {
            DriverEditModel.initDriveEdit(
                indicator,
                this.driverNode.getDriver,
                logicSelected,
                loginData.options,
                raidNode.getRaid,
                Boolean(raidNode.getRaid.getJBODState.id),
                this.driverNode.getDriver.getBootDisk,
                this.driverNode.getRaidType,
                this.driverNode.getDriver.getBootPriority,
                this.driverNode.getDriver.getPatrolState
            );
            this.isARIESrCard = true;
        } else {
            DriverEditModel.initDriveEdit(
                indicator,
                this.driverNode.getDriver,
                logicSelected,
                loginData.options,
                raidNode.getRaid,
                Boolean(raidNode.getRaid.getJBODState.id),
                this.driverNode.getDriver.getBootDisk,
                this.driverNode.getRaidType,
                this.driverNode.getDriver.getBootPriority
            );
        }

        this.driveEditModel = DriverEditModel.getInstance();
        DriverEditModel.getInstance().setOriginData = JSON.parse(JSON.stringify(DriverEditModel.getInstance().getSelectData));
        DriverEditModel.getInstance().checkSave();
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
            id: 'driverEditMessage',
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    this.loading.state.next(true);
                    this.driverEditService.updateEditData().subscribe(
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
                    msgInstance.close();
                }
            },
            cancelButton: {
                text: this.translate.instant('COMMON_CANCEL')
            }
        });
    }
    private errorTip(error: any): void {
        const msgId = error.error ? getMessageId(error.error)[0].errorId : 'COMMON_FAILED';
        this.alert.eventEmit.emit(errorTip(msgId, this.translate));
        this.loading.state.next(false);
    }
}
