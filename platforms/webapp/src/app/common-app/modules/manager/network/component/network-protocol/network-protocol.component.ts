import { Component, OnChanges, OnInit, Input, ElementRef } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder, Validators, UntypedFormControl } from '@angular/forms';
import { NetworkService } from '../../services';
import { AlertMessageService, UserInfoService, ErrortipService, SystemLockService } from 'src/app/common-app/service';
import { TiValidators, TiValidationConfig, TiMessageService } from '@cloud/tiny3';
import { IpV4Vaild } from '../../model';
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

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public iPAddressMore = true;

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
            id: 'DHCPv6',
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
        NetworkProtocols: {
            IPv4Config: {},
            IPv6Config: {}
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
            { value: null, disabled: true },
            [
                TiValidators.required,
                Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
                IpV4Vaild.firstIP(),
                IpV4Vaild.isEqualAddress('defaulGatewayControlV4', 'maskCodeControlV4')
            ]
        ],
        maskCodeControlV4: [
            { value: null, disabled: true },
            [
                TiValidators.required,
                Validators.pattern(/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/),
                IpV4Vaild.filtrationIP()
            ]
        ],
        defaulGatewayControlV4: [
            { value: null, disabled: true },
            [
                TiValidators.required,
                IpV4Vaild.isEqualAddress('ipAddressControlV4', 'maskCodeControlV4')
            ]
        ],
        selectedModelV6: [null, []],
        ipAddressControlV6: [{ value: null, disabled: true }, []],
        maskCodeBlurControl: [{ value: null, disabled: true }, []],
        gatewayControlV6: [{ value: null, disabled: true }, []]
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

    public switchDisableV4 = (switchFlag: boolean) => {
        if (switchFlag) {
            // 清除ipv4校验规
            this.netProtocolForm.get('ipAddressControlV4').clearValidators();
            this.netProtocolForm.get('maskCodeControlV4').clearValidators();
            this.netProtocolForm.get('defaulGatewayControlV4').clearValidators();
            TiValidators.check(this.netProtocolForm);
        } else {
            // 还原ipv4校验规
            this.netProtocolForm.get('ipAddressControlV4').setValidators(
                [
                    TiValidators.required,
                    Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
                    IpV4Vaild.firstIP(),
                    IpV4Vaild.isEqualAddress('defaulGatewayControlV4', 'maskCodeControlV4')
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
            // 还原ipv6校验规
            this.netProtocolForm.get('ipAddressControlV6').setValidators(
                [
                    IpV4Vaild.maskCode(this.protocalParams['netModelV6'], this.protocalParams['ipAddressV6'], 'maskCodeBlurControl', 'selectedNetProtocol'),
                    SerAddrValidators.validateIp6(),
                    this.ipVaild.validateIpv6('gatewayControlV6', 'maskCodeBlurControl')
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
                IpV4Vaild.isEqualAddress('defaulGatewayControlV4', 'maskCodeControlV4')
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
                this.ipVaild.validateIpv6('gatewayControlV6', 'maskCodeBlurControl')
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
        } else {
            if (this.isConfigureComponentsUser
                && !this.isSystemLock
                && this.netProtocolForm.root.get('selectedNetProtocol').value !== 'IPv6') {
                // 输入框可编辑
                this.netProtocolForm.root.get('ipAddressControlV4').enable();
                this.netProtocolForm.root.get('maskCodeControlV4').enable();
                this.netProtocolForm.root.get('defaulGatewayControlV4').enable();
            } else {
                this.netProtocolForm.root.get('ipAddressControlV4').disable();
                this.netProtocolForm.root.get('maskCodeControlV4').disable();
                this.netProtocolForm.root.get('defaulGatewayControlV4').disable();
            }

        }
    }
    // IpV6检验变化
    public ipValidationV6 = () => {
        if (this.netProtocolForm.root.get('selectedModelV6').value === 'DHCPv6') {
            this.netProtocolForm.patchValue({
                ipAddressControlV6: this.protocalParams['ipAddressV6'],
                maskCodeBlurControl: this.protocalParams['maskCodeV6'],
                gatewayControlV6: this.protocalParams['gatewayV6']
            });
            // 输入框禁用
            this.netProtocolForm.root.get('ipAddressControlV6').disable();
            this.netProtocolForm.root.get('maskCodeBlurControl').disable();
            this.netProtocolForm.root.get('gatewayControlV6').disable();
        } else {
            // 输入框可编辑
            if (this.isConfigureComponentsUser
                && !this.isSystemLock
                && this.netProtocolForm.root.get('selectedNetProtocol').value !== 'IPv4') {
                this.netProtocolForm.root.get('ipAddressControlV6').enable();
                this.netProtocolForm.root.get('maskCodeBlurControl').enable();
                this.netProtocolForm.root.get('gatewayControlV6').enable();
            } else {
                this.netProtocolForm.root.get('ipAddressControlV6').disable();
                this.netProtocolForm.root.get('maskCodeBlurControl').disable();
                this.netProtocolForm.root.get('gatewayControlV6').disable();
            }

        }
    }

    public protocolInit = (networkProtocol) => {
        if (JSON.stringify(networkProtocol) === '{}') {
            return;
        }
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
            gatewayControlV6: this.protocalParams['gatewayV6']
        });
        // IPv6校验
        this.netProtocolForm.setControl('ipAddressControlV6', new UntypedFormControl('',
            [
                IpV4Vaild.maskCode(this.protocalParams['netModelV6'], this.protocalParams['ipAddressV6'], 'maskCodeBlurControl', 'selectedNetProtocol'),
                SerAddrValidators.validateIp6(),
                this.ipVaild.validateIpv6('gatewayControlV6', 'maskCodeBlurControl')
            ]
        ));
        this.netProtocolForm.setControl('maskCodeBlurControl', new UntypedFormControl('',
            [
                IpV4Vaild.maskCode(this.protocalParams['netModelV6'], this.protocalParams['maskCodeV6'], 'ipAddressControlV6', 'selectedNetProtocol'),
                Validators.pattern(/^(?:[0-9]|[0-9][0-9]|1[0-1][0-9]|12[0-8])$/)
            ]
        ));
        this.netProtocolForm.setControl('gatewayControlV6', new UntypedFormControl('',
            [
                IpV4Vaild.gateway(this.protocalParams['netModelV6'], this.protocalParams['gatewayV6'], 'selectedNetProtocol'),
                SerAddrValidators.validateIp6(),
                this.ipVaild.gatewayIpv6('ipAddressControlV6', 'maskCodeBlurControl')
            ]
        ));
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
            gatewayControlV6: this.protocalParams['gatewayV6']
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
            NetworkProtocols: {
                IPv4Config: {},
                IPv6Config: {}
            }
        };
        if (
            this.netProtocolForm.root.get('selectedNetProtocol').value
            && this.netProtocolForm.root.get('selectedNetProtocol').value !== this.protocalParams['netProtocol']
        ) {
            this.queryParams.NetworkProtocols['ProtocolMode'] = this.netProtocolForm.root.get('selectedNetProtocol').value;
        }
        if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6'
            || this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4') {
            // IPv4模式
            if (
                this.netProtocolForm.root.get('selectedModelV4').value !== null
                && this.netProtocolForm.root.get('selectedModelV4').value !== this.protocalParams['netModelV4']
            ) {
                this.queryParams.NetworkProtocols.IPv4Config['IPAddressMode'] = this.netProtocolForm.root.get('selectedModelV4').value;
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
                    this.queryParams.NetworkProtocols.IPv4Config['IPAddress'] = this.netProtocolForm.root.get('ipAddressControlV4').value;
                    this.queryParams.NetworkProtocols.IPv4Config['SubnetMask'] = this.netProtocolForm.root.get('maskCodeControlV4').value;
                    this.queryParams.NetworkProtocols.IPv4Config['Gateway'] = this.netProtocolForm.root.get('defaulGatewayControlV4').value;
                }
            }
        }
        if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv4AndIPv6'
            || this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv6') {
            // IPv6模式
            if (
                this.netProtocolForm.root.get('selectedModelV6').value !== null
                && this.netProtocolForm.root.get('selectedModelV6').value !== this.protocalParams['netModelV6']
            ) {
                this.queryParams.NetworkProtocols.IPv6Config['IPAddressMode'] = this.netProtocolForm.root.get('selectedModelV6').value;
            }
            // IPv6手动配置
            if (this.netProtocolForm.root.get('selectedModelV6').value === 'Static') {
                if (
                    this.netProtocolForm.root.get('ipAddressControlV6').value !== null
                    && this.netProtocolForm.root.get('ipAddressControlV6').value !== this.protocalParams['ipAddressV6']
                ) {
                    this.queryParams.NetworkProtocols.IPv6Config['IPAddress']
                        = this.netProtocolForm.root.get('ipAddressControlV6').value;
                }
                if (
                    this.netProtocolForm.root.get('maskCodeBlurControl').value !== null
                    && ((this.netProtocolForm.root.get('maskCodeBlurControl').value === ''
                    && this.netProtocolForm.root.get('maskCodeBlurControl').value !== this.protocalParams['maskCodeV6'])
                    || (this.netProtocolForm.root.get('maskCodeBlurControl').value !== ''
                        && parseInt(this.netProtocolForm.root.get('maskCodeBlurControl').value, 10) !== this.protocalParams['maskCodeV6']))
                ) {
                    this.queryParams.NetworkProtocols.IPv6Config['PrefixLength'] = parseInt(this.netProtocolForm.root.get('maskCodeBlurControl').value, 10);
                }
                if (
                    this.netProtocolForm.root.get('gatewayControlV6').value !== null
                    && this.netProtocolForm.root.get('gatewayControlV6').value !== this.protocalParams['gatewayV6']
                ) {
                    this.queryParams.NetworkProtocols.IPv6Config['Gateway'] = this.netProtocolForm.root.get('gatewayControlV6').value;
                }
            }
        }
        this.deleteNull(this.queryParams.NetworkProtocols);
        this.deleteNull(this.queryParams);
        if (JSON.stringify(this.queryParams) !== '{}') {
            this.protocolDisabled = false;
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
        } else if (this.netProtocolForm.root.get('selectedNetProtocol').value === 'IPv6') {
            // 清除ipv4校验规则
            this.switchDisableV4(true);
            // 还原ipv6规则
            this.switchDisableV6(false);
        } else {
            this.switchDisableV4(false);
            this.switchDisableV6(false);
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
        this.ipValidationV6();
        this.protocolEnabledEN();
        TiValidators.check(this.netProtocolForm);
    }
    public ipAddressChangeV6 = () => {
        this.netProtocolForm.get('maskCodeBlurControl').updateValueAndValidity();
        this.netProtocolForm.get('gatewayControlV6').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public maskCodeBlurChange = () => {
        this.netProtocolForm.get('ipAddressControlV6').updateValueAndValidity();
        this.netProtocolForm.get('gatewayControlV6').updateValueAndValidity();
        this.protocolEnabledEN();
    }
    public gatewayChangeV6 = () => {
        this.netProtocolForm.get('ipAddressControlV6').updateValueAndValidity();
        this.netProtocolForm.get('maskCodeBlurControl').updateValueAndValidity();
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
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        this.networkService.networkProtocolDetails(this.networkProtocol, response['body']);
                        this.protocolInit(this.networkProtocol);
                    }, (error) => {
                        const errorId = getMessageId(error.error)[0].errorId;
                        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({type: 'error', label: errorMessage});
                        this.networkService.networkProtocolDetails(this.networkProtocol, error.error.data);
                        this.protocolInit(this.networkProtocol);
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
				&& (JSON.stringify(obj[key]) === '{}')
			) {
				delete obj[key];
			}
		}
		return obj;
    }

    // ipv4信息输入框失焦时，如果有多条错误信息只留下一条
    public inputBlurFn(event) {
        const inputId = event.target.parentNode.parentNode.getAttribute('id');
        // 获取当前输入框下所有的错误提示节点
        const errors = document.querySelectorAll(`#${inputId} ti-error-msg`);
        if (errors.length > 1) {
            errors.forEach((item, index) => {
                if (index !== 0) {
                    item.remove();
                }
            });
        }
    }
}
