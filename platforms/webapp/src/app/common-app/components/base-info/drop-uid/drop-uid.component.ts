import { Component, OnInit, Input, ChangeDetectionStrategy } from '@angular/core';
import * as utils from 'src/app/common-app/utils';
import { BaseInfoService } from '../base-info.service';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT, PRODUCTTYPE } from 'src/app/common-app/service/product.type';

@Component({
    selector: 'app-drop-uid',
    templateUrl: './drop-uid.component.html',
    styleUrls: ['./drop-uid.component.scss'],
    changeDetection: ChangeDetectionStrategy.OnPush
})
export class DropUidComponent implements OnInit {
    @Input() state: string = 'Off';
    @Input() id;
    public mouseState: string = 'leave';
    public showDropLang: string = 'none';
    // 是否有常规设置权限
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public list = [
        {
            label: 'HEADER_UID_ON',
            value: 'Lit',
            id: 'uidStateLit',
            hide: false
        },
        {
            label: 'HEADER_UID_BLINK',
            value: 'Blinking',
            id: 'uidStateBlink',
            // localStorage.getItem('productType')为东海机型判断，CommonData.productType为盘古三大洋判断
            hide: localStorage.getItem('productType') === PRODUCTTYPE.PANGEA || CommonData.productType === PRODUCT.PANGEA
        },
        {
            label: 'HEADER_UID_OFF',
            value: 'Off',
            id: 'uidStateOff',
            hide: false
        }
    ];
    constructor(
        private baseInfoService: BaseInfoService,
        private alertService: AlertMessageService,
        private loading: LoadingService,
        private user: UserInfoService,
        private global: GlobalDataService
    ) { }

    ngOnInit() {

    }

    public showDropMenu() {
        if (this.isConfigureComponentsUser) {
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

    // 设置UID状态
    public setUIDState(item) {
        const param = {
            IndicatorLEDState: item.value
        };
        this.loading.state.next(true);
        this.baseInfoService.setUIDState(param).subscribe({
            next: (res) => {
                this.alertService.eventEmit.emit({
                    type: 'success',
                    label: 'COMMON_SUCCESS'
                });
                this.loading.state.next(false);
                this.global.resendOverview.next(true);
            }
        });
    }

}
