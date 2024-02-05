import { Component, OnInit } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiModalRef, TiModalService } from '@cloud/tiny3';
import { FansService } from './services/fans.service';
import { FansInfo } from './model/fans.model';
import { CoolingModeSettingComponent } from './component/cooling-mode-setting/cooling-mode-setting.component';
import { getMessageId } from 'src/app/common-app/utils';
import {
    LoadingService,
    ErrortipService,
    SystemLockService,
    UserInfoService,
    AlertMessageService
} from 'src/app/common-app/service';

@Component({
    selector: 'app-asset',
    templateUrl: './fans.component.html',
    styleUrls: ['./fans.component.scss']
})
export class FansComponent implements OnInit {
    public state: boolean = false;
    public modalInstant: TiModalRef;

    public fansInfo: FansInfo;
    public displayed: TiTableRowData[];
    public srcData: TiTableSrcData;
    public isPrivileges: boolean = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock: boolean = false;
    public smartCoolingModeText: string;
    public fanModal;
    constructor(
        private loading: LoadingService,
        private user: UserInfoService,
        private tiModal: TiModalService,
        private alert: AlertMessageService,
        private lockService: SystemLockService,
        private errorTip: ErrortipService,
        private fanService: FansService
    ) {
        this.displayed = [];
        this.srcData = {
            data: [],
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
    }
    public colsNumber = 1;
    public verticalAlign = 'middle';
    public labelWidth = '88px';
    public columns: TiTableColumns[] = [
        {
            title: 'FAN_NAME',
            width: '25%'
        },
        {
            title: 'EMM_FAN_SPEED_STATUS',
            width: '25%'
        },
        {
            title: 'EMM_FAN_VERSION',
            width: '25%'
        },
        {
            title: 'STATUS',
            width: '25%'
        },
    ];
    public trackByFn(index: string, item: any): string {
        return index;
    }
    // 展开用户详情
    public beforeToggle(row: TiTableRowData): void {
        row.showDetails = !row.showDetails;
    }
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.loading.state.next(true);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
            if (res && this.fanModal) {
                this.fanModal.dismiss();
            }
        });
        this.init();
    }
    public init() {
        this.fanService.factory().subscribe(
            (fansInfo) => {
                this.fansInfo = fansInfo;
                this.smartCoolingModeText = this.fansInfo.smartCoolingModeText;
                this.srcData.data = this.fansInfo.fansList;
                this.loading.state.next(false);
            }
        );
    }
    public coolingModeSet() {
        if (this.isSystemLock) {
            return;
        }
        this.fanModal = this.tiModal.open(CoolingModeSettingComponent, {
            id: 'coolingModeSettingId',
            modalClass: 'fanModal',
            context: {
                coolingModeSetting: this.fansInfo.smartCoolingMode,
                param: null
            },
            close: (modalRef: TiModalRef): void => {
                const param = modalRef.content.instance.param;
                if (param) {
                    this.fanService.intelligentUpdate(param).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        this.init();
                    }, (error) => {
                        const errorMessage = this.errorTip.getErrorMessage(getMessageId(error.error)[0].errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                    });
                }
            },
            dismiss: (): void => {
            }
        });
    }

}
