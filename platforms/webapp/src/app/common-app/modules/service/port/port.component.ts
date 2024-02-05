import { Component, OnInit } from '@angular/core';
import { TiTableColumns, TiTableRowData, TiTableSrcData, TiValidationConfig, TiValidators, TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormControl } from '@angular/forms';
import { PortService } from './services';
import { AlertMessageService, LoadingService, UserInfoService, ErrortipService } from 'src/app/common-app/service';
import { getMessageId } from 'src/app/common-app/utils';
import { PortType } from 'src/app/common-app/models/common.interface';
@Component({
    selector: 'app-port',
    templateUrl: './port.component.html',
    styleUrls: ['../service.component.scss', './port.component.scss']
})
export class PortComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private portService: PortService,
        private alert: AlertMessageService,
        private user: UserInfoService,
        private errorTipService: ErrortipService,
        private loadingService: LoadingService
    ) { }
    // 权限判断
    public isConfigureUser = this.user.hasPrivileges(['ConfigureUsers']);
    public isOemSecurityMgmtUser = this.user.hasPrivileges(['OemSecurityMgmt']);
    public isSystemLock = this.user.systemLockDownEnabled;
    public isIf = (this.isConfigureUser || this.isOemSecurityMgmtUser) && !this.isSystemLock;
    public isConfigure = this.isConfigureUser;

    public editDisabled: boolean = false;
    public isEdit: boolean = false;
    public protocolEnabled: string;
    public openAlert: boolean = false;
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public data: TiTableRowData[] = [];
    public columns: TiTableColumns[] = [
        {
            title: this.translate.instant('SERVICE'),
            width: '15%'
        },
        {
            title: this.translate.instant('SERVICE_PORT'),
            width: '35%'
        },
        {
            title: this.translate.instant('SERVICE_KVM_PORT2'),
            width: '35%'
        },
        {
            title: this.translate.instant('SERVICE_STATUS'),
            width: '15%'
        }
    ];

    // tip提示
    public tipStr: string = this.translate.instant('SERVICE_TIP_CONTENT');

    // 初始化
    public enablePortInit: object = {};
    public buttonSave: boolean = true;
    public portParams: object = {};
    // 修改初始化
    public paramsSave: object = {};
    public SSH: object = {};
    public NAT: object = {};
    public SNMP: object = {};
    public KVM: object = {};
    public VMM: object = {};
    public VIDEO: object = {};
    public VNC: object = {};
    public HTTP: object = {};
    public HTTPS: object = {};
    public IPMILANRMCP: object = {};
    public IPMILANRMCPPLUS: object = {};

    public validation: TiValidationConfig = {
        errorMessage: {
            required: this.translate.instant('SERVICE_PORT_RANGES'),
            number: this.translate.instant('SERVICE_PORT_RANGES'),
            integer: this.translate.instant('SERVICE_PORT_RANGES'),
            rangeValue: this.translate.instant('SERVICE_PORT_RANGES')
        }
    };

    // 状态栏
    public protocolEnabledEN(protocolEnabledEN: boolean): string {
        this.protocolEnabled = this.translate.instant('SERVICE_DISABLED');
        if (protocolEnabledEN) {
            this.protocolEnabled = this.translate.instant('SERVICE_ENABLED');
        }
        return this.protocolEnabled;
    }

    // 初始化数据查询
    public initData = (enablePort) => {
        this.paramsSave = {};
        this.SSH = {};
        this.NAT = {};
        this.SNMP = {};
        this.KVM = {};
        this.VMM = {};
        this.VIDEO = {};
        this.VNC = {};
        this.HTTP = {};
        this.HTTPS = {};
        this.IPMILANRMCP = {};
        this.IPMILANRMCPPLUS = {};
        // SSH
        if (enablePort.SSH) {
            this.portParams['SSHport'] = enablePort.SSH.Port;
            this.portParams['SSHState'] = enablePort.SSH.Enabled;
            this.portParams['isShowSSH'] = true;
        }
        // NAT
        if (enablePort.NAT) {
            this.portParams['NATport'] = enablePort.NAT.Port;
            this.portParams['NATState'] = enablePort.NAT.Enabled;
            this.portParams['isShowNAT'] = true;
        } else {
            this.portParams['isShowNAT'] = false;
        }
        // SNMP
        if (enablePort.SNMPAgent) {
            this.portParams['SNMPport'] = enablePort.SNMPAgent.Port;
            this.portParams['SNMPState'] = enablePort.SNMPAgent.Enabled;
            this.portParams['isShowSNMP'] = true;
        } else {
            this.portParams['isShowSNMP'] = false;
        }
        // KVM
        if (enablePort.KVM) {
            this.portParams['KVMport'] = enablePort.KVM.Port;
            this.portParams['KVMState'] = enablePort.KVM.Enabled;
            this.portParams['isShowKVM'] = true;
        } else {
            this.portParams['isShowKVM'] = false;
        }
        // VMM
        if (enablePort.VMM) {
            this.portParams['VMMport'] = enablePort.VMM.Port;
            this.portParams['VMMState'] = enablePort.VMM.Enabled;
            this.portParams['isShowVMM'] = true;
        } else {
            this.portParams['isShowVMM'] = false;
        }
        // VIDEO
        if (enablePort.Video) {
            this.portParams['VIDEOport'] = enablePort.Video.Port;
            this.portParams['VIDEOState'] = enablePort.Video.Enabled;
            this.portParams['isShowVIDEO'] = true;
        } else {
            this.portParams['isShowVIDEO'] = false;
        }
        // VNC
        if (enablePort.VNC) {
            this.portParams['VNCport'] = enablePort.VNC.Port;
            this.portParams['VNCState'] = enablePort.VNC.Enabled;
            this.portParams['isShowVNC'] = true;
        } else {
            this.portParams['isShowVNC'] = false;
        }
        // HTTP
        if (enablePort.HTTP) {
            this.portParams['HTTPport'] = enablePort.HTTP.Port;
            this.portParams['HTTPState'] = enablePort.HTTP.Enabled;
            this.portParams['isShowHTTP'] = true;
        }
        // HTTPS
        if (enablePort.HTTPS) {
            this.portParams['HTTPSport'] = enablePort.HTTPS.Port;
            this.portParams['HTTPSState'] = enablePort.HTTPS.Enabled;
            this.portParams['isShowHTTPS'] = true;
        }
        // IPMI
        if (enablePort.IPMILANRMCP) {
            this.portParams['IPMIport'] = enablePort.IPMILANRMCP.Port;
            this.portParams['IPMIport2'] = enablePort.IPMILANRMCP.SparePort;
            this.portParams['IPMIState'] = enablePort.IPMILANRMCP.Enabled;
            this.portParams['isShowIPMILANRMCP'] = true;
        }
        // IPMI1
        if (enablePort.IPMILANRMCPPlus) {
            this.portParams['IPMI1State'] = enablePort.IPMILANRMCPPlus.Enabled;
            this.portParams['isShowIPMILANRMCPPlus'] = true;
        }

        this.data = [
            {
                id: 'SSH',
                service: this.translate.instant('SERVICE_SSH'),
                show: this.portParams['isShowSSH'],
                port: this.portParams['SSHport'],
                port2: '',
                serviceStatus: this.portParams['SSHState'],
                statusValue: this.protocolEnabledEN(this.portParams['SSHState']),
            },
            {
                id: 'NAT',
                service: this.translate.instant('SERVICE_NAT'),
                show: this.portParams['isShowNAT'],
                port: this.portParams['NATport'],
                port2: '',
                serviceStatus: this.portParams['NATState'],
                statusValue: this.protocolEnabledEN(this.portParams['NATState'])
            },
            {
                id: 'SNMP',
                service: this.translate.instant('SERVICE_SNMP_AGENT'),
                show: this.portParams['isShowSNMP'],
                port: this.portParams['SNMPport'],
                port2: '',
                serviceStatus: this.portParams['SNMPState'],
                statusValue: this.protocolEnabledEN(this.portParams['SNMPState'])
            },
            {
                id: 'KVM',
                service: this.translate.instant('SERVICE_KVM'),
                show: this.portParams['isShowKVM'],
                port: this.portParams['KVMport'],
                port2: '',
                serviceStatus: this.portParams['KVMState'],
                statusValue: this.protocolEnabledEN(this.portParams['KVMState'])
            },
            {
                id: 'VMM',
                service: this.translate.instant('SERVICE_VMM'),
                show: this.portParams['isShowVMM'],
                port: this.portParams['VMMport'],
                port2: '',
                serviceStatus: this.portParams['VMMState'],
                statusValue: this.protocolEnabledEN(this.portParams['VMMState'])
            },
            {
                id: 'VIDEO',
                service: this.translate.instant('Video'),
                show: this.portParams['isShowVIDEO'],
                port: this.portParams['VIDEOport'],
                port2: '',
                serviceStatus: this.portParams['VIDEOState'],
                statusValue: this.protocolEnabledEN(this.portParams['VIDEOState'])
            },
            {
                id: 'VNC',
                service: this.translate.instant('SERVICE_VNC'),
                show: this.portParams['isShowVNC'],
                port: this.portParams['VNCport'],
                port2: '',
                serviceStatus: this.portParams['VNCState'],
                statusValue: this.protocolEnabledEN(this.portParams['VNCState'])
            },
            {
                id: 'HTTP',
                service: this.translate.instant('WEB_HTTP'),
                show: this.portParams['isShowHTTP'],
                port: this.portParams['HTTPport'],
                port2: '',
                serviceStatus: this.portParams['HTTPState'],
                statusValue: this.protocolEnabledEN(this.portParams['HTTPState'])
            },
            {
                id: 'HTTPS',
                service: this.translate.instant('WEB_HTTPS'),
                show: this.portParams['isShowHTTPS'],
                port: this.portParams['HTTPSport'],
                port2: '',
                serviceStatus: this.portParams['HTTPSState'],
                statusValue: this.protocolEnabledEN(this.portParams['HTTPSState'])
            },
            {
                id: 'IPMI',
                service: this.translate.instant('SERVICE_IPMI'),
                show: this.portParams['isShowIPMILANRMCP'],
                port: this.portParams['IPMIport'],
                port2: this.portParams['IPMIport2'],
                serviceStatus: this.portParams['IPMIState'],
                statusValue: this.protocolEnabledEN(this.portParams['IPMIState'])
            },
            {
                id: 'IPMI1',
                service: this.translate.instant('SERVICE_IPMI_RMCP'),
                show: this.portParams['isShowIPMILANRMCPPlus'],
                port: '',
                port2: '',
                serviceStatus: this.portParams['IPMI1State'],
                statusValue: this.protocolEnabledEN(this.portParams['IPMI1State'])
            },
        ];

        for (const key of this.data) {
            if (key.id !== 'IPMI1') {
                key.portValidationControl = new UntypedFormControl(
                    'key.port',
                    [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
                );
            }
            if (key.id === 'IPMI') {
                key.port1ValidationControl = new UntypedFormControl(
                    'key.port2',
                    [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 65535)]
                );
            }
        }
        this.srcData = {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
    }

    public portGetInit = () => {
        this.portService.getNetworkPort().subscribe((response) => {
            this.enablePortInit = response['body'];
            this.initData(this.enablePortInit);
        }, () => {
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
            this.isIf = (this.isConfigureUser || this.isOemSecurityMgmtUser) && !this.isSystemLock;
            if (!this.isIf) {
                this.isEdit = false;
                this.initData(this.enablePortInit);
            }
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);

        this.portGetInit();
    }

    // 权限判断 具有用户配置权限可以设置SNMP使能、端口,具有安全配置权限可以设置其它使能和端口
    public editShow = (row) => {
        this.editDisabled = this.isConfigureUser
            ? this.isEdit && row.id !== 'IPMI1'
            : (this.isOemSecurityMgmtUser ? this.isEdit && row.id !== 'IPMI1' && row.id !== 'SNMP' : false);
        if (!this.editDisabled) {
            if (!this.isConfigure && row.id === 'SNMP') {
                row.portValidationControl.disable();
            }
        }
        return this.editDisabled;
    }

    // 编辑
    public editPort(): void {
        this.isEdit = true;
    }

    // 取消
    public cancelPort(): void {
        this.isEdit = false;
        this.initData(this.enablePortInit);
    }

    public switchBeforeChagne(row: TiTableRowData) {
        // 双因素登录时，禁止开启SSH功能
        if (row.id === PortType.SSH && this.user.isTwoFacCertification) {
            this.alert.eventEmit.emit({ type: 'error', label: 'IBMC_SSH_OPEN_FORRBIDEN_TIP' });
            return;
        }
        row.serviceStatus = !row.serviceStatus;
        this.switchChange(row);
    }

    // 数据是否发生变化
    public portTypeChange = (obj, portType, portShow, portState) => {
        if (obj.show) {
            if (parseInt(obj.port, 10) !== portShow) {
                portType['Port'] = parseInt(obj.port, 10);
            } else {
                if (portType['Port'] !== '{}') {
                    delete portType['Port'];
                }
            }
            if (obj.serviceStatus !== portState) {
                portType['Enabled'] = obj.serviceStatus;
            } else {
                if (portType['Enabled'] !== '{}') {
                    delete portType['Enabled'];
                }
            }
        } else {
            portType = {};
        }
    }
    public portClick = (obj) => {
        // 表单校验
        for (const key of this.data) {
            if (key.id !== 'IPMI1') {
                if ((key.show && key.portValidationControl.errors) || (key.id === 'IPMI' && key.port1ValidationControl.errors)) {
                    this.buttonSave = true;
                    return;
                }
            }
        }
        if (obj.id === 'SSH') {
            this.portTypeChange(obj, this.SSH, this.portParams['SSHport'], this.portParams['SSHState']);
        }
        if (obj.id === 'NAT') {
            this.portTypeChange(obj, this.NAT, this.portParams['NATport'], this.portParams['NATState']);
        }
        if (obj.id === 'SNMP') {
            this.portTypeChange(obj, this.SNMP, this.portParams['SNMPport'], this.portParams['SNMPState']);
        }
        if (obj.id === 'KVM') {
            this.portTypeChange(obj, this.KVM, this.portParams['KVMport'], this.portParams['KVMState']);
        }
        if (obj.id === 'VMM') {
            this.portTypeChange(obj, this.VMM, this.portParams['VMMport'], this.portParams['VMMState']);
        }
        if (obj.id === 'VIDEO') {
            this.portTypeChange(obj, this.VIDEO, this.portParams['VIDEOport'], this.portParams['VIDEOState']);
        }
        if (obj.id === 'VNC') {
            this.portTypeChange(obj, this.VNC, this.portParams['VNCport'], this.portParams['VNCState']);
        }
        if (obj.id === 'HTTP') {
            this.portTypeChange(obj, this.HTTP, this.portParams['HTTPport'], this.portParams['HTTPState']);
        }
        if (obj.id === 'HTTPS') {
            this.portTypeChange(obj, this.HTTPS, this.portParams['HTTPSport'], this.portParams['HTTPSState']);
        }
        if (obj.id === 'IPMI') {
            if (parseInt(obj.port, 10) !== this.portParams['IPMIport']) {
                this.IPMILANRMCP['Port'] = parseInt(obj.port, 10);
            } else {
                if (this.IPMILANRMCP['Port'] !== '{}') {
                    delete this.IPMILANRMCP['Port'];
                }
            }
            if (parseInt(obj.port2, 10) !== this.portParams['IPMIport2']) {
                this.IPMILANRMCP['SparePort'] = parseInt(obj.port2, 10);
            } else {
                if (this.IPMILANRMCP['SparePort'] !== '{}') {
                    delete this.IPMILANRMCP['SparePort'];
                }
            }
            if (obj.serviceStatus !== this.portParams['IPMIState']) {
                this.IPMILANRMCP['Enabled'] = obj.serviceStatus;
            } else {
                if (this.IPMILANRMCP['Enabled'] !== '{}') {
                    delete this.IPMILANRMCP['Enabled'];
                }
            }
        }
        if (obj.id === 'IPMI1') {
            if (obj.serviceStatus !== this.portParams['IPMI1State']) {
                this.IPMILANRMCPPLUS['Enabled'] = obj.serviceStatus;
            } else {
                if (this.IPMILANRMCPPLUS['Enabled'] !== '{}') {
                    delete this.IPMILANRMCPPLUS['Enabled'];
                }
            }
        }
        // 判断页面数据是否发生变化
        if (this.isnotEmptyObject(this.SSH)) {
            this.paramsSave['SSH'] = this.SSH;
        }
        if (this.isnotEmptyObject(this.NAT)) {
            this.paramsSave['NAT'] = this.NAT;
        }
        if (this.isnotEmptyObject(this.SNMP)) {
            this.paramsSave['SNMPAgent'] = this.SNMP;
        }
        if (this.isnotEmptyObject(this.KVM)) {
            this.paramsSave['KVM'] = this.KVM;
        }
        if (this.isnotEmptyObject(this.VMM)) {
            this.paramsSave['VMM'] = this.VMM;
        }
        if (this.isnotEmptyObject(this.VIDEO)) {
            this.paramsSave['Video'] = this.VIDEO;
        }
        if (this.isnotEmptyObject(this.VNC)) {
            this.paramsSave['VNC'] = this.VNC;
        }
        if (this.isnotEmptyObject(this.HTTP)) {
            this.paramsSave['HTTP'] = this.HTTP;
        }
        if (this.isnotEmptyObject(this.HTTPS)) {
            this.paramsSave['HTTPS'] = this.HTTPS;
        }
        if (this.isnotEmptyObject(this.IPMILANRMCP)) {
            this.paramsSave['IPMILANRMCP'] = this.IPMILANRMCP;
        }
        if (this.isnotEmptyObject(this.IPMILANRMCPPLUS)) {
            this.paramsSave['IPMILANRMCPPlus'] = this.IPMILANRMCPPLUS;
        }

        // 过滤掉对象中的空值
        this.deleteNULL(this.paramsSave);
        if (JSON.stringify(this.paramsSave) !== '{}') {
            this.buttonSave = false;
        } else {
            this.buttonSave = true;
        }
        return this.paramsSave;
    }

    // 判断对象是否为空
    public isnotEmptyObject(obj): boolean {
        let res = false;
        for (const k in obj) {
            if (Object.prototype.hasOwnProperty.call(obj, k)) {
                res = true;
            }
        }
        return res;
    }

    // change
    public switchChange = (obj) => {
        if ((obj.id === 'VNC') && obj.serviceStatus) {
            this.alert.eventEmit.emit({ type: 'warn', label: 'SERVICE_OPEN_VNC' });
        }
        if ((obj.id === 'IPMI') && obj.serviceStatus) {
            this.alert.eventEmit.emit({ type: 'warn', label: 'SERVICE_OPEN_RMCP' });
        }
    }

    // 恢复默认值
    public restFN(obj: { port: number; port2: number; }, id: string, column: number): void {
        if (id === 'SSH') {
            obj.port = 22;
        }
        if (id === 'NAT') {
            obj.port = 30022;
        }
        if (id === 'SNMP') {
            obj.port = 161;
        }
        if (id === 'KVM') {
            obj.port = 2198;
        }
        if (id === 'VMM') {
            obj.port = 8208;
        }
        if (id === 'VIDEO') {
            obj.port = 2199;
        }
        if (id === 'VNC') {
            obj.port = 5900;
        }
        if (id === 'HTTP') {
            obj.port = 80;
        }
        if (id === 'HTTPS') {
            obj.port = 443;
        }
        if (id === 'IPMI') {
            if (column === 2) {
                obj.port = 623;
            } else if (column === 3) {
                obj.port2 = 664;
            }
        }
        this.portClick(obj);
    }

    public portChangeFun (resHTTPSPort, resHTTPPort) {
        const protocol = window.location.protocol;
        // 如果是HTTPS链接，判断HTTPS端口是否变更，如果变更将浏览器端口改为实际值
        if (protocol === 'https:' && resHTTPSPort && window.location.port !== resHTTPSPort) {
            window.location.port = resHTTPSPort;
        }
        // 如果是HTTP链接，判断HTTP端口是否变更，如果变更将浏览器端口改为实际值
        if (protocol === 'http:' && resHTTPPort && window.location.port !== resHTTPPort) {
            window.location.port = resHTTPPort;
        }
    }

    // 保存
    public savePort(): void {
        // 表单校验
        for (const key of this.data) {
            if (key.id !== 'IPMI1') {
                if ((key.port && key.portValidationControl.errors) || (key.id === 'IPMI' && key.port1ValidationControl.errors)) {
                    this.buttonSave = true;
                    return;
                }
            }
        }
        const params = this.paramsSave;
        const instance = this.tiMessage.open({
            id: 'confirmPort',
            type: 'prompt',
            content: this.translate.instant('SERVICE_TIMESSAGE_CONTENT'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    instance.close();
                    this.isEdit = false;
                    this.portService.setPortService(params).subscribe((response) => {
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        this.enablePortInit = response['body'];
                        this.initData(this.enablePortInit);
                        const resHTTPSPort = params['HTTPS'] && params['HTTPS']['Port'] > 0 ? response['body'].HTTPS.Port : null;
                        const resHTTPPort = params['HTTP'] && params['HTTP']['Port'] > 0 ? response['body'].HTTP.Port : null;
                        // 延时3秒解决刷新太快，自动化无法捕获成功提示
                        setTimeout (() => {
                            this.portChangeFun(resHTTPSPort, resHTTPPort);
                        }, 3000);
                    }, (error) => {
                        const errorId = getMessageId(error.error)[0].errorId;
                        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                        this.enablePortInit = error.error.data;
                        this.initData(this.enablePortInit);
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
    public deleteNULL(obj: object) {
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
}
