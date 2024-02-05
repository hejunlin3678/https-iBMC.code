import { Component, OnInit } from '@angular/core';
import { UntypedFormBuilder } from '@angular/forms';
import { USBService } from './services/usb.service';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService, LoadingService, AlertMessageService } from 'src/app/common-app/service';
@Component({
    selector: 'app-usb',
    templateUrl: './usb.component.html',
    styleUrls: ['./usb.component.scss']
})
export class USBComponent implements OnInit {

    constructor(
        private user: UserInfoService,
        private fb: UntypedFormBuilder,
        private usbService: USBService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private alert: AlertMessageService,
        private loading: LoadingService
    ) { }

    public isPrivileges = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;

    public usbParams: object = {};
    public usbData: object = {};
    public buttonDisabled: boolean = true;
    public queryParams: object = {};
    public usbDevicePresence: boolean = false;
    public usbSystemStatus = '';


    public usbForm = this.fb.group({
        enableSwitchState: [{ value: null, disabled: false }, []]
    });


    private init(usbData) {
        this.usbParams['enabbleState'] = usbData.ServiceEnabled;
        this.usbForm.patchValue({
            enableSwitchState: this.usbParams['enabbleState']
        });
        // USB设备在位状态
        this.usbDevicePresence = usbData.USBDevicePresence;
        this.usbSystemStatus = this.usbDevicePresence ? this.translate.instant('IBMC_USB_DETECTED') : this.translate.instant('IBMC_USB_NOTDETECTED');
    }
    ngOnInit(): void {
        // 权限控制
        if (this.isPrivileges && !this.isSystemLock) {
            this.usbForm.root.get('enableSwitchState').enable();
        } else {
            this.usbForm.root.get('enableSwitchState').disable();
        }

        this.usbService.getUSB().subscribe((response) => {
            this.usbData = response['body'];
            this.init(this.usbData);
        }, () => {
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }

    public usbEnabledChange = () => {
        this.protocolEnabledEN();
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        this.queryParams = {};
        if (this.usbForm.root.get('enableSwitchState').value !== this.usbParams['enabbleState']) {
            this.queryParams['ServiceEnabled'] = this.usbForm.root.get('enableSwitchState').value;
        }
        if (JSON.stringify(this.queryParams) !== '{}') {
            this.buttonDisabled = false;
        } else {
            this.buttonDisabled = true;
        }
    }

    public saveButton = () => {
        const instance = this.tiMessage.open({
            id: 'usbButton',
            type: 'prompt',
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    this.usbService.setUSB(JSON.stringify(this.queryParams)).subscribe((response) => {
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        this.usbData = response['body'];
                        this.init(this.usbData);
                    }, (error) => {
                        this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                        this.usbData = error.error.data;
                        this.init(this.usbData);
                    });
                    instance.close();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click: () => {
                    instance.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }

}
