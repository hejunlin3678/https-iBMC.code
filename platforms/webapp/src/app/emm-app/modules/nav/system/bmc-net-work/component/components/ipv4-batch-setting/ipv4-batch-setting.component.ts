import { Component, OnInit, Input } from '@angular/core';
import { UntypedFormBuilder, UntypedFormGroup, Validators } from '@angular/forms';
import { TiValidationConfig, TiValidators } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { IpV4Vaild } from 'src/app/common-app/modules/manager/network/model/valid';
import { IIPV4Batch, IPAddressControlPrefix } from '../../model';

@Component({
    selector: 'app-ipv4-batch-setting',
    templateUrl: './ipv4-batch-setting.component.html',
    styleUrls: ['./ipv4-batch-setting.component.scss']
})
export class Ipv4BatchSettingComponent implements OnInit {
    @Input() startInfo;
    public disabled: boolean = true;
    public ipv4BatchForm: UntypedFormGroup;

    public ipv4AddressValidator: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            'required': '',
            'pattern': this.translate.instant('IBMC_INVALID_IPADDRESS'),
            'firstIP': this.translate.instant('IBMC_INVALID_IPADDRESS'),
            'isEqualAddress': this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };

    public ipV4MaskValidation: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            'required': '',
            'pattern': this.translate.instant('IBMC_SUB_NET_MASK'),
            'filtrationIP': this.translate.instant('IBMC_SUB_NET_MASK')
        }
    };

    public ipV4GateWayValidation: TiValidationConfig = {
        type: 'change',
        errorMessage: {
            'required': '',
            'isEqualAddress': this.translate.instant('IBMC_IPV4_ADDRES_ERROR')
        }
    };

    constructor(
        private fb: UntypedFormBuilder,
        private translate: TranslateService
    ) { }

    ngOnInit(): void {
        this.init();
    }

    // 初始化表单控件
    private init() {
        this.ipv4BatchForm = this.fb.group({
            'ipBatch': [null, [
                TiValidators.required,
                Validators.pattern(/^(22[0-3]|2[0-1][0-9]|1\d\d|[1-9]\d|[1-9])\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)$/),
                IpV4Vaild.firstIP(),
                IpV4Vaild.isInSameNetSegment(IPAddressControlPrefix.GATEWAY, IPAddressControlPrefix.SUBNETMASK)]],
            'maskBatch': [null, [
                TiValidators.required,
                Validators.pattern(/^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/),
                IpV4Vaild.filtrationIP()
            ]],
            'gateBatch': [null, [
                TiValidators.required,
                IpV4Vaild.isInSameNetSegment(IPAddressControlPrefix.IPADRESS, IPAddressControlPrefix.SUBNETMASK)
            ]]
        });

        const controls = this.ipv4BatchForm.controls;
        Object.keys(controls).forEach(key => {
            controls[key].valueChanges.subscribe({
                next: (value) => {
                    this.computedFormState();
                }
            });
        });
    }

    public emitData() {
        const startInfo: IIPV4Batch = {
            ipAddress: this.ipv4BatchForm.controls.ipBatch.value,
            subnetMask: this.ipv4BatchForm.controls.maskBatch.value,
            gateWay: this.ipv4BatchForm.controls.gateBatch.value
        };
        this.startInfo(startInfo);
        this.close();
    }

    /**
     * 输入IP时，自动填充子网掩码
     * 填充规则，根据IP地址的类别，设置默认的子网掩码
     * @param ip
     */
    private setMaskDefaultValue(ip: string): void {
        const ipArr: string[] = ip.split('.');
        // 如果有一个为空，则不设置值
        const flag = ipArr.some(item => item === '');
        if (flag) {
            return;
        }

        const first = parseInt(ipArr[0], 10);
        let defaultMask = '';
        if (first > 0 && first < 128) {
            defaultMask = '255.0.0.0';
        }

        if (first >= 128 && first < 192) {
            defaultMask = '255.255.0.0';
        }

        if (first >= 192 && first <= 223) {
            defaultMask = '255.255.255.0';
        }

        this.ipv4BatchForm.controls.maskBatch.patchValue(defaultMask);
    }

    // 计算Form表单的有效性，包括非空和IP匹配
    public computedFormState(): void {
        const controls = this.ipv4BatchForm.controls;
        const ipv4Control = controls.ipBatch;
        const subnetMaskControl = controls.maskBatch;
        const gateWayControl = controls.gateBatch;
        const isValidIp = this.checkIsValidIp(ipv4Control.value);
        const isValidMask = this.checkSubNetMaskIsValid(ipv4Control.value, subnetMaskControl.value);
        const isValidGateWay = this.checkGateWayIsValid(ipv4Control.value, subnetMaskControl.value, gateWayControl.value);

        this.disabled = !(isValidIp && isValidMask && isValidGateWay);
    }

    // 计算IP是否为有效的IPV4地址
    private checkIsValidIp(ip: string = ''): boolean {
        const rep = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
        return rep.test(ip);
    }

    /**
     * 计算子网掩码是否有效
     * 子网掩码的默认值;
     * A类：255.0.0.0，A类地址范围：1.0.0.0 - 126.255.255.255，其中0和127作为特殊地址。
     * B类：255.255.0.0 B类地址范围：128.0.0.0 - 191.255.255.255。
     * C类：255.255.255.0 C类地址范围：192.0.0.0 - 223.255.255.255。
     * D类：D类地址不分网络地址和主机地址，D类地址范围：224.0.0.0 - 239.255.255.255。
     * E类：E类地址也不分网络地址和主机地址，E类地址范围：240.0.0.0 - 255.255.255.255。
     * @param ip
     * @param mask
     */
    private checkSubNetMaskIsValid(ip: string, mask: string): boolean {
        // 默认的子网掩码值
        let staticMask = [];
        const exp = /^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/;
        const maskTypeA = /^(\d|[1-9]\d|1[0-1]\d|12[0-7])(\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])){3}$/;
        const maskTypeB = /^(12[8-9]|1[3-8]\d|19[0-1])(\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])){3}$/;
        const operatorResult = [];

        // 掩码地址不符合IP地址规范
        if (!exp.test(mask)) {
            return false;
        }

        // 根据IP地址的类别，判定掩码是否有效
        if (maskTypeA.test(ip)) {
            staticMask = [255, 0, 0, 0];
        } else if (maskTypeB.test(ip)) {
            staticMask = [255, 255, 0, 0];
        } else {
            staticMask = [255, 255, 255, 0];
        }

        const maskArr = mask.split('.');
        let isMaskInvalid = false;
        // 将当前掩码的每一位与默认掩码的对应位做与运算，如果结果与默认的掩码不一致，则当前的掩码不符合要求
        operatorResult.push(parseInt(staticMask[0], 10) & parseInt(maskArr[0], 10));
        operatorResult.push(parseInt(staticMask[1], 10) & parseInt(maskArr[1], 10));
        operatorResult.push(parseInt(staticMask[2], 10) & parseInt(maskArr[2], 10));
        operatorResult.push(parseInt(staticMask[3], 10) & parseInt(maskArr[3], 10));
        operatorResult.forEach((result, index) => {
            if (result !== staticMask[index]) {
                isMaskInvalid = true;
            }
        });

        return !isMaskInvalid;
    }

    // 计算默认网关是否合法, IP&掩码 和 网关&掩码 结果是否相同，不相同则不在同一网段
    private checkGateWayIsValid(ip: string, mask: string, gateway: string): boolean {
        const ipExp = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
        const maskExp = /^(254|252|248|240|224|192|128|0)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)$/;
        if (!ipExp.test(ip) || !maskExp.test(mask) || !ipExp.test(gateway)) {
            return false;
        }

        const staticIpArr = ip.split('.');
        const staticMaskArr = mask.split('.');
        const staticGateWayArr = gateway.split('.');
        const resultIp = [];
        const resultGateWay = [];

        resultIp.push(parseInt(staticIpArr[0], 10) & parseInt(staticMaskArr[0], 10));
        resultIp.push(parseInt(staticIpArr[1], 10) & parseInt(staticMaskArr[1], 10));
        resultIp.push(parseInt(staticIpArr[2], 10) & parseInt(staticMaskArr[2], 10));
        resultIp.push(parseInt(staticIpArr[3], 10) & parseInt(staticMaskArr[3], 10));

        resultGateWay.push(parseInt(staticGateWayArr[0], 10) & parseInt(staticMaskArr[0], 10));
        resultGateWay.push(parseInt(staticGateWayArr[1], 10) & parseInt(staticMaskArr[1], 10));
        resultGateWay.push(parseInt(staticGateWayArr[2], 10) & parseInt(staticMaskArr[2], 10));
        resultGateWay.push(parseInt(staticGateWayArr[3], 10) & parseInt(staticMaskArr[3], 10));

        if (resultIp.join() !== resultGateWay.join()) {
            return false;
        }
        return true;
    }


    // close和dismiss2个方法必须得写，保持空函数就可以，否则，弹出框将无法被关闭
    public close(): void {
    }

    public dismiss(): void {
    }

}
