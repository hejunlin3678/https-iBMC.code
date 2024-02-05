import { Component, Input, OnInit } from '@angular/core';
import { UntypedFormBuilder, FormGroup, Validators } from '@angular/forms';
import { IIPV6Batch } from '../../model';
import { TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { IpV4Vaild } from 'src/app/emm-app/modules/nav/manager/network/model/valid';
import { BmcValidator } from '../../model/bmc.validator';

@Component({
    selector: 'app-ipv6-batch-setting',
    templateUrl: './ipv6-batch-setting.component.html',
    styleUrls: ['./ipv6-batch-setting.component.scss']
})
export class Ipv6BatchSettingComponent implements OnInit {

    public disabled: boolean = true;
    public ipv6Start: string = '';
    public prefix: string = '';
    public gateWay: string = '';

    @Input() startInfo;
    @Input() validateIpv6;

    constructor(
        private fb: UntypedFormBuilder,
        private translate: TranslateService
    ) { }
    public ipv6BatchForm = this.fb.group({
        'ipBatch': [null, []],
        'prefixBatch': [null, []],
        'gateBatch': [null, []]
    });
    public ipv6Keys = {
        adress: 'ipBatch',
        prefix: 'prefixBatch',
        gate: 'gateBatch'
    };
    // IPV6地址校验提示
    public ipV6AdressValidation: TiValidationConfig = {
        type: 'change',
        tip: this.translate.instant('IBMC_IPV6_RANGES'),
        errorMessage: {
            'validateIp6': this.translate.instant('IBMC_INVALID_IPADDRESS_V6'),
            'invalidAddress': this.translate.instant('IBMC_INVALID_IPADDRESS_V6')
        }
    };

    // IPV6默认网关校验提示
    public ipV6GateWayValidation: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            'validateIp6': this.translate.instant('IBMC_INVALID_IPADDRESS_V6')
        }
    };

    // 前缀校验提示
    public ipV6PrefixValidation: TiValidationConfig = {
        type: 'change',
        tip: this.translate.instant('IBMC_IPV6PREFIX_LENGTH'),
        errorMessage: {
            integer: this.translate.instant('IBMC_IPV6PREFIX_LENGTH'),
            pattern: this.translate.instant('IBMC_IPV6PREFIX_LENGTH')
        }
    };

    // IPV6地址校验规则 1. 有效的IPV6地址 2. 转换成大写后不能以FF、FE8、FE9、FEA、FEB开头
    public ipv6AddressValidator = [
        SerAddrValidators.validateIp6()
    ];
    // 网关校验规则，1. 有效的IPV6地址
    public ipv6GatewayValidator = [
        SerAddrValidators.validateIp6()
    ];
    public ipv6PrefixValidator = [
        TiValidators.integer,
        Validators.pattern(/^(?:[0-9]|[0-9][0-9]|1[0-1][0-9]|12[0-8])$/)
    ];

    ngOnInit(): void {
        this.init();
    }
    // 初始化表单控件
    private init() {
        this.ipv6AddressValidator.push(this.validateIpv6('gateBatch', 'prefixBatch'));
        this.ipv6GatewayValidator.push(this.validateIpv6('ipBatch', 'prefixBatch'));
        this.ipv6BatchForm.get('ipBatch').setValidators(this.ipv6AddressValidator);
        this.ipv6BatchForm.get('gateBatch').setValidators(this.ipv6GatewayValidator);
        this.ipv6BatchForm.get('prefixBatch').setValidators(this.ipv6PrefixValidator);
        const controls = this.ipv6BatchForm.controls;
        Object.keys(controls).forEach(key => {
            controls[key].valueChanges.subscribe({
                next: (value) => {
                    this.computedFormState();
                }
            });
        });
    }
    public emitData() {
        const startInfo: IIPV6Batch = {
            ipAddress: this.ipv6BatchForm.controls.ipBatch.value,
            prefix: this.ipv6BatchForm.controls.prefixBatch.value,
            gateWay: this.ipv6BatchForm.controls.gateBatch.value
        };
        this.startInfo(startInfo);
        this.close();
    }

    // close和dismiss2个方法必须得写，保持空函数就可以，否则，弹出框将无法被关闭
    public close(): void {

    }

    public dismiss(): void {

    }
    public computedFormState(): void {
        const controls = this.ipv6BatchForm.controls;
        const ipv6 = controls.ipBatch.value;
        const prefix = controls.prefixBatch.value;
        const gateWay = controls.gateBatch.value;
        if (!this.ipv6BatchForm.valid) {
            this.disabled = true;
        } else {
            if (!ipv6 || !prefix || !gateWay) {
                this.disabled = true;
            } else {
                this.disabled = false;
            }
        }
    }
    // 当前控件触发 ngModelChange时，对同一fromGroup下的其他控件值做一次变更检测
    public ipChange(ctrlName) {
        const controls = this.ipv6BatchForm.controls;
        Object.keys(controls).forEach(key => {
            if (key !== ctrlName) {
                controls[key].markAllAsTouched();
                controls[key].updateValueAndValidity();
            }
        });
    }
}
