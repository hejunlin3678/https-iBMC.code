import { Component, OnInit, OnDestroy } from '@angular/core';
import { Ibma } from './model/ibma';
import { IbmaService } from './services/ibma.service';
import { AlertMessageService, UserInfoService, LoadingService, SystemLockService } from 'src/app/common-app/service';
import { TiModalService } from '@cloud/tiny3';
import { IbmaModalComponent } from './component/ibma-modal/ibma-modal.component';
@Component({
    selector: 'app-ibma',
    templateUrl: './ibma.component.html',
    styleUrls: ['./ibma.component.scss']
})
export class IbmaComponent implements OnInit, OnDestroy {

    constructor(
        private service: IbmaService,
        private alert: AlertMessageService,
        private tiModal: TiModalService,
        private user: UserInfoService,
        private loading: LoadingService,
        private lockService: SystemLockService
    ) { }

    public isPrivileges: boolean = this.user.hasPrivileges(['OemKvm', 'OemVmm']);
    public isSystemLock: boolean = false;
    public ibmaModal;

    public iBMA: Ibma = new Ibma();
    public getiBMAUSBStickTime;
    public versionShow: boolean = false;

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    private openJavaShare() {
        this.service.exportKvmStartupFile().then().catch((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_FAILED' });
        });
    }

    public installNotes() {
        if (this.isSystemLock) {
            return;
        }
        if (this.iBMA.iBMAInserted) {
            this.alert.eventEmit.emit({ type: 'prompt', label: 'IBMA_ALERT1' });
        } else {
            this.loading.state.next(true);
            const param = {
                Type: 'Connect'
            };
            this.service.usbStickControl(JSON.stringify(param)).subscribe((response) => {
                this.init();
                this.loading.state.next(false);
                this.ibmaModal = this.tiModal.open(IbmaModalComponent, {
                    id: 'ibmaModalId',
                    close: (): void => {
                        localStorage.setItem('H5ClientModel', 'Shared');
                        const toURL = `#/kvm_h5`;
                        // 如果获取不到sessionId, 则不发送
                        if (this.user.resourceId) {
                            this.service.refreshActivationState().subscribe(() => {
                                window.open(toURL, '_blank');
                            }, (error) => {
                                this.openJavaShare();
                            });
                        } else {
                            this.openJavaShare();
                        }
                    },
                    dismiss: (): void => { }
                });
            }, (error) => {
                this.alert.eventEmit.emit({ type: 'error', label: 'IBMA_ALERT2' });
                this.loading.state.next(false);
            });
        }
    }

    // ngFor增加trackby，减少界面重新渲染
    public trackByFn(item: any): string {
        return String(item.label);
    }

    private init() {
        this.service.getIBMA().subscribe((response) => {
            const ibmaData = this.service.getIBMAInfo(response['body']);
            this.iBMA.iBMAInstallableShow = ibmaData.installableVersionShow;
            this.iBMA.iBMAInfos = ibmaData.ibmaInfos;
            this.iBMA.iBMAInserted = ibmaData.inserted;
            this.iBMA.iBMAInsertedStr = ibmaData.insertedStr;
            this.iBMA.iBMAVersion = ibmaData.version;
            this.iBMA.iBMADirverVersion = ibmaData.driverVersion;
            this.iBMA.iBMARunStatus = ibmaData.runningStatus;
            this.loading.state.next(false);
        }, (error) => {
            this.loading.state.next(false);
            clearInterval(this.getiBMAUSBStickTime);
        });

    }
    ngOnInit(): void {
        this.loading.state.next(true);
        this.getiBMAUSBStickTime = setInterval(() => { this.init(); }, 3000);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.ibmaModal) {
                this.ibmaModal.dismiss();
            }
        });
        this.init();
    }
    ngOnDestroy(): void {
        if (this.getiBMAUSBStickTime) {
            clearInterval(this.getiBMAUSBStickTime);
        }
    }

}
