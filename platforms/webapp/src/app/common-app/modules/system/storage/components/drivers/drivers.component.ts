import { Component, OnInit } from '@angular/core';
import { DriversService } from './drivers.service';
import { TranslateService } from '@ngx-translate/core';
import { TiModalService, TiModalRef } from '@cloud/tiny3';
import { DriverEditComponent } from './driver-edit/driver-edit.component';
import { StorageTree, DriverNode, Driver, Raid, DriverEditModel } from '../../models';
import { LoadingService, UserInfoService, SystemLockService } from 'src/app/common-app/service';
import { StorageCollectLogService } from 'src/app/common-app/service/storage-collectlog.service';

@Component({
    selector: 'app-drivers',
    templateUrl: './drivers.component.html',
    styleUrls: ['./drivers.component.scss']
})
export class DriversComponent implements OnInit {

    public driver: Driver;
    public driverNode: DriverNode;
    public raid: Raid;
    public isSetting: boolean;
    public systemLocked: boolean = true;
    public startCollectingLogs: boolean = false;

    constructor(
        private driversService: DriversService,
        private loading: LoadingService,
        private tiModal: TiModalService,
        private i18n: TranslateService,
        public user: UserInfoService,
        private lockService: SystemLockService,
        private storageCollectLogService: StorageCollectLogService
    ) {
        this.isSetting = false;
    }

    ngOnInit() {
        this.driverNode = StorageTree.getInstance().getCheckedNode as DriverNode;
        if (this.driverNode.getDriver) {
            this.driver = this.driverNode.getDriver;
            this.getRaid();
            this.isSetting = this.settingShow();
        } else {
            this.factory();
        }
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
        const labelId = this.driverNode.labelId;
        const str = labelId?.split('_')[0];
        this.startCollectingLogs = this.storageCollectLogService.getCollectedState(str) || false;
        this.storageCollectLogService.changeState.subscribe((res: any) => {
            if (str === res.key) {
                this.startCollectingLogs = res.value;
            }
        });
    }

    private factory() {
        this.loading.state.next(true);
        this.driversService.factory(this.driverNode).subscribe(
            (driver: Driver) => {
                this.driverNode.setDriver = driver;
                this.driver = driver;
                this.getRaid();
                this.isSetting = this.settingShow();
            },
            () => {},
            () => {
                this.loading.state.next(false);
            }
        );
    }

    show(): void {
        this.tiModal.open(DriverEditComponent, {
            id: 'driverEdit',
            modalClass: 'driver-edit-class',
            context: {
                driverNode: this.driverNode,
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                DriverEditModel.destroy();
                modalRef.destroy(false);
                if (reason) {
                    this.factory();
                }
            },
            close: (modalRef: TiModalRef): void => {
            },
            dismiss: (modalRef: TiModalRef): void => {
            }
        });
    }

    private settingShow(): boolean {
        if (
            this.driverNode && this.driverNode.getIsRaid &&
            this.driver.getFirmwareStatus && this.raid.getOOBSupport
        ) {
            return true;
        }
        return false;
    }

    private getRaid() {
        if (this.driverNode.getRaidIndex === 0 || this.driverNode.getRaidIndex) {
            this.raid = StorageTree.getInstance().getParentRaid(this.driverNode.getRaidIndex).getRaid;
        }
    }

}
