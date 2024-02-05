import { Component, OnInit } from '@angular/core';
import { TiModalService, TiModalRef } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { VolumesService } from './volumes.service';
import { StorageTree, Volume, VolumeNode, VolumeEditModel, RaidType } from '../../models';
import { VolumnEditComponent } from './volumn-edit/volumn-edit.component';
import { LoadingService, UserInfoService, SystemLockService } from 'src/app/common-app/service';
import { PMCEditModel } from '../../models/volume/pmc-edit.model';
import { VolumnAriesEditComponent } from './volumn-aries-edit/volumn-aries-edit.component';
import { VolumnPMCEditComponent } from './volumn-pmc-edit/volumn-pmc-edit.component';
import { StorageCollectLogService } from 'src/app/common-app/service/storage-collectlog.service';
import { VolumnSPEditComponent } from './volumn-sp-edit/volumn-sp-edit.component';

@Component({
    selector: 'app-volumes',
    templateUrl: './volumes.component.html',
    styleUrls: ['./volumes.component.scss']
})
export class VolumesComponent implements OnInit {

    public volumeNode: VolumeNode;
    public pmcEditData: PMCEditModel = null;
    public systemLocked: boolean = true;
    public isSetting: boolean;
    public isCache: boolean = false;
    public startCollectingLogs: boolean = false;

    constructor(
        private volumesService: VolumesService,
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
        this.volumeNode = StorageTree.getInstance().getCheckedNode as VolumeNode;
        this.factory();
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe((res) => {
            this.systemLocked = res;
        });
        this.isSetting = this.settingShow();
        const { labelId, id } = this.volumeNode;
        const raidId = labelId?.replace('_' + id, '');
        this.startCollectingLogs = this.storageCollectLogService.getCollectedState(raidId) || false;
        this.storageCollectLogService.changeState.subscribe((res) => {
            if (res.key === raidId) {
                this.startCollectingLogs = res.value;
            }
        });
    }
    private factory() {
        this.loading.state.next(true);
        this.volumesService.factory(this.volumeNode).subscribe(
            (node: {volume: Volume, isCache: boolean, pmcEditModel: PMCEditModel}) => {
                this.volumeNode.setVolume = node.volume;
                this.isCache = node.isCache;
                if (node.pmcEditModel) {
                    this.pmcEditData = node.pmcEditModel;
                }
            },
            () => {},
            () => {
                this.loading.state.next(false);
            }
        );
    }

    show(): void {
        const cardType = this.volumeNode.getRaidType;
        let comp = null;
        if (cardType === RaidType.PMC || cardType === RaidType.HBA) {
            comp = VolumnPMCEditComponent;
        } else if (cardType === RaidType.ARIES) {
            if (this.volumeNode.getSPType) {
                comp = VolumnSPEditComponent;
            } else {
                comp = VolumnAriesEditComponent;
            }
        } else {
            comp = VolumnEditComponent;
        }
        this.tiModal.open(comp, {
            id: 'volumnEdit',
            modalClass: 'volumn-edit-class',
            context: {
                volumeNode: this.volumeNode,
                cardType,
                isCache: this.isCache,
                pmcEditData: this.pmcEditData
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                VolumeEditModel.destroy();
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
        if (this.volumeNode.getRaidType === RaidType.BRCM ||
            this.volumeNode.getRaidType === RaidType.PMC ||
            this.volumeNode.getRaidType === RaidType.ARIES ||
            this.volumeNode.getRaidType === RaidType.HBA
        ) {
            return true;
        }
        return false;
    }

}
