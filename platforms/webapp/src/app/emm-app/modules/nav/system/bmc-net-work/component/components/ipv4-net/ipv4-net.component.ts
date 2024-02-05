import { Component, OnInit, ViewChild, ViewContainerRef, ElementRef, Input, SimpleChanges } from '@angular/core';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiModalService, TiModalRef, TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { NetworkService } from '../../network.service';
import { IPV4Address, IIpv4Group } from '../../model';
import { DhcpConfigComponent } from '../dhcp-config/dhcp-config.component';
import { Ipv4BatchSettingComponent } from '../ipv4-batch-setting/ipv4-batch-setting.component';
import { UntypedFormGroup, UntypedFormBuilder, UntypedFormControl, Validators } from '@angular/forms';
import { IpV4Vaild } from 'src/app/common-app/modules/manager/network/model/valid';
import { IPAddressControlPrefix, IIPV4Table } from '../../model';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { TranslateService } from '@ngx-translate/core';
import { AlertMessageService } from 'src/app/common-app/service';
import { forkJoin } from 'rxjs';
import { MultVaild } from 'src/app/common-app/utils/multValid';
import { getMessageId } from 'src/app/common-app/utils/common';

@Component({
    selector: 'app-ipv4-net',
    templateUrl: './ipv4-net.component.html',
    styleUrls: ['./ipv4-net.component.scss']
})
export class Ipv4NetComponent implements OnInit {
    @ViewChild('dhcpConfig', { read: ViewContainerRef, static: true }) dhcpConfig: ViewContainerRef;
    @ViewChild('tip', { read: true, static: true }) tip: ElementRef;
    @Input() slotList;
    @Input() managerId;

    public allSwitchState: boolean = false;
    private conflictIps = [];
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public columns: TiTableColumns[] = [
        {
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '10%'
        },
        {
            title: 'IBMC_IPV4_ADDRESS',
            width: '25%'
        },
        {
            title: 'IBMC_IPV4_SUBNET_MASK',
            width: '25%'
        },
        {
            title: 'IBMC_IP_ADDRESS03',
            width: '25%'
        },
        {
            title: 'IBMC_DHCP_MODEL',
            width: '20%'
        }
    ];
    // 按钮的禁用状态，初始为true
    public disabled: boolean = true;

    // 表单数据
    public ipv4Form: IIPV4Table[] = [];
    private baseFormData: { id: string, url: string, baseValue: any[], group: UntypedFormGroup }[] = [];
    private changeData: { [key: string]: any } = {};

    // 校验提示信息
    public ipV4AddressValidation: TiValidationConfig = {
        type: 'change',
        tip: this.translate.instant('IBMC_IPV4_RANGES'),
        errorMessage: {
            'pattern': this.translate.instant('IBMC_INVALID_IPADDRESS'),
            'firstIP': this.translate.instant('IBMC_INVALID_IPADDRESS'),
            'isEqualAddress': this.translate.instant('IBMC_IPV4_ADDRES_ERROR'),
            'conflict': this.translate.instant('IBMC_IP_CONFLICT')
        }
    };

