import { Component, OnChanges, OnInit, Input, ElementRef, Output, EventEmitter } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder, Validators, UntypedFormControl } from '@angular/forms';
import { NetworkService } from '../../services';
import { AlertMessageService, UserInfoService, ErrortipService, SystemLockService } from 'src/app/common-app/service';
import { TiValidators, TiValidationConfig, TiMessageService } from '@cloud/tiny3';
import { IpV4Vaild, IPInfo } from '../../model';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-network-protocol',
    templateUrl: './network-protocol.component.html',
    styleUrls: ['../../network.component.scss', './network-protocol.component.scss']
})
export class NetworkProtocolComponent implements OnInit, OnChanges {

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private elRef: ElementRef,
        private user: UserInfoService,
        private networkService: NetworkService,
        private alert: AlertMessageService,
        private errorTipService: ErrortipService,
        private tiMessage: TiMessageService,
        private lockService: SystemLockService
    ) { }

    @Input() networkProtocol;
    @Input() isSystemLock;
    @Input() protocolBeingEdited;
    @Input() protocolEditDisabled;
    @Output() private outer = new EventEmitter();

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public iPAddressMore = true;

    public netModelV4Active: string;
    public netModelV6Active: string;
    public standbyStatus: boolean = true;

    public protocolDisabled: boolean = true;
    public networkProtocolList = [
        {
            id: 'IPv4',
            key: 'IPv4',
            disable: false
        }, {
            id: 'IPv6',
            key: 'IPv6',
            disable: false
        }, {
            id: 'IPv4AndIPv6',
            key: 'IPv4/IPv6',
            disable: false
        }
    ];
    public networkModelV4 = [
        {
            id: 'DHCP',
            key: this.translate.instant('IBMC_IP_ADDRESS_AUTOMATICALLY'),
            disable: false
        },
        {
            id: 'Static',
            key: this.translate.instant('IBMC_IP_CONFIGURATION'),
            disable: false
        }
    ];
    public networkModelV6 = [
        {
            id: 'DHCP',
            key: this.translate.instant('IBMC_IP_ADDRESS_AUTOMATICALLY'),
            disable: false
        },
        {
            id: 'Static',
            key: this.translate.instant('IBMC_IP_CONFIGURATION'),
            disable: false
        }
    ];
    public iPAddresslabel = [];
    public isclickMoreBtn: boolean = false;

    public protocalParams = {};
    public queryParams = {
        IPinfo: {
            IPv4: {
                Active: [{}],
                Standby: [{}],
                Float: [{}]
            } as IPInfo,
            IPv6: {
                Active: [{}],
                Standby: [{}],
                Float: [{}]
            } as IPInfo
        }
    };
    public ipv4Flag: boolean = true;
    public ipv6Flag: boolean = false;
    public iPAddressShow: boolean = false;
    public addressMacV4: string = '';
    public addressValueV6: string = '';
    public ipVaild = new IpV4Vaild(this.networkService);
    public netProtocolForm = this.fb.group({
        selectedNetProtocol: [null, []],
        selectedModelV4: [null, []],
        ipAddressControlV4: [
            { value: null, disabled: true }
        ],
        maskCodeControlV4: [
            { value: null, disabled: true }
        ],
        defaulGatewayControlV4: [
            { value: null, disabled: true }
        ],
        // 备用ipv4地址
        ipAddressControlV4Standby: [
            { value: null, disabled: false },
            [
                Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
                IpV4Vaild.firstIP(),
                this.ipVaild.testIpSame('ipAddressControlV4'),
                this.ipVaild.testIpSame('ipAddressControlV4Float'),
                this.ipVaild.requiredFun('maskCodeControlV4Standby')
            ]
        ],
        // 备用ipv4掩码
        maskCodeControlV4Standby: [
            { value: null, disabled: false },
            [
                Validators.pattern(/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/),
                IpV4Vaild.filtrationIP(),
                this.ipVaild.requiredFun('ipAddressControlV4Standby')
            ]
        ],
        // 浮动ipv4地址
        ipAddressControlV4Float: [
            { value: null, disabled: false },
            [
                Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
                IpV4Vaild.firstIP(),
                this.ipVaild.testIpSame('ipAddressControlV4'),
                this.ipVaild.testIpSame('ipAddressControlV4Standby'),
                this.ipVaild.requiredFun('maskCodeControlV4Float')
            ]
        ],
        // 浮动ipv4掩码
        maskCodeControlV4Float: [
            { value: null, disabled: false },
            [
                Validators.pattern(/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/),
                IpV4Vaild.filtrationIP(),
                this.ipVaild.requiredFun('ipAddressControlV4Float')
            ]
        ],
        selectedModelV6: [null, []],
        // 主用ipv6地址
        ipAddressControlV6: [{ value: null, disabled: true }, []],
        // 主用ipv6前缀
        maskCodeBlurControl: [{ value: null, disabled: true }, []],
        // 主用ipv6网关
        gatewayControlV6: [{ value: null, disabled: true }, []],
        // 备用ipv6地址
        ipAddressControlV6Standby: [
            { value: null, disabled: false },
            [
                SerAddrValidators.validateIp6(),
                this.ipVaild.validateIpv6('gatewayControlV6', 'maskCodeBlurControlStandby'),
                this.ipVaild.testIpSame('ipAddressControlV6'),
                this.ipVaild.testIpSame('ipAddressControlV6Float'),
                this.ipVaild.requiredFun('maskCodeBlurControlStandby')
            ]
        ],
        // 备用ipv6前缀
        maskCodeBlurControlStandby: [
            { value: null, disabled: false },
            [
                Validators.pattern(/^(?:[0-9]|[0-9][0-9]|1[0-1][0-9]|12[0-8])$/),
                this.ipVaild.requiredFun('ipAddressControlV6Standby')
            ]
        ],
        // 浮动ipv6地址
        ipAddressControlV6Float: [
            { value: null, disabled: false },
            [
                SerAddrValidators.validateIp6(),
                this.ipVaild.validateIpv6('gatewayControlV6', 'maskCodeBlurControlFloat'),
                this.ipVaild.testIpSame('ipAddressControlV6'),
                this.ipVaild.testIpSame('ipAddressControlV6Standby'),
                this.ipVaild.requiredFun('maskCodeBlurControlFloat')
            ]
        ],
        // 浮动ipv6前缀
        maskCodeBlurControlFloat: [
            { value: null, disabled: false },
            [
                Validators.pattern(/^(?:[0-9]|[0-9][0-9]|1[0-1][0-9]|12[0-8])$/),
                this.ipVaild.requiredFun('ipAddressControlV6Float')
            ]
        ],
    });

    // IPv4校验
    validationAddressV4: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_ENTER_CORRECT_IP'),
            firstIP: this.translate.instant('IBMC_NET_ERROR_IP'),
            isEqualAddress: this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };

    // IPv4掩码
    validationMaskCodeV4: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_SUB_NET_MASK'),
            filtrationIP: this.translate.instant('IBMC_SUB_NET_MASK')
        }
    };

    // IPv4默认网关
    validationDefaulGateway: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            isEqualAddress: this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };

    // IPv6校验
    validationAddressV6: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            maskCode: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            validateIp6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };

    // IPv6前缀长度
    validationMaskCodeBlur: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            maskCode: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_PREFIX_CHECK')
        }
    };

    // 默认网关
    validationGatewayV6: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            gateway: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            validateIp6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };

    // 备用IPv4校验
    validationAddressV4Standby: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_ENTER_CORRECT_IP'),
            firstIP: this.translate.instant('IBMC_NET_ERROR_IP'),
            isEqualAddress: this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };
    // 备用IPv4掩码
    validationMaskCodeV4Standby: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_SUB_NET_MASK'),
            filtrationIP: this.translate.instant('IBMC_SUB_NET_MASK')
        }
    };
    // 备用IPv6校验
    validationAddressV6Standby: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            maskCode: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            validateIp6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
        }
    };

    // 备用IPv6前缀长度
    validationMaskCodeBluStandbyr: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            maskCode: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_PREFIX_CHECK')
        }
    };
    // 浮动IPv4校验
    validationAddressV4Float: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_ENTER_CORRECT_IP'),
            firstIP: this.translate.instant('IBMC_NET_ERROR_IP'),
            isEqualAddress: this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };
    // 浮动IPv4掩码
    validationMaskCodeV4Float: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_SUB_NET_MASK'),
            filtrationIP: this.translate.instant('IBMC_SUB_NET_MASK')
        }
    };
    // 浮动IPv6校验
    validationAddressV6Float: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            maskCode: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            validateIp6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        }
    };
    // 浮动IPv6前缀长度
    validationMaskCodeBlurFloat: TiValidationConfig = {
        type: 'blur',
        errorMessage: {
            maskCode: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
            pattern: this.translate.instant('IBMC_PREFIX_CHECK')
        }
    };

    public switchDisableV4 = (switchFlag: boolean) => {
        if (switchFlag) {
            // 清除ipv4校验规
            this.netProtocolForm.get('ipAddressControlV4').clearValidators();
            this.netProtocolForm.get('maskCodeControlV4').clearValidators();
            this.netProtocolForm.get('defaulGatewayControlV4').clearValidators();
            TiValidators.check(this.netProtocolForm);
        } else {
            TiValidators.check(this.netProtocolForm);
        }
    }
    public switchDisableV6 = (switchFlag: boolean) => {
        if (switchFlag) {
            // 清除ipv6校验规
            this.netProtocolForm.get('ipAddressControlV6').clearValidators();
            this.netProtocolForm.get('maskCodeBlurControl').clearValidators();
            this.netProtocolForm.get('gatewayControlV6').clearValidators();
            TiValidators.check(this.netProtocolForm);
        } else {
            TiValidators.check(this.netProtocolForm);
        }
    }
    public reValidatorsV4 = () => {
        // 还原ipv4校验规
        this.netProtocolForm.get('ipAddressControlV4').setValidators(
            [
                TiValidators.required,
                Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
                IpV4Vaild.firstIP(),
                IpV4Vaild.isEqualAddress('defaulGatewayControlV4', 'maskCodeControlV4'),
                this.ipVaild.testIpSame('ipAddressControlV4Standby'),
                this.ipVaild.testIpSame('ipAddressControlV4Float')
            ]
        );
        this.netProtocolForm.get('maskCodeControlV4').setValidators(
            [
                TiValidators.required,
                Validators.pattern(/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/),
                IpV4Vaild.filtrationIP()
            ]
        );
        this.netProtocolForm.get('defaulGatewayControlV4').setValidators(
            [
                TiValidators.required,
                IpV4Vaild.isEqualAddress('ipAddressControlV4', 'maskCodeControlV4')
            ]
        );
    }
    public reValidatorsV6 = () => {
        // 还原ipv6校验规
        this.netProtocolForm.get('ipAddressControlV6').setValidators(
            [
                IpV4Vaild.maskCode(this.protocalParams['netModelV6'], this.protocalParams['ipAddressV6'], 'maskCodeBlurControl', 'selectedNetProtocol'),
                SerAddrValidators.validateIp6(),
                this.ipVaild.validateIpv6('gatewayControlV6', 'maskCodeBlurControl'),
                this.ipVaild.testIpSame('ipAddressControlV6Standby'),
                this.ipVaild.testIpSame('ipAddressControlV6Float')
            ]
        );
        this.netProtocolForm.get('maskCodeBlurControl').setValidators(
            [
                IpV4Vaild.maskCode(this.protocalParams['netModelV6'], this.protocalParams['maskCodeV6'], 'ipAddressControlV6', 'selectedNetProtocol'),
                Validators.pattern(/^(?:[0-9]|[0-9][0-9]|1[0-1][0-9]|12[0-8])$/)
            ]
        );
        this.netProtocolForm.get('gatewayControlV6').setValidators(
            [
                IpV4Vaild.gateway(this.protocalParams['netModelV6'], this.protocalParams['gatewayV6'], 'selectedNetProtocol'),
                SerAddrValidators.validateIp6(),
                this.ipVaild.gatewayIpv6('ipAddressControlV6', 'maskCodeBlurControl')
            ]
        );
    }
    public disableV4 = () => {
        this.netProtocolForm.get('ipAddressControlV4').disable();
        this.netProtocolForm.get('maskCodeControlV4').disable();
        this.netProtocolForm.get('defaulGatewayControlV4').disable();
        this.netProtocolForm.get('ipAddressControlV4Standby').disable();
        this.netProtocolForm.get('maskCodeControlV4Standby').disable();
        this.netProtocolForm.get('ipAddressControlV4Float').disable();
        this.netProtocolForm.get('maskCodeControlV4Float').disable();
    }
    public disableV6 = () => {
        this.netProtocolForm.get('ipAddressControlV6').disable();
        this.netProtocolForm.get('maskCodeBlurControl').disable();
        this.netProtocolForm.get('gatewayControlV6').disable();
        this.netProtocolForm.get('ipAddressControlV6Standby').disable();
        this.netProtocolForm.get('maskCodeBlurControlStandby').disable();
        this.netProtocolForm.get('ipAddressControlV6Float').disable();
        this.netProtocolForm.get('maskCodeBlurControlFloat').disable();
    }
    public enableV4 = () => {
        this.netProtocolForm.get('ipAddressControlV4').enable();
        this.netProtocolForm.get('maskCodeControlV4').enable();
        this.netProtocolForm.get('defaulGatewayControlV4').enable();
        this.netProtocolForm.get('ipAddressControlV4Standby').enable();
        this.netProtocolForm.get('maskCodeControlV4Standby').enable();
        this.netProtocolForm.get('ipAddressControlV4Float').enable();
        this.netProtocolForm.get('maskCodeControlV4Float').enable();
    }
    public enableV6 = () => {
        this.netProtocolForm.get('ipAddressControlV6').enable();
        this.netProtocolForm.get('maskCodeBlurControl').enable();
        this.netProtocolForm.get('gatewayControlV6').enable();
        this.netProtocolForm.get('ipAddressControlV6Standby').enable();
        this.netProtocolForm.get('maskCodeBlurControlStandby').enable();
        this.netProtocolForm.get('ipAddressControlV6Float').enable();
        this.netProtocolForm.get('maskCodeBlurControlFloat').enable();
    }

    // IpV4检验变化
    public ipValidationV4 = () => {
        if (this.netProtocolForm.root.get('selectedModelV4').value === 'DHCP') {
            this.netProtocolForm.patchValue({
                ipAddressControlV4: this.protocalParams['ipAddressV4'],
                maskCodeControlV4: this.protocalParams['maskCodeV4'],
                defaulGatewayControlV4: this.protocalParams['gatewayV4']
            });
            // 输入框禁用
            this.netProtocolForm.root.get('ipAddressControlV4').disable();
            this.netProtocolForm.root.get('maskCodeControlV4').disable();
            this.netProtocolForm.root.get('defaulGatewayControlV4').disable();
            this.netProtocolForm.root.get('ipAddressControlV4Standby').disable();
            this.netProtocolForm.root.get('maskCodeControlV4Standby').disable();
        } else {
            if (this.isConfigureComponentsUser
                && !this.isSystemLock
                && this.netProtocolForm.root.get('selectedNetProtocol').value !== 'IPv6') {
                // 输入框可编辑
                this.netProtocolForm.root.get('ipAddressControlV4').enable();
                this.netProtocolForm.root.get('maskCodeControlV4').enable();
                this.netProtocolForm.root.get('defaulGatewayControlV4').enable();
                this.netProtocolForm.root.get('ipAddressControlV4Standby').enable();
                this.netProtocolForm.root.get('maskCodeControlV4Standby').enable();
            } else {
                this.netProtocolForm.root.get('ipAddressControlV4').disable();
                this.netProtocolForm.root.get('maskCodeControlV4').disable();
                this.netProtocolForm.root.get('defaulGatewayControlV4').disable();
                this.netProtocolForm.root.get('ipAddressControlV4Standby').disable();
                this.netProtocolForm.root.get('maskCodeControlV4Standby').disable();
            }

        }
    }
    // IpV6检验变化
    public ipValidationV6 = () => {
        if (this.netProtocolForm.root.get('selectedModelV6').value === 'DHCP') {
            this.netProtocolForm.patchValue({
                ipAddressControlV6: this.protocalParams['ipAddressV6'],
                maskCodeBlurControl: this.protocalParams['maskCodeV6'],
                gatewayControlV6: this.protocalParams['gatewayV6']
            });
            // 输入框禁用
            this.netProtocolForm.root.get('ipAddressControlV6').disable();
            this.netProtocolForm.root.get('maskCodeBlurControl').disable();
            this.netProtocolForm.root.get('gatewayControlV6').disable();
            this.netProtocolForm.root.get('ipAddressControlV6Standby').disable();
            this.netProtocolForm.root.get('maskCodeBlurControlStandby').disable();
        } else {
            // 输入框可编辑
            if (this.isConfigureComponentsUser
                && !this.isSystemLock
                && this.netProtocolForm.root.get('selectedNetProtocol').value !== 'IPv4') {
                this.netProtocolForm.root.get('ipAddressControlV6').enable();
                this.netProtocolForm.root.get('maskCodeBlurControl').enable();
                this.netProtocolForm.root.get('gatewayControlV6').enable();
                this.netProtocolForm.root.get('ipAddressControlV6Standby').enable();
                this.netProtocolForm.root.get('maskCodeBlurControlStandby').enable();
            } else {
                this.netProtocolForm.root.get('ipAddressControlV6').disable();
                this.netProtocolForm.root.get('maskCodeBlurControl').disable();
                this.netProtocolForm.root.get('gatewayControlV6').disable();
                this.netProtocolForm.root.get('ipAddressControlV6Standby').disable();
                this.netProtocolForm.root.get('maskCodeBlurControlStandby').disable();
            }

        }
    }

    public protocolInit = (networkProtocol) => {
        if (JSON.stringify(networkProtocol) === '{}') {
            return;
        }
        this.netModelV4Active = networkProtocol.getNetModelV4() === 'DHCP' ? 'IBMC_IP_ADDRESS_AUTOMATICALLY' : 'IBMC_IP_CONFIGURATION';
        this.netModelV6Active = networkProtocol.getNetModelV6() === 'DHCP' ? 'IBMC_IP_ADDRESS_AUTOMATICALLY' : 'IBMC_IP_CONFIGURATION';
        this.standbyStatus = networkProtocol.getStandbyStatust();
        this.protocalParams['netProtocol'] = networkProtocol.getNetIPVersion();
        // IPv4初始化
        this.protocalParams['netModelV4'] = networkProtocol.getNetModelV4();
        this.protocalParams['ipAddressV4'] = networkProtocol.getIpAddressV4();
        this.protocalParams['maskCodeV4'] = networkProtocol.getMaskCodeV4();
        this.protocalParams['gatewayV4'] = networkProtocol.getGatewayV4();
        this.addressMacV4 = networkProtocol.getAddMacV4();
        // IPv6初始化
        this.protocalParams['netModelV6'] = networkProtocol.getNetModelV6();
        this.protocalParams['ipAddressV6'] = networkProtocol.getIpAddressV6();
        this.protocalParams['maskCodeV6'] = networkProtocol.getMaskCodeV6();
        this.protocalParams['gatewayV6'] = networkProtocol.getGatewayV6();
        this.addressValueV6 = networkProtocol.getAddressValueV6();
        // IPv6 IP地址段
        this.iPAddresslabel = networkProtocol.getIPAddresslabel();
        this.iPAddressShow = networkProtocol.getIpAddressShow();

        // 备用ip
        this.protocalParams['ipAddressV4Standby'] = networkProtocol.getIpAddressV4Standby();
        this.protocalParams['maskCodeV4Standby'] = networkProtocol.getMaskCodeV4Standby();
        this.protocalParams['ipAddressV6Standby'] = networkProtocol.getIpAddressV6Standby();
        this.protocalParams['prefixV6Standby'] = networkProtocol.getPrefixV6Standby();
        this.protocalParams['addressValueV6Standby'] = networkProtocol.getAddressValueV6Standby();
        this.protocalParams['iPAddresslabelV6Standby'] = networkProtocol.getIPAddresslabelV6Standby();
        // 浮动ip
        this.protocalParams['ipAddressV4Float'] = networkProtocol.getIpAddressV4Float();
        this.protocalParams['maskCodeV4Float'] = networkProtocol.getMaskCodeV4Float();
        this.protocalParams['ipAddressV6Float'] = networkProtocol.getIpAddressV6Float();
        this.protocalParams['prefixV6Float'] = networkProtocol.getPrefixV6Float();
        // 初始化
        this.netProtocolForm.patchValue({
            selectedNetProtocol: this.protocalParams['netProtocol'],
            selectedModelV4: this.protocalParams['netModelV4'],
            ipAddressControlV4: this.protocalParams['ipAddressV4'],
            maskCodeControlV4: this.protocalParams['maskCodeV4'],
            defaulGatewayControlV4: this.protocalParams['gatewayV4'],
            selectedModelV6: this.protocalParams['netModelV6'],
            ipAddressControlV6: this.protocalParams['ipAddressV6'],
            maskCodeBlurControl: this.protocalParams['maskCodeV6'],
            gatewayControlV6: this.protocalParams['gatewayV6'],
            ipAddressControlV4Standby: this.protocalParams['ipAddressV4Standby'],
            maskCodeControlV4Standby: this.protocalParams['maskCodeV4Standby'],
            ipAddressControlV4Float: this.protocalParams['ipAddressV4Float'],
            maskCodeControlV4Float: this.protocalParams['maskCodeV4Float'],
            ipAddressControlV6Standby: this.protocalParams['ipAddressV6Standby'],
            maskCodeBlurControlStandby: this.protocalParams['prefixV6Standby'],
            ipAddressControlV6Float: this.protocalParams['ipAddressV6Float'],
            maskCodeBlurControlFloat: this.protocalParams['prefixV6Float'],
        });
        // 查询输入框是否禁用
        this.ipModelChange();
        this.ipValidationV4();
        this.ipValidationV6();
    }
    ngOnInit(): void {
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
                this.ipValidationV4();
                this.ipValidationV6();
            }
        });
        TiValidators.check(this.netProtocolForm);
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.networkProtocol && changes.networkProtocol.currentValue) {
            this.protocolInit(changes.networkProtocol.currentValue);
        }
    }

    // 模式变化事件
    public ipModelChange = () => {
        if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6') {
            this.ipv4Flag = true;
            this.ipv6Flag = true;
        } else if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4') {
            this.ipv4Flag = true;
            this.ipv6Flag = false;
        } else if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv6') {
            this.ipv4Flag = false;
            this.ipv6Flag = true;
        }
        // 还原IPv4,IPv6数据
        this.netProtocolForm.patchValue({
            selectedModelV4: this.protocalParams['netModelV4'],
            ipAddressControlV4: this.protocalParams['ipAddressV4'],
            maskCodeControlV4: this.protocalParams['maskCodeV4'],
            defaulGatewayControlV4: this.protocalParams['gatewayV4'],
            selectedModelV6: this.protocalParams['netModelV6'],
            ipAddressControlV6: this.protocalParams['ipAddressV6'],
            maskCodeBlurControl: this.protocalParams['maskCodeV6'],
            gatewayControlV6: this.protocalParams['gatewayV6'],
            ipAddressControlV4Standby: this.protocalParams['ipAddressV4Standby'],
            maskCodeControlV4Standby: this.protocalParams['maskCodeV4Standby'],
            ipAddressControlV4Float: this.protocalParams['ipAddressV4Float'],
            maskCodeControlV4Float: this.protocalParams['maskCodeV4Float'],
            ipAddressControlV6Standby: this.protocalParams['ipAddressV6Standby'],
            maskCodeBlurControlStandby: this.protocalParams['prefixV6Standby'],
            ipAddressControlV6Float: this.protocalParams['ipAddressV6Float'],
            maskCodeBlurControlFloat: this.protocalParams['prefixV6Float']
        });
    }

    public protocolEnabledEN = () => {
        if (!this.netProtocolForm.valid) {
            this.protocolDisabled = true;
            return;
        }
        // 校验通过，清除错误的校验信息
        const error = this.elRef.nativeElement.querySelectorAll('.ti3-unifyvalid-tip-error-container');
        error.forEach((item) => {
            if (item.style) {
                item.style.display = 'none';
            }
        });

        this.queryParams = {
            IPinfo: {
                IPv4: {
                    Active: [{}],
                    Standby: [{}],
                    Float: [{}]
                },
                IPv6: {
                    Active: [{}],
                    Standby: [{}],
                    Float: [{}]
                }
            }
        };

        if (
            this.netProtocolForm.root.get('selectedNetProtocol').value
            && this.netProtocolForm.root.get('selectedNetProtocol').value !== this.protocalParams['netProtocol']
        ) {
            this.queryParams.IPinfo['IPVersion'] = this.netProtocolForm.root.get('selectedNetProtocol').value;
        }
        if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6'
            || this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4') {
            // IPv4模式
            if (
                this.netProtocolForm.root.get('selectedModelV4').value !== null
                && this.netProtocolForm.root.get('selectedModelV4').value !== this.protocalParams['netModelV4']
            ) {
                this.queryParams.IPinfo.IPv4.Active[0]['AddressOrigin'] = this.netProtocolForm.root.get('selectedModelV4').value;
            }
            // IPv4手动配置 3个参数一块下发
            if (this.netProtocolForm.root.get('selectedModelV4').value === 'Static') {
                if (
                    (this.netProtocolForm.root.get('ipAddressControlV4').value !== null
                        && this.netProtocolForm.root.get('ipAddressControlV4').value !== this.protocalParams['ipAddressV4'])
                    || (this.netProtocolForm.root.get('maskCodeControlV4').value !== null
                        && this.netProtocolForm.root.get('maskCodeControlV4').value !== this.protocalParams['maskCodeV4'])
                    || (this.netProtocolForm.root.get('defaulGatewayControlV4').value !== null
                        && this.netProtocolForm.root.get('defaulGatewayControlV4').value !== this.protocalParams['gatewayV4'])
                ) {
                    this.queryParams.IPinfo.IPv4.Active[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV4').value;
                    this.queryParams.IPinfo.IPv4.Active[0]['SubnetMask'] = this.netProtocolForm.root.get('maskCodeControlV4').value;
                    this.queryParams.IPinfo.IPv4['DefaultGateway'] = this.netProtocolForm.root.get('defaulGatewayControlV4').value;
                }
            }
        }
        if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6'
            || this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv6') {
            // IPv6模式
            if (
                this.netProtocolForm.root.get('selectedModelV6').value
                && this.netProtocolForm.root.get('selectedModelV6').value !== this.protocalParams['netModelV6']
            ) {
                this.queryParams.IPinfo.IPv6.Active[0]['AddressOrigin'] = this.netProtocolForm.root.get('selectedModelV6').value;
            }
            // IPv6手动配置
            if (this.netProtocolForm.root.get('selectedModelV6').value === 'Static') {
                if (
                    this.netProtocolForm.root.get('ipAddressControlV6').value
                    && this.netProtocolForm.root.get('ipAddressControlV6').value !== this.protocalParams['ipAddressV6']
                ) {
                    this.queryParams.IPinfo.IPv6.Active[0]['Address']
                        = this.netProtocolForm.root.get('ipAddressControlV6').value;
                }
                if (
                    this.netProtocolForm.root.get('maskCodeBlurControl').value !== null
                    && ((this.netProtocolForm.root.get('maskCodeBlurControl').value === ''
                        && this.netProtocolForm.root.get('maskCodeBlurControl').value !== this.protocalParams['maskCodeV6'])
                        || (this.netProtocolForm.root.get('maskCodeBlurControl').value !== ''
                            && parseInt(this.netProtocolForm.root.get('maskCodeBlurControl').value, 10)
                            !== this.protocalParams['maskCodeV6']))
                ) {
                    this.queryParams.IPinfo.IPv6.Active[0]['PrefixLength'] =
                        parseInt(this.netProtocolForm.root.get('maskCodeBlurControl').value, 10);
                }
                if (
                    this.netProtocolForm.root.get('gatewayControlV6').value
                    && this.netProtocolForm.root.get('gatewayControlV6').value !== this.protocalParams['gatewayV6']
                ) {
                    this.queryParams.IPinfo.IPv6['DefaultGateway'] = this.netProtocolForm.root.get('gatewayControlV6').value;
                }
            }
        }
        // 修改备用ip和浮动ip时需要两个参数一起下发，否则接口会报错
        if (this.netProtocolForm.root.get('ipAddressControlV4Standby').value !== this.protocalParams['ipAddressV4Standby']) {
            this.queryParams.IPinfo.IPv4.Standby[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV4Standby').value;
            this.queryParams.IPinfo.IPv4.Standby[0]['SubnetMask'] = this.netProtocolForm.root.get('maskCodeControlV4Standby').value;
        }
        if (this.netProtocolForm.root.get('maskCodeControlV4Standby').value !== this.protocalParams['maskCodeV4Standby']) {
            this.queryParams.IPinfo.IPv4.Standby[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV4Standby').value;
            this.queryParams.IPinfo.IPv4.Standby[0]['SubnetMask'] = this.netProtocolForm.root.get('maskCodeControlV4Standby').value;
        }
        if (this.netProtocolForm.root.get('ipAddressControlV6Standby').value !== this.protocalParams['ipAddressV6Standby']) {
            this.queryParams.IPinfo.IPv6.Standby[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV6Standby').value;
            this.queryParams.IPinfo.IPv6.Standby[0]['PrefixLength']
                = parseInt(this.netProtocolForm.root.get('maskCodeBlurControlStandby').value, 10);
        }
        if (this.netProtocolForm.root.get('maskCodeBlurControlStandby').value !== this.protocalParams['prefixV6Standby']) {
            this.queryParams.IPinfo.IPv6.Standby[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV6Standby').value;
            this.queryParams.IPinfo.IPv6.Standby[0]['PrefixLength']
                = parseInt(this.netProtocolForm.root.get('maskCodeBlurControlStandby').value, 10);
        }
        if (this.netProtocolForm.root.get('ipAddressControlV4Float').value !== this.protocalParams['ipAddressV4Float']) {
            this.queryParams.IPinfo.IPv4.Float[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV4Float').value;
            this.queryParams.IPinfo.IPv4.Float[0]['SubnetMask'] = this.netProtocolForm.root.get('maskCodeControlV4Float').value;
        }
        if (this.netProtocolForm.root.get('maskCodeControlV4Float').value !== this.protocalParams['maskCodeV4Float']) {
            this.queryParams.IPinfo.IPv4.Float[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV4Float').value;
            this.queryParams.IPinfo.IPv4.Float[0]['SubnetMask'] = this.netProtocolForm.root.get('maskCodeControlV4Float').value;
        }
        if (this.netProtocolForm.root.get('ipAddressControlV6Float').value !== this.protocalParams['ipAddressV6Float']) {
            this.queryParams.IPinfo.IPv6.Float[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV6Float').value;
            this.queryParams.IPinfo.IPv6.Float[0]['PrefixLength']
                = parseInt(this.netProtocolForm.root.get('maskCodeBlurControlFloat').value, 10);
        }
        if (this.netProtocolForm.root.get('maskCodeBlurControlFloat').value !== this.protocalParams['prefixV6Float']) {
            this.queryParams.IPinfo.IPv6.Float[0]['Address'] = this.netProtocolForm.root.get('ipAddressControlV6Float').value;
            this.queryParams.IPinfo.IPv6.Float[0]['PrefixLength']
                = parseInt(this.netProtocolForm.root.get('maskCodeBlurControlFloat').value, 10);
        }
        this.deleteNull(this.queryParams.IPinfo.IPv6);
        this.deleteNull(this.queryParams.IPinfo.IPv4);
        this.deleteNull(this.queryParams.IPinfo);
        this.deleteNull(this.queryParams);
        if (JSON.stringify(this.queryParams) !== '{}') {
            this.protocolDisabled = false;
            if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6'
                || this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4') {
                const iPv4 = {
                    Active: [{
                        Address: this.netProtocolForm.root.get('ipAddressControlV4').value,
                        SubnetMask: this.netProtocolForm.root.get('maskCodeControlV4').value,
                        AddressOrigin: this.netProtocolForm.root.get('selectedModelV4').value
                    }]
                };
                if (this.netProtocolForm.root.get('selectedModelV4').value === 'Static' ||
                    (this.protocalParams['netModelV4'] === 'DHCP' && this.queryParams.IPinfo['IPVersion'])) {
                    // 下发IP地址时必须带上掩码一起，否则接口会报错
                    if (this.queryParams.IPinfo.IPv4) {
                        this.queryParams.IPinfo.IPv4['Active'] = iPv4.Active;
                    } else {
                        this.queryParams.IPinfo['IPv4'] = iPv4;
                    }
                }
            }
            if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6'
                || this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv6') {
                if (this.netProtocolForm.root.get('selectedModelV6').value === 'Static' ||
                    (this.protocalParams['netModelV6'] === 'DHCP' && this.queryParams.IPinfo['IPVersion'])) {
                    const iPv6 = {
                        Active: [{
                            Address: this.netProtocolForm.root.get('ipAddressControlV6').value,
                            PrefixLength: parseInt(this.netProtocolForm.root.get('maskCodeBlurControl').value, 10),
                            AddressOrigin: this.netProtocolForm.root.get('selectedModelV6').value
                        }]
                    };
                    if (this.queryParams.IPinfo.IPv6) {
                        this.queryParams.IPinfo.IPv6['Active'] = iPv6.Active;
                    } else {
                        this.queryParams.IPinfo['IPv6'] = iPv6;
                    }
                }
            }
        } else {
            this.protocolDisabled = true;
        }

    }
    public protocolChange = () => {
        if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4') {
            // 还原ipv4校验规则
            this.switchDisableV4(false);
            // 清除ipv6校验规则
            this.switchDisableV6(true);
            this.enableV4();
            this.disableV6();
        } else if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv6') {
            // 清除ipv4校验规则
            this.switchDisableV4(true);
            // 还原ipv6规则
            this.switchDisableV6(false);
            this.disableV4();
            this.enableV6();
        } else {
            this.switchDisableV4(false);
            this.switchDisableV6(false);
            this.enableV4();
            this.enableV6();
        }
        this.ipModelChange();
        this.protocolEnabledEN();
        TiValidators.check(this.netProtocolForm);
    }
    public v4OnFocus = () => {
        this.reValidatorsV4();
    }
    public v6OnFocus = () => {
        this.reValidatorsV6();
    }
    public modelChangeV4 = () => {
        this.ipValidationV4();
        this.protocolEnabledEN();
        TiValidators.check(this.netProtocolForm);
    }
    public ipAddressChangeV4 = () => {
        this.netProtocolForm.get('maskCodeControlV4').updateValueAndValidity();
        this.netProtocolForm.get('defaulGatewayControlV4').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV4Standby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV4Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public maskCodeChangeV4 = () => {
        this.netProtocolForm.get('ipAddressControlV4').updateValueAndValidity();
        this.netProtocolForm.get('defaulGatewayControlV4').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public defaulGatewayChangeV4 = () => {
        this.netProtocolForm.get('ipAddressControlV4').updateValueAndValidity();
        this.netProtocolForm.get('maskCodeControlV4').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV4Standby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV4Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public modelChangeV6 = () => {
        if (!this.netProtocolForm.controls.gatewayControlV6.value) {
            this.netProtocolForm.setControl('gatewayControlV6', new UntypedFormControl('',
                [
                    IpV4Vaild.gateway(this.netProtocolForm.controls.selectedModelV6.value, this.protocalParams['gatewayV6'], 'selectedNetProtocol'),
                    SerAddrValidators.validateIp6(),
                    this.ipVaild.gatewayIpv6('ipAddressControlV6', 'maskCodeBlurControl')
                ]
            ));
        }
        this.netProtocolForm.controls.ipAddressControlV6.markAsTouched();
        this.ipValidationV6();
        this.protocolEnabledEN();
        TiValidators.check(this.netProtocolForm);
    }
    public ipAddressChangeV6 = () => {
        this.netProtocolForm.get('maskCodeBlurControl').updateValueAndValidity();
        this.netProtocolForm.get('gatewayControlV6').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Standby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public maskCodeBlurChange = () => {
        this.netProtocolForm.get('ipAddressControlV6').updateValueAndValidity();
        this.netProtocolForm.get('gatewayControlV6').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Standby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public gatewayChangeV6 = () => {
        this.netProtocolForm.get('ipAddressControlV6').updateValueAndValidity();
        this.netProtocolForm.get('maskCodeBlurControl').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Standby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }

    public expandShutDown = () => {
        this.iPAddressMore = !this.iPAddressMore;
        this.isclickMoreBtn = true;
    }
    public expandHide = () => {
        if (!this.isclickMoreBtn) {
            this.iPAddressMore = true;
        }
        this.isclickMoreBtn = false;
    }
    // 保存
    public protocolButton(): void {
        const instance = this.tiMessage.open({
            id: 'netProtocol',
            type: 'prompt',
            content: this.translate.instant('IBMC_NETWORK_CONFIG_TIP'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    instance.close();
                    this.networkService.saveNetWork(JSON.stringify(this.queryParams)).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        this.networkService.networkProtocolDetails(this.networkProtocol, response['body'].IPinfo);
                        this.protocolInit(this.networkProtocol);
                        this.outer.emit(false);
                    }, (error) => {
                        const errorId = getMessageId(error.error)[0].errorId;
                        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                        this.protocolInit(this.networkProtocol);
                        this.outer.emit(false);
                    });
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click(): void {
                    instance.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }
    /**
     * 过滤掉对象中的空值
     * @param obj 要过滤的对象集合
     * @returns 过滤后的对象
     */
    public deleteNull(obj: any) {
        for (const key in obj) {
            if (
                Object.prototype.hasOwnProperty.call(obj, key)
                && (JSON.stringify(obj[key]) === '{}' || JSON.stringify(obj[key]) === '[{}]')
            ) {
                delete obj[key];
            }
        }
        return obj;
    }
    // 取消编辑
    public protocolEditCancel() {
        this.outer.emit(false);
        this.ipModelChange();
        this.netProtocolForm.patchValue({
            selectedNetProtocol: this.protocalParams['netProtocol']
        });
        this.protocolChange();
    }
    // 编辑
    public protocolEdit() {
        this.outer.emit(true);
    }
    // ip保存
    public protocolEditSave() {
        this.protocolButton();
    }
    // 备用ipv4地址change
    public ipAddressChangeV4Standby() {
        this.netProtocolForm.get('ipAddressControlV4').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV4Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 备用ipv4掩码change
    public maskCodeChangeV4Standby() {
        this.netProtocolForm.get('ipAddressControlV4Standby').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 备用ipv6地址change
    public ipAddressChangeV6Standby() {
        this.netProtocolForm.get('ipAddressControlV6').updateValueAndValidity();
        this.netProtocolForm.get('maskCodeBlurControlStandby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 备用ipv6前缀change
    public maskCodeBlurChangeStandby() {
        this.netProtocolForm.get('ipAddressControlV6Standby').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 浮动ipv4地址change
    public ipAddressChangeV4Float() {
        this.netProtocolForm.get('ipAddressControlV4Standby').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV4').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 浮动ipv4掩码change
    public maskCodeChangeV4Float() {
        this.netProtocolForm.get('ipAddressControlV6Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 浮动ipv6地址change
    public ipAddressChangeV6Float() {
        this.netProtocolForm.get('ipAddressControlV6').updateValueAndValidity();
        this.netProtocolForm.get('maskCodeBlurControlFloat').updateValueAndValidity();
        this.netProtocolForm.get('ipAddressControlV6Standby').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    // 浮动ipv6前缀change
    public maskCodeBlurChangeFloat() {
        this.netProtocolForm.get('ipAddressControlV6Float').updateValueAndValidity();
        this.protocolEnabledEN();
    }
}
