import { Component, Input, OnInit, ViewChild, ViewContainerRef } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiModalService, TiModalRef, TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { NetworkService } from '../../network.service';
import { IIPV6Table, IPAddress, IPAddressControlPrefix, IPV6Address, IIPV6Batch } from '../../model';
import { IPv6Supplement, IPv6Translation, convertToBinary } from '../../model/data.func';
import { DhcpConfigComponent } from '../dhcp-config/dhcp-config.component';
import { Ipv6BatchSettingComponent } from '../ipv6-batch-setting/ipv6-batch-setting.component';
import { AlertMessageService, LoadingService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { AbstractControl, UntypedFormBuilder, UntypedFormControl, UntypedFormGroup, ValidationErrors, ValidatorFn, Validators } from '@angular/forms';
import { getMessageId, iPv6Translation, MultVaild, restituteIpv6, SerAddrValidators } from 'src/app/common-app/utils';
import { BmcValidator } from '../../model/bmc.validator';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';

@Component({
    selector: 'app-ipv6-net',
    templateUrl: './ipv6-net.component.html',
    styleUrls: ['./ipv6-net.component.scss']
})
export class Ipv6NetComponent implements OnInit {
    @Input() slotList;
    @Input() managerId;
    @ViewChild('dhcpConfig', { read: ViewContainerRef, static: true }) dhcpConfig: ViewContainerRef;
    public closeOtherDetails: boolean = false;
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;

    public disabled = true;
    public allSwitchState = false;

    // 表单数据
    public ipv6Form: IIPV6Table[] = [];
    private baseFormData: { id: string, url: string, baseValue: any[], group: UntypedFormGroup }[] = [];
    private changeData: { [key: string]: any } = {};
    private conflictIps = [];

    public columns: TiTableColumns[] = [
        {
            title: ''
        },
        {
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '9%'
        },
        {
            title: 'IBMC_IPV6_ADDRESS',
            width: '25%'
        },
        {
            title: 'IBMC_IP_ADDRESS03',
            width: '25%'
        },
        {
            title: 'IBMC_IP_ADDRESS11',
            width: '15%'
        },
        {
            title: 'IBMC_SLAAC_IPV6_AMOUNT',
            width: '15%'
        },
        {
            title: 'IBMC_DHCP_MODEL',
            width: '12%'
        }
    ];
    // 按钮的禁用状态，初始为true
    public curBtnState: boolean = true;

    // 全部开启、全部关闭
    public component: any = DhcpConfigComponent;
    public tipContext: any = {
        outputs: {
            activeChange: (active: boolean) => {
                const method = active ? 'enable' : 'disable';
                this.ipv6Form.forEach(form => {
                    const ipName = form.ipControlName;
                    const gateName = form.gateControlName;
                    const prefixName = form.prefixControlName;
                    form.formGroup.controls[ipName][method]();
                    form.formGroup.controls[gateName][method]();
                    form.formGroup.controls[prefixName][method]();
                    form.formGroup.controls[form.dhcpControlName].patchValue(active);
                });
                this.computeBatchState();
            }
        }
    };

    // IPV6地址校验提示
    public ipv6AddressTip: TiValidationConfig = {
        tip: this.translate.instant('IBMC_IPV6_RANGES'),
        errorMessage: {
            'validateIp6': this.translate.instant('IBMC_INVALID_IPADDRESS_V6'),
            'invalidAddress': this.translate.instant('IBMC_INVALID_IPADDRESS_V6')
        }
    };

    // IPV6默认网关校验提示
    public ipv6GateWayTip: TiValidationConfig = {
        errorMessage: {
            'validateIp6': this.translate.instant('IBMC_INVALID_IPADDRESS_V6')
        }
    };

    // 前缀校验提示
    public ipv6PrefixTip: TiValidationConfig = {
        tip: this.translate.instant('IBMC_IPV6PREFIX_LENGTH'),
        errorMessage: {
            integer: this.translate.instant('IBMC_IPV6PREFIX_LENGTH'),
            pattern: this.translate.instant('IBMC_IPV6PREFIX_LENGTH')
        }
    };

    // IPV6地址校验规则 1. 有效的IPV6地址 2. 转换成大写后不能以FF、FE8、FE9、FEA、FEB开头
    public ipv6AddressValidator = [
        Validators.required,
        SerAddrValidators.validateIp6(),
        BmcValidator.validateIp46()
    ];
    // 网关校验规则，1. 有效的IPV6地址
    public ipv6GatewayValidator = [
        Validators.required,
        SerAddrValidators.validateIp6()
    ];
    public ipv6PrefixValidator = [
        Validators.required,
        TiValidators.integer,
        Validators.pattern(/^(?:[0-9]|[0-9][0-9]|1[0-1][0-9]|12[0-8])$/)
    ];

    constructor(
        private networkService: NetworkService,
        private fb: UntypedFormBuilder,
        private modal: TiModalService,
        private loading: LoadingService,
        private translate: TranslateService,
        private alertService: AlertMessageService
    ) {

    }

    ngOnInit(): void {
        this.init();
        this.getConflictIp();
    }

    public init() {
        this.loading.state.next(true);
        this.changeData = {};
        this.baseFormData = [];
        if (this.slotList?.ips.length > 0) {
            this.networkService.getIPV6Info(this.slotList.ips).subscribe({
                next: (res: IPV6Address[]) => {
                    this.initForms(res);
                    this.srcData = {
                        data: this.ipv6Form,
                        state: {
                            searched: false,
                            sorted: false,
                            paginated: false
                        }
                    };
                },
                complete: () => {
                    this.loading.state.next(false);
                }
            });
        } else {
            this.loading.state.next(false);
        }
    }

    // 跳转到Blade登录页
    public openBladeLogin(url: string) {
        const linkUrl = `https://[${url}]`;
        window.open(linkUrl, '_blank');
    }

    // 获取当前设备的IP作为冲突IP列表
    public getConflictIp() {
        forkJoin([
            this.networkService.getCurrentDeviceIp({ board: 'HMM1', type: 'ipv6', ipType: 'static' }),
            this.networkService.getCurrentDeviceIp({ board: 'HMM2', type: 'ipv6', ipType: 'static' }),
            this.networkService.getCurrentDeviceIp({ board: this.managerId, type: 'ipv6', ipType: 'float' }),
        ]).subscribe({
            next: (res) => {
                let result = [];
                res.map(item => {
                    result = result.concat(item);
                });
                this.conflictIps.push(...result);
            },
            error: () => {

            }
        });
    }

    // 记录控件变更标记
    private recordChange(group: UntypedFormGroup) {
        let targetId = '';
        let isFormChange = false;
        this.baseFormData.forEach(item => {
            if (item.group === group) {
                targetId = item.id;
                isFormChange = JSON.stringify(item.baseValue) !== JSON.stringify(group.value);
            }
        });
        // 如果数据没有变化，则删除changeData的标记，否则，添加该标记
        if (isFormChange) {
            this.changeData[targetId] = group;
        } else {
            delete this.changeData[targetId];
        }
    }

    // 初始化表单
    private initForms(ipv6Datas: IPV6Address[]) {
        this.disabled = true;
        this.ipv6Form = [];
        ipv6Datas.forEach((item: IPV6Address, index: number) => {
            const ipValidator = [...this.ipv6AddressValidator];
            const gateValidator = [...this.ipv6GatewayValidator];
            const prefixValidator = [...this.ipv6PrefixValidator];

            const ipControl = new UntypedFormControl({ value: item.address, disabled: item.dhcpModel }, []);
            const prefixControl = new UntypedFormControl({ value: item.prefix, disabled: item.dhcpModel }, []);
            const gateControl = new UntypedFormControl({ value: item.gateWay, disabled: item.dhcpModel }, []);
            const dhcpControl = new UntypedFormControl(item.dhcpModel);
            const tempGroup = this.fb.group({});

            // 追加IP不能与环境IP一致的校验规则
            ipValidator.push(MultVaild.conflictIp(this.ipv6Form, item.ipControlName, this.conflictIps));
            ipValidator.push(this.validateIpv6(item.gateControlName, item.prefixControlName));
            gateValidator.push(this.validateIpv6(item.ipControlName, item.prefixControlName));
            ipControl.setValidators(ipValidator);
            gateControl.setValidators(gateValidator);
            prefixControl.setValidators(prefixValidator);

            tempGroup.addControl(item.ipControlName, ipControl);
            tempGroup.addControl(item.prefixControlName, prefixControl);
            tempGroup.addControl(item.gateControlName, gateControl);
            tempGroup.addControl(item.dhcpControlName, dhcpControl);

            tempGroup.valueChanges.subscribe(() => {
                this.recordChange(tempGroup);
                this.computeState();
            });

            const param: IIPV6Table = {
                slotNum: item.slotNum,
                linkUrl: item.address,
                url: item.url,
                slaacQuantity: item.slaacQuantity,
                ipControlName: item.ipControlName,
                prefixControlName: item.prefixControlName,
                gateControlName: item.gateControlName,
                dhcpControlName: item.dhcpControlName,
                formGroup: tempGroup
            };
            this.ipv6Form.push(param);

            // 记录baseForm数据，用于后续的变更参数生成
            const value = JSON.parse(JSON.stringify(tempGroup.value));
            const tempBaseData = {
                id: item.slotNum,
                url: item.url,
                baseValue: value,
                group: tempGroup
            };
            this.baseFormData.push(tempBaseData);
        });
    }

    // 当前控件触发 ngModelChange时，对同一fromGroup下的其他控件值做一次变更检测
    public updateOthersState(group: UntypedFormGroup, ctrlName) {
        const controls = group.controls;
        Object.keys(controls).forEach(key => {
            if (key !== ctrlName) {
                controls[key].markAllAsTouched();
                controls[key].updateValueAndValidity();
            }
        });
    }

    private computeState() {
        // 比较基础数据和变化后的数据是否相等
        const groupValues = this.getFormGroupValues(this.ipv6Form);
        const baseValue = this.baseFormData.map((item) => item.baseValue);
        const isChange = JSON.stringify(baseValue) !== JSON.stringify(groupValues);

        // 查看校验是否都是通过
        const formGroups = this.ipv6Form.map((item: IIPV6Table) => item.formGroup);
        const stateArr = formGroups.map((item: UntypedFormGroup) => item.pristine || item.valid);
        const isValid = stateArr.reduce((prev: boolean, next: boolean) => {
            return prev && next;
        });
        this.disabled = !(isChange && isValid);
    }

    // 获取formGroup组的值
    private getFormGroupValues(groups: { slotNum: string, formGroup: UntypedFormGroup }[]): { [key: string]: any }[] {
        const groupArr = groups.map(item => item.formGroup);
        return groupArr.map(group => group.value);
    }

    // switch开关变化时调用
    public changeIPState(fp: UntypedFormGroup, ctrlName: string): void {
        const value = fp.controls[ctrlName].value;
        const othersCtrlNames = Object.keys(fp.controls).filter(cName => {
            return cName !== ctrlName;
        });

        othersCtrlNames.forEach(cName => {
            if (value) {
                fp.controls[cName].disable();
            } else {
                fp.controls[cName].enable();
            }
            this.updateOthersState(fp, ctrlName);
        });

        this.computeBatchState();
    }

    // 计算批量设置按钮的禁用姿态
    private computeBatchState() {
        const dhcpValues = this.ipv6Form.map(form => {
            const switchName = form.dhcpControlName;
            return form.formGroup.value[switchName];
        });
        this.allSwitchState = dhcpValues.reduce((prev, current) => {
            return prev && current;
        });
    }

    // 保存IPV6配置
    public saveIpV6Config() {
        // 所有formGroup校验失败时，禁止保存，这里增加该项校验是因为避免用户手动修改保存按钮的状态，来跳过前面的校验检查
        const formGroups = this.ipv6Form.map((item: IIPV6Table) => item.formGroup);
        const stateArr = formGroups.map((item: UntypedFormGroup) => item.pristine || item.valid);
        const isValid = stateArr.reduce((prev: boolean, next: boolean) => {
            return prev && next;
        });
        if (!isValid) {
            return;
        }

        // 确定变化的数据项
        const changeArr = Object.keys(this.changeData);
        const requestArr = [];
        changeArr.forEach(key => {
            // 获取变化后的group值
            const curFormGroupValue = this.changeData[key].value;
            const url = this.baseFormData.filter(item => item.id === key)[0].url;
            const tempParam: { IPv6Addresses?: any[], IPv6DefaultGateway?: string } = { IPv6Addresses: [] };
            const param: any = {};
            // 如果是动态获取IP，则只需要封装1个参数，否则需要封装4个参数
            if (curFormGroupValue[IPAddressControlPrefix.DHCP + key]) {
                param.AddressOrigin = 'DHCPv6';
            } else {
                const baseFormVAlue = this.baseFormData.filter(baseForm => baseForm.id === key)[0].baseValue;
                const ipKey = IPAddressControlPrefix.IPADRESS + key;
                const gateKey = IPAddressControlPrefix.GATEWAY + key;
                const prefixKey = IPAddressControlPrefix.PREFIX + key;

                if (baseFormVAlue[ipKey] !== curFormGroupValue[ipKey]) {
                    param.Address = curFormGroupValue[ipKey];
                }
                if (baseFormVAlue[prefixKey] !== curFormGroupValue[prefixKey]) {
                    param.PrefixLength = parseInt(curFormGroupValue[prefixKey], 10);
                }
                if (baseFormVAlue[gateKey] !== curFormGroupValue[gateKey]) {
                    tempParam.IPv6DefaultGateway = curFormGroupValue[gateKey];
                }
            }
            if (Object.keys(param).length > 0) {
                tempParam.IPv6Addresses.push(param);
            } else {
                delete tempParam.IPv6Addresses;
            }
            requestArr.push(this.networkService.saveIPConfig(url, tempParam));
        });

        this.disabled = true;
        this.loading.state.next(true);
        forkJoin(requestArr).subscribe({
            next: (res) => {
                const errorIds = [];
                res.forEach((item: any) => {
                    if (item.error) {
                        const errorObjs = getMessageId(item.error);
                        errorIds.push(errorObjs);
                    }
                });
                if (errorIds.length > 0) {
                    const errorId = errorIds[0][0].errorId;
                    this.alertService.eventEmit.emit({ type: 'error', label: this.translate.instant(errorId).errorMessage });
                    if (errorId === 'BoardNotManagedByMM') {
                        const bladeID = /blade./.exec(errorIds[0][0].description)[0];
                        this.alertService.eventEmit.emit({
                            type: 'error',
                            label: this.translate.instant('EMM_SET_IP_ERR').replace('{blade}', bladeID)
                        });
                    }
                } else {
                    this.alertService.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    setTimeout(() => {
                        this.init();
                    }, 2000);
                }
            },
            complete: () => {
                this.loading.state.next(false);
            }
        });
    }

    // IPV6批量设置
    public openIpv6BatchSetting(): void {
        this.modal.open(Ipv6BatchSettingComponent, {
            id: 'ipv6BatchModal',
            context: {
                startInfo: (data: IIPV6Batch) => {
                    this.batchIpv6(data);
                },
                validateIpv6: this.validateIpv6
            },
            close: (modalRef: TiModalRef): void => {

            },
            dismiss: (modalRef: TiModalRef): void => {

            }
        });
    }

    // 展开详情
    public beforeToggle(row: TiTableRowData) {
        row.showDetails = !row.showDetails;
    }
    private batchIpv6(data: IIPV6Batch) {
        let temp = Object.assign({}, data);
        let count = 0;
        const activeFormGroup = this.ipv6Form.filter(item => {
            return !item.formGroup.get(item.dhcpControlName).value;
        });

        for (let i = 0; i < activeFormGroup.length; i++) {
            const item = activeFormGroup[i];
            const ipControl = item.formGroup.get(item.ipControlName);
            const maskControl = item.formGroup.get(item.prefixControlName);
            const gateControl = item.formGroup.get(item.gateControlName);

            if (i === 0) {
                ipControl.reset();
                maskControl.reset();
                gateControl.reset();
                ipControl.patchValue(data.ipAddress);
                maskControl.patchValue(data.prefix);
                gateControl.patchValue(data.gateWay);
                count++;
            } else {
                const nextIp = this.getNextValidIPv6(temp);
                // 如果返回得IP地址是有效的，则设置，否则直接退出, 后续的不再设置
                if (nextIp.ipAddress === '') {
                    break;
                } else {
                    ipControl.reset();
                    maskControl.reset();
                    gateControl.reset();
                    ipControl.patchValue(nextIp.ipAddress);
                    maskControl.patchValue(nextIp.prefix);
                    gateControl.patchValue(nextIp.gateWay);
                    temp = nextIp;
                    count++;
                }
            }
        }

        activeFormGroup.forEach(item => {
            item.formGroup.controls[item.ipControlName].markAsTouched();
            item.formGroup.controls[item.ipControlName].updateValueAndValidity();
        });

        // 若IP资源不足，则提示IP资源不足
        if (activeFormGroup.length > count) {
            this.alertService.eventEmit.emit({
                type: 'warn',
                label: this.translate.instant('IBMC_IP_RESOURCE_LACK')
            });
        }
    }
    // 根据当前IP增加下一个IP
    private getNextValidIPv6(data: IIPV6Batch): IIPV6Batch {
        const ipGroup: IIPV6Batch = {
            ipAddress: '',
            gateWay: '',
            prefix: ''
        };

        // 获取下一个有效的IP
        const validIp = this.getValidIP(data.ipAddress);
        if (validIp !== '') {
            ipGroup.ipAddress = validIp;
            ipGroup.gateWay = data.gateWay;
            ipGroup.prefix = data.prefix;
        }
        return ipGroup;
    }
    // 获取下一个有效IP
    private getValidIP(ip: string): string {
        const lenArr = this.lentghOfIps(ip);
        // 从最后一位开始检测
        const ipv6 =  IPv6Supplement(ip);
        let result = '';
        const ipArr = ipv6.split(':');
        for (let i = 0; i < ipArr.length; i++) {
            ipArr[i] = this.hex2int(ipArr[i]);
        }
        const ips = {};
        ips['ip0'] = parseInt(ipArr[0], 10);
        ips['ip1'] = parseInt(ipArr[1], 10);
        ips['ip2'] = parseInt(ipArr[2], 10);
        ips['ip3'] = parseInt(ipArr[3], 10);
        ips['ip4'] = parseInt(ipArr[4], 10);
        ips['ip5'] = parseInt(ipArr[5], 10);
        ips['ip6'] = parseInt(ipArr[6], 10);
        ips['ip7'] = parseInt(ipArr[7], 10);
        ips['ip7']++;
        // IP生成逻辑：第四位加1，如 > 255 ，则第三位加1，依次朝前类推
        if (ips['ip7'] > 0xffff) {
            ips['ip7'] = 0;
            ips['ip6']++;
        }
        if (ips['ip6'] > 0xffff) {
            ips['ip6'] = 0;
            ips['ip5']++;
        }
        if (ips['ip5'] > 0xffff) {
            ips['ip5'] = 0;
            ips['ip4']++;
        }
        if (ips['ip4'] > 0xffff) {
            ips['ip4'] = 0;
            ips['ip3']++;
        }
        if (ips['ip3'] > 0xffff) {
            ips['ip3'] = 0;
            ips['ip2']++;
        }

        if (ips['ip2'] > 0xffff) {
            ips['ip2'] = 0;
            ips['ip1']++;
        }

        if (ips['ip1'] > 0xffff) {
            ips['ip1'] = 0;
            ips['ip0']++;
        }

        if (ips['ip0'] > 0xffff) {
            result = '';
        } else {
            const nips = [];
            // 补全ip长度信息
            while (lenArr.length < 8) {
                const index = lenArr.findIndex((item) => item === 0);
                lenArr.splice(index, 0, 0);
            }
            // 还原全长度ipv6
            for (let i = 0; i < ipArr.length; i++) {
                nips.push(this.int2hex(ips[`ip${i}`], lenArr[i]));
            }
            // 组装ipv6字符串，相连多余:直接替换为::
            result = nips.join(':').replace(/:::*/, '::');
        }

        return result;
    }
    public hex2int(hexStr) {
        const len = hexStr.length;
        const arr = new Array(len);
        let charCode;
        for (let i = 0; i < len; i++) {
            charCode = hexStr.charCodeAt(i);
            if (48 <= charCode && charCode < 58) {
                charCode -= 48;
            } else {
                charCode = (charCode & 0xdf) - 65 + 10;
            }
            arr[i] = charCode;
        }
        return arr.reduce((acc, c) => {
            acc = 16 * acc + c;
            return acc;
        }, 0);
    }
    public int2hex(numberIp, width) {
        const hex = '0123456789abcdef';
        let str = '';
        while (numberIp) {
            str = hex.charAt(numberIp % 16) + str;
            numberIp = Math.floor(numberIp / 16);
        }
        if (typeof width === 'undefined' || width <= str.length) {
            return str;
        }
        let delta = width - str.length;
        let padding = '';
        while (delta-- > 0) {
            padding += '0';
        }
        return padding + str;
    }
    public lentghOfIps(ip: string) {
        const lenArr = [];
        const ipArr = ip.split(':');
        for (const item of ipArr) {
            lenArr.push(item.length);
        }
        return lenArr;
    }
    /**
     * IPv6地址的验证规则 ,校验的顺序: 是否为空->格式是否正确->是否是环回地址->是否是组播地址->是否是链路地址->是否与网关相等->是否与网关在同一网段
     * 1：不能为回环地址
     * 2：不能为组播地址
     * 3：不能为链路本地地址
     * 4：格式错误
     * 5：不能与ipv6网关地址相同
     * 6：ipv6必须同网关在同一网段
     */
    public validateIpv6(ipAddress1: string, prefixLenth1: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const ipV4Vaild = {
                error1: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_1'),
                error2: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_2'),
                error3: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_3'),
                error4: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
                error5: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_5'),
                error6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_6'),
                error7: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_7')
            };
            const errorModel = {
                validateIpv6: {
                    tiErrorMessage: 'error'
                }
            };
            const gateways = control.root.get(ipAddress1);
            const prefix = control.root.get(prefixLenth1);
            if (gateways === null || prefix === null) {
                return null;
            }

            const ipregexV4 = /^((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.){3}(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])$/;
            // 网关字符串
            const gateway1 = control.root.get(ipAddress1).value;
            // 前缀长度
            const prefixLenth = parseInt(control.root.get(prefixLenth1).value, 10);
            const ipValue = control.value;
            if (!ipValue) {
                return null;
            }
            // 当前值
            const ipv46Arr = ipValue.split(':');
            let ipv46Gateway;
            if (gateway1 && gateway1.indexOf(':') > -1) {
                ipv46Gateway = gateway1.split(':');
            }
            // 判断Ipv6末尾是否由IPv4组成,并转换二进制
            let binaryIpAdd;
            if (ipregexV4.test(ipv46Arr[ipv46Arr.length - 1])) {
                binaryIpAdd = convertToBinary(iPv6Translation(ipv46Arr));
            } else {
                // ip二进制
                binaryIpAdd = convertToBinary(restituteIpv6(ipValue));
            }

            // 判断默认网关末尾是否由IPv4组成,并转换二进制
            let binaryGateway;
            if (
                gateway1 && gateway1.indexOf(':') > -1
                && ipregexV4.test(ipv46Gateway[ipv46Gateway.length - 1])
            ) {
                binaryGateway = convertToBinary(iPv6Translation(ipv46Gateway));
            } else {
                // 网关二进制
                binaryGateway = convertToBinary(restituteIpv6(gateway1));
            }

            // 未指定的地址
            const emptyAddr = convertToBinary(restituteIpv6('0::0'));
            if (binaryIpAdd === emptyAddr) {
                errorModel.validateIpv6.tiErrorMessage = ipV4Vaild.error7;
                return errorModel;
            }

            // 1:环回地址,0::1
            const loopBackAddr = convertToBinary(restituteIpv6('0::1'));
            if (binaryIpAdd === loopBackAddr) {
                errorModel.validateIpv6.tiErrorMessage = ipV4Vaild.error1;
                return errorModel;
            }
            if (ipValue === '') {
                return null;
            }

            /*
            * 组播地址
            * 判断末尾是否由IPv4组成
            */
            let restituteIpv6Str;
            if (ipregexV4.test(ipv46Arr[ipv46Arr.length - 1])) {
                restituteIpv6Str = iPv6Translation(ipv46Arr);
            } else {
                restituteIpv6Str = restituteIpv6(ipValue);
            }
            if (restituteIpv6Str.toUpperCase().indexOf('FF') === 0) {
                errorModel.validateIpv6.tiErrorMessage = ipV4Vaild.error2;
                return errorModel;
            }

            // 不能为链路本地地址
            if (
                restituteIpv6Str.toUpperCase().indexOf('FE8') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FE9') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FEA') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FEB') === 0
            ) {
                errorModel.validateIpv6.tiErrorMessage = ipV4Vaild.error3;
                return errorModel;
            }

            // 如果网关地址为空，则返回true
            if (!gateway1) {
                return null;
            }
            // 不能与网关地址相同
            if (binaryIpAdd === binaryGateway) {
                errorModel.validateIpv6.tiErrorMessage = ipV4Vaild.error5;
                return errorModel;
            }
            // ipv6必须同网关在同一网段
            if (binaryIpAdd.substr(0, prefixLenth) !== binaryGateway.substr(0, prefixLenth)) {
                errorModel.validateIpv6.tiErrorMessage = ipV4Vaild.error6;
                return errorModel;
            }
        };
    }
}