    public ipV4MaskValidation: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            'pattern': this.translate.instant('IBMC_SUB_NET_MASK'),
            'filtrationIP': this.translate.instant('IBMC_SUB_NET_MASK')
        }
    };

    public ipV4GateWayValidation: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            'isEqualAddress': this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };

    // IP,掩码，网关校验
    private ipv4AddressValidator = [
        TiValidators.required,
        Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
        IpV4Vaild.firstIP(),
        IpV4Vaild.isInSameNetSegment(IPAddressControlPrefix.GATEWAY, IPAddressControlPrefix.SUBNETMASK)
    ];
    private ipv4SubnetValidator = [
        TiValidators.required,
        Validators.pattern(/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/),
        IpV4Vaild.filtrationIP()
    ];
    private ipv4GatewayValidator = [
        TiValidators.required,
        IpV4Vaild.isInSameNetSegment(IPAddressControlPrefix.IPADRESS, IPAddressControlPrefix.SUBNETMASK)
    ];

    public component: any = DhcpConfigComponent;
    // DHCP模式全部开启和全部关闭
    public tipContext: any = {
        outputs: {
            activeChange: (active: boolean) => {
                const method = active ? 'enable' : 'disable';
                this.ipv4Form.forEach(form => {
                    const ipName = form.ipControlName;
                    const gateName = form.gateControlName;
                    const maskName = form.maskControlName;
                    form.formGroup.controls[ipName][method]();
                    form.formGroup.controls[gateName][method]();
                    form.formGroup.controls[maskName][method]();
                    form.formGroup.controls[form.dhcpControlName].patchValue(active);
                });
                this.computeBatchState();
            }
        }
    };

    constructor(
        private networkService: NetworkService,
        private modal: TiModalService,
        private loading: LoadingService,
        private fb: UntypedFormBuilder,
        private translate: TranslateService,
        private alertService: AlertMessageService
    ) { }

    ngOnInit(): void {

    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.slotList.currentValue) {
            this.initIpv4Data(changes.slotList.currentValue.ips);
            this.getConflictIp();
        }
    }

    public init() {
        this.initIpv4Data(this.slotList.ips);
        this.getConflictIp();
    }

    public getConflictIp() {
        forkJoin([
            this.networkService.getCurrentDeviceIp({ board: 'HMM1', type: 'ipv4', ipType: 'static' }),
            this.networkService.getCurrentDeviceIp({ board: 'HMM2', type: 'ipv4', ipType: 'static' }),
            this.networkService.getCurrentDeviceIp({ board: this.managerId, type: 'ipv4', ipType: 'float' }),
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

    // 跳转到Blade登录页
    public openBladeLogin(url: string) {
        window.open('https://' + url, '_blank');
    }

    // 获取所有IPV4设备
    public initIpv4Data(slotIdArr) {
        this.loading.state.next(true);
        this.changeData = {};
        this.baseFormData = [];
        if (slotIdArr.length > 0) {
            this.networkService.getIPV4Info(slotIdArr).subscribe({
                next: (res: IPV4Address[]) => {
                    this.initForms(res);
                    this.srcData = {
                        data: this.ipv4Form,
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
    private initForms(ipv4Datas: IPV4Address[]) {
        this.disabled = true;
        this.ipv4Form = [];
        ipv4Datas.forEach((item: IPV4Address, index: number) => {

            const ipControl = new UntypedFormControl({ value: item.address, disabled: item.dhcpModel });
            const maskControl = new UntypedFormControl({ value: item.subnetMask, disabled: item.dhcpModel });
            const gateControl = new UntypedFormControl({ value: item.gateWay, disabled: item.dhcpModel });
            const dhcpControl = new UntypedFormControl(item.dhcpModel);
            const tempGroup = this.fb.group({});

            tempGroup.addControl(item.ipControlName, ipControl);
            tempGroup.addControl(item.maskControlName, maskControl);
            tempGroup.addControl(item.gateControlName, gateControl);
            tempGroup.addControl(item.dhcpControlName, dhcpControl);

            tempGroup.valueChanges.subscribe(() => {
                this.recordChange(tempGroup);
                this.computeState();
            });

            const param: IIPV4Table = {
                slotNum: item.slotNum,
                linkUrl: item.address,
                url: item.url,
                ipControlName: item.ipControlName,
                maskControlName: item.maskControlName,
                gateControlName: item.gateControlName,
                dhcpControlName: item.dhcpControlName,
                formGroup: tempGroup
            };
            this.ipv4Form.push(param);

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
            const ipValidator = [...this.ipv4AddressValidator];
            const maskValidator = [...this.ipv4SubnetValidator];
            const gateValidator = [...this.ipv4GatewayValidator];
            if (key.includes('ipBlade')) {
                 ipValidator.push(MultVaild.conflictIp(this.ipv4Form, key, this.conflictIps));
                 controls[key].setValidators(ipValidator);
            }
            if (key.includes('maskBlade')) {
                controls[key].setValidators(maskValidator);
            }
            if (key.includes('gateBlade')) {
                controls[key].setValidators(gateValidator);
            }
            if (key !== ctrlName) {
                controls[key].markAllAsTouched();
                controls[key].updateValueAndValidity();
            }
        });
    }

    // 计算保存按钮的状态
    private computeState() {
        // 比较基础数据和变化后的数据是否相等
        const groupValues = this.getFormGroupValues(this.ipv4Form);
        const baseValue = this.baseFormData.map((item) => item.baseValue);
        const isChange = JSON.stringify(baseValue) !== JSON.stringify(groupValues);

        // 查看校验是否都是通过
        const formGroups = this.ipv4Form.map((item: IIPV4Table) => item.formGroup);
        const stateArr = formGroups.map((item: UntypedFormGroup) => item.valid);
        const isValid = stateArr.reduce((prev: boolean, next: boolean) => {
            return prev && next;
        });
        this.disabled = !(isChange && isValid);
    }

    // 计算批量设置按钮的状态
    private computeBatchState() {
        const dhcpValues = this.ipv4Form.map(form => {
            const switchName = form.dhcpControlName;
            return form.formGroup.value[switchName];
        });
        this.allSwitchState = dhcpValues.reduce((prev, current) => {
            return prev && current;
        });
    }

    // 获取formGroup组的值
    private getFormGroupValues(groups: { slotNum: string, formGroup: UntypedFormGroup }[]): { [key: string]: any }[] {
        const groupArr = groups.map(item => item.formGroup);
        return groupArr.map(group => group.value);
    }

    // 获取formControl的错误
    public getFirstError(formContrl: UntypedFormControl): { show: boolean, message: string } {
        const result: any = {
            show: false,
            message: ''
        };

        if (formContrl.errors) {
            result.show = true;
            result.message = Object.values(formContrl.errors)[0].tiErrorMessage;
        }

        return result;
    }

    // IPV4批量设置
    public openIpv4BatchSetting(): void {
        this.modal.open(Ipv4BatchSettingComponent, {
            id: 'ipv4BatchModal',
            context: {
                startInfo: (data: IIpv4Group) => {
                    this.batchIpv4(data);
                }
            },
            close: (modalRef: TiModalRef): void => {
                // 空方法保留，匹配框架格式
            },
            dismiss: (modalRef: TiModalRef): void => {
                // 空方法保留，匹配框架格式
            }
        });
    }

    private batchIpv4(data: IIpv4Group) {
        let temp = Object.assign({}, data);
        let count = 0;
        const activeFormGroup = this.ipv4Form.filter(item => {
            return !item.formGroup.get(item.dhcpControlName).value;
        });

        for (let i = 0; i < activeFormGroup.length; i++) {
            const item = activeFormGroup[i];
            const ipControl = item.formGroup.get(item.ipControlName);
            const maskControl = item.formGroup.get(item.maskControlName);
            const gateControl = item.formGroup.get(item.gateControlName);

            if (i === 0) {
                ipControl.reset();
                maskControl.reset();
                gateControl.reset();
                ipControl.patchValue(data.ipAddress);
                maskControl.patchValue(data.subnetMask);
                gateControl.patchValue(data.gateWay);
                count++;
            } else {
                const nextIp = this.getNextValidIPv4(temp);
                // 如果返回得IP地址是有效的，则设置，否则直接退出, 后续的不再设置
                if (nextIp.ipAddress === '') {
                    break;
                } else {
                    ipControl.reset();
                    maskControl.reset();
                    gateControl.reset();
                    ipControl.patchValue(nextIp.ipAddress);
                    maskControl.patchValue(nextIp.subnetMask);
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
    private getNextValidIPv4(data: IIpv4Group): IIpv4Group {
        const ipGroup: IIpv4Group = {
            ipAddress: '',
            gateWay: '',
            subnetMask: ''
        };

        // 获取下一个有效的IP
        const validIp = this.getValidIP(data.ipAddress);
        if (validIp !== '') {
            ipGroup.ipAddress = validIp;
            ipGroup.gateWay = data.gateWay;
            ipGroup.subnetMask = data.subnetMask;
        }
        return ipGroup;
    }

    // 获取下一个有效IP
    private getValidIP(ip: string): string {
        const rep = /^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/;
        if (!rep.test(ip)) {
            return '';
        }
        // 从最后一位开始检测
        let result = '';
        const ipArr = ip.split('.');
        let ip0 = parseInt(ipArr[0], 10);
        let ip1 = parseInt(ipArr[1], 10);
        let ip2 = parseInt(ipArr[2], 10);
        let ip3 = parseInt(ipArr[3], 10);
        ip3++;
        // IP生成逻辑：第四位加1，如 > 255 ，则第三位加1，依次朝前类推
        if (ip3 > 255) {
            ip3 = 0;
            ip2++;
        }

        if (ip2 > 255) {
            ip2 = 0;
            ip1++;
        }

        if (ip1 > 255) {
            ip1 = 0;
            ip0++;
        }

        // 若IP地址第一位为0或127或大于223，则是无效地址，需重新计算
        if (ip0 === 0) {
            result = '1.0.0.0';
        } else if (ip0 === 127) {
            result = '128.0.0.0';
        } else if (ip0 > 223) {
            result = '';
        } else {
            result = `${ip0}.${ip1}.${ip2}.${ip3}`;
        }

        return result;
    }

    // 保存IPV4配置
    public saveIpV4Config() {
        // 所有formGroup校验失败时，禁止保存，这里增加该项校验是因为避免用户手动修改保存按钮的状态，来跳过前面的校验检查
        const formGroups = this.ipv4Form.map((item: IIPV4Table) => item.formGroup);
        const stateArr = formGroups.map((item: UntypedFormGroup) => item.valid);
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
            const tempParam: { IPv4Addresses: any[] } = { IPv4Addresses: [] };
            const param: any = {};
            // 如果是动态获取IP，则只需要封装1个参数，否则需要封装4个参数
            if (curFormGroupValue[IPAddressControlPrefix.DHCP + key]) {
                param.AddressOrigin = 'DHCP';
            } else {
                const baseFormVAlue = this.baseFormData.filter(baseForm => baseForm.id === key)[0].baseValue;
                const ipKey = IPAddressControlPrefix.IPADRESS + key;
                const maskKey = IPAddressControlPrefix.SUBNETMASK + key;
                const gateKey = IPAddressControlPrefix.GATEWAY + key;
                const dhcpKey = IPAddressControlPrefix.DHCP + key;

                if (baseFormVAlue[ipKey] !== curFormGroupValue[ipKey]) {
                    param.Address = curFormGroupValue[ipKey];
                }
                if (baseFormVAlue[maskKey] !== curFormGroupValue[maskKey]) {
                    param.SubnetMask = curFormGroupValue[maskKey];
                }
                if (baseFormVAlue[gateKey] !== curFormGroupValue[gateKey]) {
                    param.Gateway = curFormGroupValue[gateKey];
                }
                if (baseFormVAlue[dhcpKey] !== curFormGroupValue[dhcpKey]) {
                    param.AddressOrigin = 'Static';
                }
            }
            tempParam.IPv4Addresses.push(param);
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

    // 开关状态变化时，禁用、启用IP输入
    public changeIPState(fp: UntypedFormGroup, ctrlName: string) {
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
    }
}
