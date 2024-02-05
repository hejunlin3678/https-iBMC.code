import { Component, OnInit, Input, OnChanges } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder } from '@angular/forms';
import { NetworkService } from '../../services';
import { AlertMessageService, UserInfoService, ErrortipService, SystemLockService } from 'src/app/common-app/service';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { getMessageId } from 'src/app/common-app/utils';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT } from 'src/app/common-app/service/product.type';

@Component({
    selector: 'app-network-vlan',
    templateUrl: './network-vlan.component.html',
    styleUrls: ['../../network.component.scss', './network-vlan.component.scss']
})
export class NetworkVlanComponent implements OnInit, OnChanges {

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private user: UserInfoService,
        private networkService: NetworkService,
        private errorTipService: ErrortipService,
        private alert: AlertMessageService,
        private lockService: SystemLockService
    ) { }

    @Input() networkVlan;

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;
    public vlanTitle: string = '';

    public validationVlan: TiValidationConfig = {
        tip: this.translate.instant('IBMC_VLAN_TIP'),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('COMMON_FORMAT_ERROR'),
            number: this.translate.instant('COMMON_FORMAT_ERROR'),
            integer: this.translate.instant('COMMON_FORMAT_ERROR'),
            rangeValue: this.translate.instant('IBMC_VLAN_TIP')
        }
    };

    public netVlanForm = this.fb.group({
        vlanEnableState: [{ value: null, disabled: false }, []],
        vlanControl: [
            { value: null, disabled: true },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 4094)]
        ]
    });
    public vlanParams = {};
    public param = { VLAN: {} };
    public vlanDisabled: boolean = true;

    public vlanInit = (networkVlan) => {
        this.vlanParams['vlanEnable'] = networkVlan.getVlanEnable();

        this.netVlanForm.patchValue({
            vlanEnableState: this.vlanParams['vlanEnable'],
        });
        if (this.netVlanForm.root.get('vlanEnableState').value === false && this.isConfigureComponentsUser && !this.isSystemLock) {
            this.netVlanForm.root.get('vlanControl').enable();
            this.vlanParams['vlanId'] = 0;
            this.netVlanForm.patchValue({
                vlanControl: 0,
            });
        } else {
            this.vlanParams['vlanId'] = networkVlan.getVlanId();
            this.netVlanForm.root.get('vlanControl').disable();
            this.netVlanForm.patchValue({
                vlanControl: this.vlanParams['vlanId'],
            });
        }
        if (this.isConfigureComponentsUser && !this.isSystemLock) {
            this.netVlanForm.root.get('vlanEnableState').enable();
        } else {
            this.netVlanForm.root.get('vlanEnableState').disable();
        }
        this.vlanEnabledControl();
    }
    ngOnInit() {
        this.vlanInit(this.networkVlan);
        this.getVlanTitle(CommonData.productType);
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
            }
        });
    }

    public vlanEnabledControl = () => {
        if (this.netVlanForm.root.get('vlanEnableState') && this.netVlanForm.root.get('vlanEnableState').value === true) {
            this.netVlanForm.patchValue({
                vlanControl: this.vlanParams['vlanId'] ? this.vlanParams['vlanId'] : ''
            });
            if (this.isConfigureComponentsUser && !this.isSystemLock) {
                this.netVlanForm.root.get('vlanControl').enable();
            } else {
                this.netVlanForm.root.get('vlanControl').disable();
            }
        } else {
            this.netVlanForm.root.get('vlanControl').disable();
            this.netVlanForm.patchValue({
                vlanControl: 0
            });
        }
        this.netVlanForm.get('vlanControl').updateValueAndValidity();
    }
    public protocolEnabledEN = () => {
        if (!this.netVlanForm.valid) {
            this.vlanDisabled = true;
            return;
        }
        this.param = { VLAN: {} };
        if (this.netVlanForm.root.get('vlanEnableState').value !== this.vlanParams['vlanEnable']) {
            this.param.VLAN['Enabled'] = this.netVlanForm.root.get('vlanEnableState').value;
        }
        if (this.netVlanForm.root.get('vlanControl').value
            && parseInt(this.netVlanForm.root.get('vlanControl').value, 10) !== this.vlanParams['vlanId']) {
            this.param.VLAN['ID'] = parseInt(this.netVlanForm.root.get('vlanControl').value, 10);
        }
        if (JSON.stringify(this.param.VLAN) !== '{}') {
            this.vlanDisabled = false;
        } else {
            this.vlanDisabled = true;
        }
    }

    // change事件
    public vlanEnableIdChange = () => {
        this.vlanEnabledControl();
        this.protocolEnabledEN();
    }
    public vlanChange = () => {
        this.protocolEnabledEN();
    }

    // 保存
    public vlanButton(): void {
        this.vlanDisabled = true;
        this.protocolEnabledEN();
        this.networkService.saveNetWork(JSON.stringify(this.param)).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.vlanDisabled = true;
            this.networkService.networkVlanDetails(this.networkVlan, response['body']);
            this.vlanInit(this.networkVlan);
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
            this.networkService.networkVlanDetails(this.networkVlan, error.error.data);
            this.vlanInit(this.networkVlan);
        });
    }
    public getVlanTitle(res) {
        switch (res) {
            case PRODUCT.IBMC:
                this.vlanTitle = this.translate.instant('IBMC_VLAN');
                break;
            case PRODUCT.IRM:
                this.vlanTitle = this.translate.instant('IRM_VLAN');
                break;
            case PRODUCT.EMM:
                break;
        }
    }
}
