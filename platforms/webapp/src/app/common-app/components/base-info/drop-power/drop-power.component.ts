import { Component, OnInit, Input } from '@angular/core';
import * as utils from 'src/app/common-app/utils';
import { BaseInfoService } from '../base-info.service';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT, PRODUCTTYPE } from 'src/app/common-app/service/product.type';

@Component({
    selector: 'app-drop-power',
    templateUrl: './drop-power.component.html',
    styleUrls: ['./drop-power.component.scss']
})
export class DropPowerComponent implements OnInit {
    @Input() state: string = 'Off';
    @Input() id;
    showDropLang: string = 'none';
    mouseState: string = 'leave';
    // 是否有电源控制权限
    public oemPowerControl = this.user.hasPrivileges(['OemPowerControl']);
    public list = [
        {
            label: 'HEADER_POWER_ON',
            hide: false,
            value: 'On',
            state: 'Off',
            id: 'powerStateOn'
        },
        {
            label: 'POWER_OFF',
            hide: false,
            value: 'GracefulShutdown',
            state: 'On',
            id: 'powerStateShutdown'
        },
        {
            label: 'HEADER_POWER_FORCE_OFF',
            hide: false,
            value: 'ForceOff',
            state: 'On',
            id: 'powerStateOff'
        },
        {
            label: 'HEADER_POWER_FORCE_RESET',
            hide: this.global.productType === PRODUCTTYPE.PANGEA || CommonData.productType === PRODUCT.PANGEA || localStorage.getItem('productType') === PRODUCTTYPE.PANGEA,
            value: 'ForceRestart',
            state: 'On',
            id: 'powerStateRestart'
        },
        {
            label: 'HEADER_POWER_FORECE_OFF_ON',
            hide: false,
            value: 'ForcePowerCycle',
            state: 'On',
            id: 'powerStateCycle'
        }
    ];
    constructor(
        private baseInfoService: BaseInfoService,
        private loading: LoadingService,
        private alertService: AlertMessageService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private user: UserInfoService,
        private global: GlobalDataService
    ) { }

    ngOnInit() {
    }

    public showDropMenu() {
        if (this.oemPowerControl) {
            this.mouseState = 'enter';
            this.showDropLang = 'block';
        }
    }

    public hideDropMenu(ev) {
        this.mouseState = 'leave';
        if (utils.getMouseOutPosition(ev) === 'top') {
            this.showDropLang = 'none';
        }
    }

    public hideMenu(ev) {
        this.mouseState = 'leave';
        if (utils.getMouseOutPosition(ev) === 'bottom') {
            this.showDropLang = 'none';
        }
    }

    public setPowerState(item) {
        let message = '';
        switch (item.value) {
            case 'ForceOff':
                message = this.translate.instant('POWER_STRONG_DOWN_POWER_TIP');
                break;
            case 'ForceRestart':
                message = this.translate.instant('POWER_REST_START_TIP');
                break;
            case 'ForcePowerCycle':
                message = this.translate.instant('POWER_OFF_POWER_ON_TIP');
                break;
            default:
                message = this.translate.instant('COMMON_ASK_OK');
        }
        this.tiMessage.open({
            id: 'prompt',
            content: message,
            type: 'prompt',
            title: this.translate.instant('COMMON_CONFIRM'),
            okButton: {
                autofocus: false
            },
            close: () => {
                const param = {
                    type: item.value
                };
                this.loading.state.next(true);
                this.baseInfoService.setPowerState(param).subscribe({
                    next: () => {
                        this.alertService.eventEmit.emit({
                            type: 'success',
                            label: 'COMMON_SUCCESS'
                        });
                        this.loading.state.next(false);
                        setTimeout(() => {
                            this.global.resendOverview.next(true);
                        }, 2000);
                    },
                });
            }
        });
    }
}
