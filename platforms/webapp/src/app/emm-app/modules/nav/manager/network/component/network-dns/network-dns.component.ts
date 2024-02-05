import { Component, OnInit, Input, OnChanges, Output, EventEmitter } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder, Validators } from '@angular/forms';
import { TiValidationConfig, TiMessageService } from '@cloud/tiny3';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { NetworkService } from '../../services';
import { AlertMessageService, UserInfoService, ErrortipService, SystemLockService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-network-dns',
    templateUrl: './network-dns.component.html',
    styleUrls: ['../../network.component.scss', './network-dns.component.scss']
})
export class NetworkDnsComponent implements OnInit, OnChanges {

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private user: UserInfoService,
        private networkService: NetworkService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private errorTipService: ErrortipService,
        private lockService: SystemLockService
    ) { }

    @Input() networkDns;
    @Input() dnsBeingEdited;
    @Input() dnsEditDisabled;
    @Input() HMMID;
    @Output() private outer = new EventEmitter();

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;

    public selectedDnsServerModel: string;

    public dnsDisabled: boolean = true;
    public queryParams = {};

    public dnsServerList: { key: string; id: string; disable: boolean }[] = [
        {
            key: this.translate.instant('IBMC_DHCPV4_AUTOGET'),
            id: 'IPv4',
            disable: false
        }, {
            key: this.translate.instant('IBMC_DHCPV6_AUTOGET'),
            id: 'IPv6',
            disable: false
        }, {
            key: this.translate.instant('IBMC_IP_CONFIGURATION'),
            id: 'Static',
            disable: false
        }
    ];

    public dnsParams = {};

    public netDnsForm = this.fb.group({
        selectedDnsServer: [null, []],
        domainControl: [{ value: null, disabled: true }, [
            Validators.pattern(/^(?=^.{2,67}$)(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63}))(\.(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63}))?)+$/)
        ]],
        preferredControl: [{ value: null, disabled: true }, [SerAddrValidators.validateIp46()]],
        serverControl1: [{ value: null, disabled: true }, [SerAddrValidators.validateIp46()]],
        serverControl2: [{ value: null, disabled: true }, [SerAddrValidators.validateIp46()]],
    });

    // 域名
    validationDomain: TiValidationConfig = {
        tip: this.translate.instant('IBMC_DNS_DOMAIN_TIP'),
        tipMaxWidth: '400px',
        type: 'blur',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };

    public validationServer: TiValidationConfig = {
        tip: this.translate.instant('IBMC_DNS_TIP_CHECK'),
        type: 'blur',
        tipPosition: 'right',
        errorMessage: {
            validateIp46 : this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };

    public dnsInit = (networkDns) => {
        if (JSON.stringify(networkDns) === '{}') {
            return;
        }
        // dns模式选择
        this.dnsParams['dnsAddress'] = networkDns.getDnsAddress();
        // 域名
        this.dnsParams['dnsDomain'] = networkDns.getDnsDomain();
        // 首选服务器
        this.dnsParams['dnsServer'] = networkDns.getDnsServer();
        // 备选服务器1
        this.dnsParams['dnsServer1'] = networkDns.getDnsServer1();
        // 备选服务器2
        this.dnsParams['dnsServer2'] = networkDns.getDnsServer2();
        switch (this.dnsParams['dnsAddress']) {
            case 'IPv4':
                this.selectedDnsServerModel = 'IBMC_DHCPV4_AUTOGET';
                break;
            case 'IPv6':
                this.selectedDnsServerModel = 'IBMC_DHCPV6_AUTOGET';
                break;
            case 'Static':
                this.selectedDnsServerModel = 'IBMC_IP_CONFIGURATION';
                break;
            default:
                this.selectedDnsServerModel = 'IBMC_IP_CONFIGURATION';
        }
        this.netDnsForm.patchValue({
            selectedDnsServer: this.dnsParams['dnsAddress'],
            domainControl: this.dnsParams['dnsDomain'],
            preferredControl: this.dnsParams['dnsServer'],
            serverControl1: this.dnsParams['dnsServer1'],
            serverControl2: this.dnsParams['dnsServer2'],
        });
        this.ipVersionDNS(networkDns);
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
            }
        });
        if (this.netDnsForm.root.get('selectedDnsServer').value === 'Static' && this.isConfigureComponentsUser && !this.isSystemLock) {
            this.netDnsForm.root.get('domainControl').enable();
            this.netDnsForm.root.get('preferredControl').enable();
            this.netDnsForm.root.get('serverControl1').enable();
            this.netDnsForm.root.get('serverControl2').enable();
        } else {
            this.netDnsForm.root.get('domainControl').disable();
            this.netDnsForm.root.get('preferredControl').disable();
            this.netDnsForm.root.get('serverControl1').disable();
            this.netDnsForm.root.get('serverControl2').disable();
        }
    }
    ngOnInit() {
    }
    public ipVersionDNS = (networkDns) => {
        if (networkDns.getIpVersion() === 'IPv4') {
            if (networkDns.getNetModelV4() !== 'Static') {
                this.dnsServerList[0].disable = false;
                this.dnsServerList[1].disable = true;
            } else {
                this.dnsServerList[0].disable = true;
                this.dnsServerList[1].disable = true;
            }
        } else if (networkDns.getIpVersion() === 'IPv6') {
            if (networkDns.getNetModelV6() !== 'Static') {
                this.dnsServerList[0].disable = true;
                this.dnsServerList[1].disable = false;
            } else {
                this.dnsServerList[0].disable = true;
                this.dnsServerList[1].disable = true;
            }
        } else if (networkDns.getIpVersion() === 'IPv4AndIPv6') {
            if (networkDns.getNetModelV4() !== 'Static') {
                this.dnsServerList[0].disable = false;
            } else {
                this.dnsServerList[0].disable = true;
            }
            if (networkDns.getNetModelV6() !== 'Static') {
                this.dnsServerList[1].disable = false;
            } else {
                this.dnsServerList[1].disable = true;
            }
        }
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.networkDns && changes.networkDns.currentValue) {
            this.dnsInit(changes.networkDns.currentValue);
        }
    }

    public protocolEnabledEN = () => {
        if (!this.netDnsForm.valid) {
            this.dnsDisabled = true;
            return;
        }
        const nameServers = [
            this.dnsParams['dnsServer'],
            this.dnsParams['dnsServer1'],
            this.dnsParams['dnsServer2']
        ];
        const mode = {
            Oem: {
                Huawei: {}
            }
        };
        this.queryParams = {};
        if (this.netDnsForm.root.get('selectedDnsServer').value !== this.dnsParams['dnsAddress']) {
            mode.Oem.Huawei['DNSAddressOrigin'] = this.netDnsForm.root.get('selectedDnsServer').value;
        }
        if (this.netDnsForm.root.get('domainControl').value !== this.dnsParams['dnsDomain']) {
            this.queryParams['FQDN'] = this.netDnsForm.root.get('domainControl').value;
        }
        if (this.netDnsForm.root.get('preferredControl').value !== this.dnsParams['dnsServer']) {
            nameServers[0] = this.netDnsForm.root.get('preferredControl').value;
        }
        if (this.netDnsForm.root.get('serverControl1').value !== this.dnsParams['dnsServer1']) {
            nameServers[1] = this.netDnsForm.root.get('serverControl1').value;
        }
        if (this.netDnsForm.root.get('serverControl2').value !== this.dnsParams['dnsServer2']) {
            nameServers[2] = this.netDnsForm.root.get('serverControl2').value;
        }
        if (nameServers[0] !== this.dnsParams['dnsServer']
            || nameServers[1] !== this.dnsParams['dnsServer1']
            || nameServers[2] !== this.dnsParams['dnsServer2']) {
            this.queryParams['NameServers'] = nameServers;
        }
        if (JSON.stringify(mode.Oem.Huawei) !== '{}') {
            this.queryParams['Oem'] = mode.Oem;
        }
        if (JSON.stringify(this.queryParams) !== '{}') {
            this.dnsDisabled = false;
        } else {
            this.dnsDisabled = true;
        }
        return this.queryParams;

    }
    public changeDNSFn = () => {
        if (this.netDnsForm.root.get('selectedDnsServer').value === 'IPv4') {
            this.netDnsForm.root.get('domainControl').disable();
            this.netDnsForm.root.get('preferredControl').disable();
            this.netDnsForm.root.get('serverControl1').disable();
            this.netDnsForm.root.get('serverControl2').disable();
            this.netDnsForm.patchValue({
                domainControl: this.dnsParams['dnsDomain'],
                preferredControl: this.dnsParams['dnsServer'],
                serverControl1: this.dnsParams['dnsServer1'],
                serverControl2: this.dnsParams['dnsServer2']
            });
        } else if (this.netDnsForm.root.get('selectedDnsServer').value === 'IPv6') {
            this.netDnsForm.root.get('domainControl').disable();
            this.netDnsForm.root.get('preferredControl').disable();
            this.netDnsForm.root.get('serverControl1').disable();
            this.netDnsForm.root.get('serverControl2').disable();
            this.netDnsForm.patchValue({
                domainControl: this.dnsParams['dnsDomain'],
                preferredControl: this.dnsParams['dnsServer'],
                serverControl1: this.dnsParams['dnsServer1'],
                serverControl2: this.dnsParams['dnsServer2']
            });
        } else if (this.netDnsForm.root.get('selectedDnsServer').value === 'Static') {
            this.netDnsForm.root.get('domainControl').enable();
            this.netDnsForm.root.get('preferredControl').enable();
            this.netDnsForm.root.get('serverControl1').enable();
            this.netDnsForm.root.get('serverControl2').enable();
        }
    }
    // change事件
    public dnsRadioChange = () => {
        this.changeDNSFn();
        this.protocolEnabledEN();
    }
    public domainChange = () => {
        this.protocolEnabledEN();
    }
    public preferredChange = () => {
        this.protocolEnabledEN();
    }
    public serverChange1 = () => {
        this.protocolEnabledEN();
    }
    public serverChange2 = () => {
        this.protocolEnabledEN();
    }

    // 该方法得作用在于解决自动化脚本再重置输入框得值为空值时，不能触发代码得change事件，同时 html的事件需要修改为ngModelChange
    public dipatchEvent(param: { value: string, name: string }): void {
        this.netDnsForm.controls[param.name].patchValue(param.value);
    }

    // 保存
    public dnsButton(): void {
        this.dnsDisabled = true;
        this.protocolEnabledEN();
        const instance = this.tiMessage.open({
            id: 'dnsConfirm',
            type: 'prompt',
			content: this.translate.instant('IBMC_SAVE_DNS_POP_UP_BOX'),
			okButton: {
				show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
				click: () => {
                    instance.close();
                    this.networkService.saveHost(this.HMMID, JSON.stringify(this.queryParams)).subscribe((response) => {
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        this.dnsDisabled = true;
                        this.networkService.networkDnsDetails(this.networkDns, response['body']);
                        this.dnsInit(this.networkDns);
                        this.outer.emit(false);
                    }, (error) => {
                        const errorId = getMessageId(error.error)[0].errorId;
                        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({type: 'error', label: errorMessage});
                        this.dnsInit(this.networkDns);
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

    public dnsEdit() {
        this.outer.emit(true);
    }
    public dnsEditCancel() {
        this.netDnsForm.patchValue({
            selectedDnsServer: this.dnsParams['dnsAddress'],
            domainControl: this.dnsParams['dnsDomain'],
            preferredControl: this.dnsParams['dnsServer'],
            serverControl1: this.dnsParams['dnsServer1'],
            serverControl2: this.dnsParams['dnsServer2'],
        });
        if (this.netDnsForm.valid) {
            this.outer.emit(false);
        }
    }
    public dnsEditSave() {
        this.dnsButton();
    }
}
