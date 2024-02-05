import { IPV4 } from 'src/app/common-app/utils/common';

// 还原完整的IPV6格式
export function IPv6Supplement(ipv6: string): string {
    if (ipv6 === '' || ipv6 === undefined || ipv6 === null) {
        return '';
    }
    let ipv6Arr = ipv6.split(':');
    // 包含IPv4格式
    if (IPV4.test(ipv6Arr[ipv6Arr.length - 1])) {
        return IPv6Translation(ipv6Arr);
    }
    // 数组长度大于8的情况为，存在::在开头或末尾,且只需补一个0000
    if (ipv6Arr.length > 8) {
        ipv6 = ipv6.replace('::', ':');
        ipv6Arr = ipv6.split(':');
    }
    // 计算::需要替代多少个0000
    const len = 8 - ipv6Arr.length + 1;
    const supplementArr = [];
    for (let i = 0; i < len; i++) {
        supplementArr.push('0000');
    }
    const supplementStr = supplementArr.join(':');
    for (let i = 0; i < ipv6Arr.length; i++) {
        if (ipv6Arr[i].length === 0) {
            if (i === 0 || i === ipv6Arr.length - 1) {
                ipv6Arr[i] = '0000';
            } else {
                ipv6Arr[i] = supplementStr;
            }
        }
        while (ipv6Arr[i].length < 4) {
            ipv6Arr[i] = '0' + ipv6Arr[i];
        }
    }
    return ipv6Arr.join(':');
}

export function IPv6Translation(ipv6Arr): string {
    const spacelen = 6 - (ipv6Arr.length - 1) + 1;
    let spaceStr = '';
    for (let j = 0; j < spacelen; j++) {
        spaceStr += '0000:';
    }
    spaceStr = spaceStr.substr(0, spaceStr.length - 1);
    for (let i = 0; i < ipv6Arr.length - 1; i++) {
        switch (ipv6Arr[i].length) {
            case 0:
                if (i === 0) {
                    ipv6Arr[i] = '0000';
                } else {
                    ipv6Arr[i] = spaceStr;
                }
                break;
            case 1:
                ipv6Arr[i] = '000' + ipv6Arr[i];
                break;
            case 2:
                ipv6Arr[i] = '00' + ipv6Arr[i];
                break;
            case 3:
                ipv6Arr[i] = '0' + ipv6Arr[i];
                break;
            default:
                break;
        }
    }

    return ipv6Arr.join(':');
}

export function checkV6Segment(ip, gateway, prefixLength): boolean {
    if (!ip || !gateway) {
        return;
    }
    let ipSegment = IPv6Supplement(ip);
    ipSegment = convertToBinary(ipSegment);

    let gatewaySegment = IPv6Supplement(gateway);
    gatewaySegment = convertToBinary(gatewaySegment);

    const ipStr = ipSegment.substr(0, prefixLength);
    const gatewayStr = gatewaySegment.substr(0, prefixLength);
    if (ipStr === gatewayStr) {
        return true;
    } else {
        return false;
    }
}

// 将完整的IPv6地址转换成二进制字符串形式
export function convertToBinary(ipv6): string {
    if (!ipv6) {
        return '';
    }
    const result = [];
    let res = '';
    const arr = ipv6.split(':');
    for (const item of arr) {
        if (IPV4.test(item)) {
            const tempArr1 = item.split('.');
            for (const item1 of tempArr1) {
                res = Number(parseInt(item1, 10)).toString(2);
                while (res.length < 8) {
                    res = '0' + res;
                }
                result.push(res);
            }
        } else {
            const tempArr2 = item.split('');
            for (const item2 of tempArr2) {
                res = Number(parseInt(item2, 16)).toString(2);
                if (res.length === 1) {
                    res = '000' + res;
                }
                if (res.length === 2) {
                    res = '00' + res;
                }
                if (res.length === 3) {
                    res = '0' + res;
                }
                result.push(res);
            }
        }
    }
    return result.join('');
}
