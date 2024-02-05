import { AbstractControl, ValidatorFn, ValidationErrors } from '@angular/forms';
import { convertToBinary, iPv6Translation, restituteIpv6 } from 'src/app/common-app/utils';
import { NetworkService } from '../services';

export class IpV4Vaild {
    public static error1 = '';
    public static error2 = '';
    public static error3 = '';
    public static error4 = '';
    public static error5 = '';
    public static error6 = '';
    public static error7 = '';
    public static error8 = '';
    public static error9 = '';
    constructor(
      private service: NetworkService
    ) {
        IpV4Vaild.error1 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_1;
        IpV4Vaild.error2 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_2;
        IpV4Vaild.error3 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_3;
        IpV4Vaild.error4 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_4;
        IpV4Vaild.error5 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_5;
        IpV4Vaild.error6 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_6;
        IpV4Vaild.error7 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_7;
        IpV4Vaild.error8 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_8;
        IpV4Vaild.error9 = this.service.i18n.IBMC_IPV6_ADDRES_ERROR_9;
    }
    // 过滤开头和结尾是连字符的 “-”
    public static hyphenation():  ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                hyphenation: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (value === null) {
                return null;
            }
            return (value[0] === '-') || (value[value.length - 1] === '-') ? errorModel : null;
        };
    }
    // IPv4过滤127开头的IP
    public static firstIP():  ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                firstIP: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (value === null) {
                return null;
            }
            const firstIP = value.split('.')[0];
            return firstIP !== '127' ? null : errorModel;
        };
    }
    // 掩码过滤0.0.0.0
    public static filtrationIP():  ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                filtrationIP: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            return value !== '0.0.0.0' ? null : errorModel;
        };
    }
    // 校验IP是否和网关在同一网段
    public static isEqualAddress(ipAddress1: string, ipAddress2: string):  ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                isEqualAddress: {
                    tiErrorMessage: 'error'
                }
            };
            const ip1 = control.root.get(ipAddress1);
            const ip2 = control.root.get(ipAddress2);
            if (ip1 === null || ip2 === null) {
                return null;
            }
            let addr1 = control.value;
            let addr2 = ip1.value;
            let mask = ip2.value;
            if (!addr1 || !addr2 || !mask) {
                return errorModel;
            }
            const res1 = [];
            const res2 = [];
            addr1 = addr1.split('.');
            addr2 = addr2.split('.');
            mask = mask.split('.');
            for (let i = 0; i < addr1.length; i++) {
                res1.push(parseInt(addr1[i], 10) & parseInt(mask[i], 10));
                res2.push(parseInt(addr2[i], 10) & parseInt(mask[i], 10));
            }
            if (res1.join('.') === res2.join('.')) {
                return null;
            } else {
                return errorModel;
            }
        };
    }
    // 校验IPv6,前缀长度是否可设置为空
    public static maskCode(modelV6: string, maskCode: string, ipAddress: string, selectedMode: string):  ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                maskCode: {
                    tiErrorMessage: 'error'
                }
            };
            const ip = control.root.get(ipAddress);
            const mode = control.root.get(selectedMode);
            if (modelV6 === null || maskCode === null || ip === null || mode === null) {
                return null;
            }
            const maskValue = control.value;
            const ip1 = ip.value;
            const mode1 = mode.value;
            if (modelV6 === 'Static' && (maskCode === null || maskCode === '') && mode1 !== 'IPv6') {
                if (ip1 !== ''  && ip1 !== null  && maskValue === '') {
                    return errorModel;
                } else {
                    return null;
                }
            } else {
                if (maskValue === '' || maskValue === null) {
                    return errorModel;
                } else {
                    return null;
                }
            }
        };
    }

    // 默认网关是否可设置为空
    public static gateway(modelV6: string, ipAddress: string, selectedMode: string):  ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                gateway: {
                    tiErrorMessage: 'error'
                }
            };

            const mode = control.root.get(selectedMode);
            if (modelV6 === null || ipAddress === null || mode === null) {
                return null;
            }
            const gatewayValue = control.value;
            const mode1 = mode.value;

            if (modelV6 === 'Static' && (ipAddress === null || ipAddress === '') && mode1 !== 'IPv6') {
                return null;
            } else {
                if (gatewayValue === '' || gatewayValue === null) {
                    return errorModel;
                } else {
                    return null;
                }
            }
        };
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
    public validateIpv6(ipAddress1: string, prefixLenth1: string, model?: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
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
                errorModel.validateIpv6.tiErrorMessage = IpV4Vaild.error7;
                return errorModel;
            }

            // 1:环回地址,0::1
            const loopBackAddr = convertToBinary(restituteIpv6('0::1'));
            if (binaryIpAdd === loopBackAddr) {
                errorModel.validateIpv6.tiErrorMessage = IpV4Vaild.error1;
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
                errorModel.validateIpv6.tiErrorMessage = IpV4Vaild.error2;
                return errorModel;
            }

            // 不能为链路本地地址
            if (
                restituteIpv6Str.toUpperCase().indexOf('FE8') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FE9') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FEA') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FEB') === 0
            ) {
                errorModel.validateIpv6.tiErrorMessage = IpV4Vaild.error3;
                return errorModel;
            }

            // 如果网关地址为空，则返回true
            if (!gateway1) {
                return null;
            }
            // 不能与网关地址相同
            if (binaryIpAdd === binaryGateway) {
                errorModel.validateIpv6.tiErrorMessage = IpV4Vaild.error5;
                return errorModel;
            }
            // ipv6必须同网关在同一网段
            if (binaryIpAdd.substr(0, prefixLenth) !== binaryGateway.substr(0, prefixLenth)) {
                errorModel.validateIpv6.tiErrorMessage = IpV4Vaild.error6;
                if (model && model === 'DHCP') {
                    return null;
                }
                return errorModel;
            }
        };
    }

    /**
     * 网关校验顺序： 是否为空->如果IP为空，则校验通过->网关和IP是否相等->网关和IP是否在同一网段
     * 网关校验规则
     * 1：地址合法性
     * 2：不能与IPv6地址相同
     * 4：必须同ipv6地址在同一网段
     */
    public gatewayIpv6(ipAddress1: string, prefixLenth1: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                gatewayIpv6: {
                    tiErrorMessage: 'error'
                }
            };
            const ip = control.root.get(ipAddress1);
            const prefix = control.root.get(prefixLenth1);
            if (ip === null || prefix === null) {
                return null;
            }

            const ipregexV4 = /^((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.){3}(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])$/;
            // ip字符串
            const ip1 = control.root.get(ipAddress1).value;
            // 当前值
            const gatewayValue = control.value;
            if (gatewayValue === null) {
                return null;
            }
            const ipv46Gateway = gatewayValue.split(':');

            let ipv4And6;
            if (ip1 && ip1.indexOf(':') > -1) {
                ipv4And6 = ip1.split(':');
            }
            let binaryIpAdd;

            // 判断默认网关末尾是否由IPv4组成
            if (ipregexV4.test(ipv46Gateway[ipv46Gateway.length - 1])) {
                binaryIpAdd = convertToBinary(iPv6Translation(ipv46Gateway));
            } else {
                // 网关二进制
                binaryIpAdd = convertToBinary(restituteIpv6(gatewayValue));
            }

            let binaryIp;
            if (ip1 && ip1.indexOf(':') > -1 && ipregexV4.test(ipv4And6[ipv4And6.length - 1])) {
                binaryIp = convertToBinary(iPv6Translation(ipv4And6));
            } else {
                // ip二进制
                binaryIp = convertToBinary(restituteIpv6(ip1));
            }

            // 如果ip地址为空，则返回true,不做校验
            if (!ip1 || !gatewayValue) {
                return null;
            }

            // 根据补全的IP和前缀长度，判断IP和网关是不是在一个网段内
            // 前缀长度
            const prefixLenth = parseInt(control.root.get(prefixLenth1).value, 10);
            let networkNOIp;
            // 判断IP
            if (ip1 && ip1.indexOf(':') > -1 && ipregexV4.test(ipv4And6[ipv4And6.length - 1])) {
                networkNOIp = convertToBinary(iPv6Translation(ipv4And6)).substr(0, prefixLenth);
            } else {
                networkNOIp = convertToBinary(restituteIpv6(ip1)).substr(0, prefixLenth);
            }

            // 判断默认网关
            let networkNoGateway;
            if (ipregexV4.test(ipv46Gateway[ipv46Gateway.length - 1])) {
                networkNoGateway = convertToBinary(iPv6Translation(ipv46Gateway)).substr(0, prefixLenth);
            } else {
                networkNoGateway = convertToBinary(restituteIpv6(gatewayValue)).substr(0, prefixLenth);
            }
            // 网关和ip相同
            if (binaryIp === binaryIpAdd) {
                errorModel.gatewayIpv6.tiErrorMessage = IpV4Vaild.error5;
                return errorModel;
            }

            // ip和网关网段不在同一网段
            if (networkNOIp !== networkNoGateway) {
                errorModel.gatewayIpv6.tiErrorMessage = IpV4Vaild.error6;
                return errorModel;
            }

            // 网关为未指定地址、环回地址、组播地址 链路地址
            const emptyGateway = convertToBinary(restituteIpv6('0::0'));
            if (binaryIpAdd === emptyGateway) {
                errorModel.gatewayIpv6.tiErrorMessage = IpV4Vaild.error7;
                return errorModel;
            }

            const loopBackAddr = convertToBinary(restituteIpv6('0::1'));
            if (binaryIpAdd === loopBackAddr) {
                errorModel.gatewayIpv6.tiErrorMessage = IpV4Vaild.error1;
                return errorModel;
            }

            const restituteIpv6Str = restituteIpv6(gatewayValue);
            if (restituteIpv6Str.toUpperCase().indexOf('FF') === 0) {
                errorModel.gatewayIpv6.tiErrorMessage = IpV4Vaild.error2;
                return errorModel;
            }

            if (
                restituteIpv6Str.toUpperCase().indexOf('FE8') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FE9') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FEA') === 0
                || restituteIpv6Str.toUpperCase().indexOf('FEB') === 0
            ) {
                errorModel.gatewayIpv6.tiErrorMessage = IpV4Vaild.error3;
                return errorModel;
            }
            return null;
        };
    }

    // 校验相同冲突ip
    public testIpSame(ipAddress1: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                testIpSame: {
                    tiErrorMessage: 'error'
                }
            };
            const ip = control.root.get(ipAddress1);
            if (ip === null || ip.value === null || !value) {
                return null;
            }
            const ipValue = ip.value;
            if (ipValue === value) {
                errorModel.testIpSame.tiErrorMessage = IpV4Vaild.error8;
                return errorModel;
            }
            return null;
        };
    }
    public requiredFun(ipAddress: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            const errorModel = {
                dulRequired: {
                    tiErrorMessage: 'error'
                }
            };
            const ip = control.root.get(ipAddress);
            if (ip === null || ip.value === null) {
                return null;
            }
            const ipValue = ip.value;
            if (ipValue && !value) {
                errorModel.dulRequired.tiErrorMessage = IpV4Vaild.error9;
                return errorModel;
            }
            return null;
        };
    }

}
