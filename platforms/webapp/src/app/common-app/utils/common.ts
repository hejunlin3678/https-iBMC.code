import { CommonData } from '../models/common.model';

export function getRandomFileName(fileName: string) {
    return `${Date.now()}_${fileName}`;
}

// 从响应信息获取错误ID
export function getMessageId(error) {
    let errorObject = '';
    const errorIdArr = [];

    if (typeof (error) === 'string') {
        try {
            errorObject = JSON.parse(error);
        } catch (error) {
            return errorIdArr;
        }
    } else {
        errorObject = error.responseJSON || error;
    }
    if (errorObject['error'] && Array.isArray(errorObject['error'])) {
        errorObject['error'].forEach(errorItem => {
            // 自定义的错误类型，errorId, ralation:表示此错误关联的属性值
            errorIdArr.push({
                errorId: errorItem.code,
                description: errorItem.message,
                relation: ''
            });
        });
    } else if (errorObject['error'] && errorObject['error']['@Message.ExtendedInfo']) {
        errorObject['error']['@Message.ExtendedInfo'].forEach(item => {
            const oldErrorId = item.MessageId.split('.').reverse().shift();
            errorIdArr.push({
                errorId: oldErrorId,
                description: item.Message,
                relation: item.relatedProperties
            });
        });
    }
    return errorIdArr;
}


// 失败提示词解析方法
export function errorTip(msgId: string, i18n: any) {
    let alertObj = {
        type: 'error',
        label: 'COMMON_FAILED'
    };
    // 如果是授权失败错误，则不提示
    if (msgId === 'AuthorizationFailed') {
        return;
    }
    const msgObj = i18n.instant(msgId);

    if (msgObj && typeof msgObj === 'object') {
        if (msgObj.successTips) {
            alertObj = {
                type: 'success',
                label: msgObj.successTips || 'COMMON_SUCCESS'
            };
        } else {
            alertObj = {
                type: 'error',
                label: msgObj.errorMessage || 'COMMON_FAILED'
            };
        }
    } else {
        alertObj.label = 'COMMON_FAILED';
    }
    return alertObj;
}

// 针对后端返回的字符串做编码处理，预防xss攻击
export function htmlEncode(content: string): string {
    if (typeof content !== 'string') {
        return content;
    }
    if (content.length === 0) {
        return '';
    }
    let result = '';
    result = content.replace(/&/g, '&amp;');
    result = result.replace(/</g, '&lt;');
    result = result.replace(/>/g, '&gt;');
    result = result.replace(/\'/g, '&#39;');
    result = result.replace(/\"/g, '&quot;');
    return result;
}

// 字节数超最大数时，截取字符串
export function cutStr(str: string, maxLength: number): string {
    let realLength = 0;
    let charCode = -1;

    let maxIndex = str.length;
    for (let i = 0; i < str.length; i++) {
        charCode = str.charCodeAt(i);
        if (0 <= charCode && charCode <= 127) {
            realLength += 1;
        } else if (128 <= charCode && charCode <= 2047) {
            realLength += 2;
        } else if (2048 <= charCode && charCode <= 65535) {
            realLength += 3;
        }
        if (realLength > maxLength) {
            maxIndex = i;
            break;
        }
    }
    return str.substr(0, maxIndex);
}

// 获取浏览器的类型和版本
export function getBrowserType() {
    const agent = navigator.userAgent.toLowerCase();
    let version: number = 0;
    let browser: string = '';

    // Edge浏览器
    if (agent.indexOf('edge') > -1) {
        browser = 'edge';
        version = parseInt(agent.split('edge/')[1], 10);
    }
    // IE< 11浏览器
    if (agent.indexOf('msie') > -1 && agent.indexOf('compatible') > -1) {
        browser = 'ie';
        version = parseInt(agent.match(/msie\s+\d+\.\d+;/)[0].split(/\s+/)[1], 10);
    }
    // IE11浏览器
    if (agent.indexOf('trident') > -1 && agent.indexOf('rv:11.0') > -1) {
        browser = 'ie';
        version = 11;
    }
    // Firefox浏览器
    if (agent.indexOf('firefox') > -1) {
        browser = 'firefox';
        version = parseInt(agent.split('firefox/')[1], 10);
    }
    // Chrome浏览器
    if (agent.indexOf('chrome') > -1) {
        browser = 'chrome';
        version = parseInt(agent.match(/chrome\/\d+\.[\.\d]+/)[0].split('/')[1], 10);
    }
    // Safari浏览器
    if (agent.indexOf('safari') > -1 && agent.indexOf('chrome') < 0) {
        browser = 'safari';
        version = parseInt(agent.match(/safari\/[\d.]+/)[0].split('/')[1], 10);
    }
    return {
        browser,
        version
    };
}

// 判定浏览器是否低于支持的版本
export function isLowBrowserVer(): boolean {
    const { browser, version } = getBrowserType();
    if (!CommonData.BROWSER_VERSION_SUPPORT[browser] || version < CommonData.BROWSER_VERSION_SUPPORT[browser]) {
        return true;
    }
    return false;
}

// 将国际化文字中的 {0}的参数替换为内容
export function formatEntry(i18nText, rangeArr): string {
    let content = i18nText || '';
    let index = 0;
    content = content.replace(/\{\d+\}/g, () => {
        return rangeArr[index++];
    });
    return content;
}

export function getMouseOutPosition(ev) {
    let direct = '';
    const position = ev.target.getBoundingClientRect();
    const height = ev.target.offsetHeight;
    const bottomY = position.y + height;

    if (ev.clientY < bottomY) {
        direct = 'top';
    }
    if (ev.clientY > position.y) {
        direct = 'bottom';
    }
    return direct;
}

export function getByte(str) {
    if (typeof (str) !== 'string') {
        return 0;
    }
    const codeRangeArr = [
        {
            max: 127,
            min: 0,
            length: 1
        }, {
            max: 2047,
            min: 128,
            length: 2
        }, {
            max: 65535,
            min: 2048,
            length: 3
        }
    ];
    const length = str.length;
    let charCodeLength = 0;
    for (let i = 0; i < length; i++) {
        const charCode = str.charCodeAt(i);
        codeRangeArr.forEach((item) => {
            if (charCode >= item.min && charCode <= item.max) {
                charCodeLength += item.length;
            }
        });
    }
    return charCodeLength;
}

// 检查密码是否由重复子串组成
export function computeRepeatTimes(value: string = ''): number {
    let str1 = '';
    let str2 = '';
    const vLength = value.length;
    for (let i = 1; i <= vLength; i++) {
        if (vLength % i === 0) {
            for (let m = i; m < vLength; m += i) {
                str1 = value.substring(0, i);
                str2 = value.substring(m, m + i);
                if (str1 !== str2) {
                    break;
                } else if ((m + i) === vLength) {
                    return vLength / i;
                }
            }
        }
    }
    return 1;
}

export function isOtherEmptyProperty(value: any, unit?: string): any {
    if (value === null || value === undefined || value === 'N/A' || value === '' || value === 'Unknown') {
        return '--';
    } else {
        return value;
    }
}

/**
 * 所有用到的正则信息
 * 用户名
 */
export const VALID_USERNAME = /^[A-Za-z0-9`~!@#$%^&*()_+<>?:"{},.\/;'[\]]{0,64}$/;
export const VALID_USERNAME_REQUIRE = /^[A-Za-z0-9`~!@#$%^&*()_+<>?:"{},.\/;'[\]]{1,64}$/;
// 0-48个字符，由数字、英文字母和特殊字符组成。
export const VALID_PRODUCT_TAG = /^[a-zA-Z0-9~`!\?,.:;\-_'"\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]{0,48}$/;
// 文件名
export const VALID_VALUE1 = /^[A-Za-z0-9`~!@#$%^&*()_+<>?:"{},.\/;'[\]\\=\s]{0,128}$/;
// 服务器地址
export const VALID_VALUE2 = /^[A-Za-z0-9`~!@#$%^&*()_+<>?:"{},.\/;'[\]\\=\s]{0,1024}$/;
// 描述信息
export const DESCRIPTION = /^[a-zA-Z0-9~`!\?,.:;\-_'"\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]{0,255}$/;
export const IP_REGEX_V4 = /^(25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d?)(\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)){3}$/;
export const IPV4 = /^((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.){3}(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])$/;
// tslint:disable-next-line:max-line-length
export const IPV6 = /^(((([\da-f]{1,4}):){7}([\da-f]{1,4}))|(((([\da-f]{1,4}):){1,7}:)|((([\da-f]{1,4}):){6}:([\da-f]{1,4}))|((([\da-f]{1,4}):){5}:(([\da-f]{1,4}):)?([\da-f]{1,4}))|((([\da-f]{1,4}):){4}:(([\da-f]{1,4}):){0,2}([\da-f]{1,4}))|((([\da-f]{1,4}):){3}:(([\da-f]{1,4}):){0,3}([\da-f]{1,4}))|((([\da-f]{1,4}):){2}:(([\da-f]{1,4}):){0,4}([\da-f]{1,4}))|((([\da-f]{1,4}):){1}:(([\da-f]{1,4}):){0,5}([\da-f]{1,4}))|(::(([\da-f]{1,4}):){0,6}([\da-f]{1,4}))|(::([\da-f]{1,4})?))|(((([\da-f]{1,4}):){6}(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))|((([\da-f]{1,4}):){5}:(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))|((([\da-f]{1,4}):){4}:(([\da-f]{1,4}):)?(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))|((([\da-f]{1,4}):){3}:(([\da-f]{1,4}):){0,2}(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))|((([\da-f]{1,4}):){2}:(([\da-f]{1,4}):){0,3}(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))|(([\da-f]{1,4})::(([\da-f]{1,4}):){0,4}(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))|(::(([\da-f]{1,4}):){0,5}(((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5]))\.){3}((1?[1-9]?\d)|(10\d)|(2[0-4]\d)|(25[0-5])))))$/i;
// tslint:disable-next-line:max-line-length
export const DOMAIN_NAME = /^(?=^.{3,255}$)(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63}))(\.(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63})))+$/;
export const IPV4_IPV6 = /^([0-9A-Fa-f]{1,4}:){6}(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)){3}$/;
// 必须包含大写、小写、数字中的2项
export const MIN_CHARTYPE_2 = [/[a-z]+/, /[A-Z]+/, /[0-9]+/];
// 密码校验不能包含特殊字符
export const SPECIAL_CHAR = /[~`!\?,.:;\-_'"\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=]/;
// 至少包含一个空格或以下特殊字符`~!@#$%^&*()-_=+\\|[{}];:\,<.>/?。
export const SPACE_SPECIAL_CHAR = /[~`!\?,.:;\-_'"\(\)\{\}\[\]\/<>@#\$%\^&\*\+\|\\=\s]/;
// 数字，英文字符，特殊字符,空格
export const KRB_DOMAIN = /^[0-9a-zA-Z`~!@#$%^&*()-_=+[{\]}\\|;'"<.>/?:, ]*$/;
// 主机名
export const HOSTNAME = /^[0-9a-zA-Z-]{1,64}$/;
// 64位由0-9，a-f，A-F组合；全数字，全英文（abcdef/ABCDEF）64位也可以
export const VALID_BASELINE = /^[0-9a-fA-F]{64}$/;

export function ipv6Translation(ipv6Arr) {
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
            default:
                break;
        }
    }
    return ipv6Arr.join(':');
}

export function isEmptyProperty(value: any, unit?: string): any {
    if (value instanceof Array && value.length > 0) {
        if (value.length > 0) {
            return value;
        } else {
            return null;
        }
    } else if (value instanceof Boolean) {
        return value;
    } else if (value === 0 || value) {
        if (unit) {
            return value + unit;
        }
        return value;
    }
    return null;
}

// 将完整的IPv6地址转换成二进制字符串形式
export function convertToBinary(ipv6) {
    if (!ipv6) {
        return '';
    }
    const result = [];
    let res = '';
    const arr = ipv6.split(':');
    const ipregexV4 = /^((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.){3}(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])$/;
    for (const key1 of arr) {
        if (ipregexV4.test(key1)) {
            const _arr = key1.split('.');
            for (const key2 of _arr) {
                res = Number(parseInt(key2, 10)).toString(2);
                while (res.length < 8) {
                    res = '0' + res;
                }
                result.push(res);
            }
        } else {
            const _arrIp = key1.split('');
            for (const key3 of _arrIp) {
                res = Number(parseInt(key3, 16)).toString(2);
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
// 还原ipv6完整格式
export function restituteIpv6(ipv6) {
    if (typeof ipv6 !== 'string') {
        return '';
    }
    // 补全逻辑：1：将字符串由 :: 分隔，2：左侧数组和右侧数组，分别添加到临时的数据中，根据分隔出来的长度，来确定需要补全0的段数 增加补全逻辑 判断::是否在开头或者结尾
    let _temp = null;
    if (ipv6.indexOf('::') > -1) {
        const checkEndReg = /::$/;
        if (ipv6.indexOf('::') === 0) {
            ipv6 = '0' + ipv6;
        } else if (checkEndReg.test(ipv6)) {
            ipv6 = ipv6 + '0';
        }
        const arr = ipv6.split('::');
        let tempArr = [];
        tempArr = arr[0].length > 0 ? tempArr.concat(arr[0]) : tempArr;
        const len = 8 - arr[0].split(':').length - arr[1].split(':').length;
        for (let i = 0; i < len; i++) {
            tempArr.push('0000');
        }
        tempArr = arr[1].length > 0 ? tempArr.concat(arr[1]) : tempArr;
        _temp = tempArr.join(':').split(':');
    }

    // 如果没有::分隔的IP地址，则将8段数字前缀补全为0
    _temp = _temp || ipv6.split(':');
    for (let j = 0; j < _temp.length; j++) {
        while (_temp[j].length < 4) {
            _temp[j] = '0' + _temp[j];
        }
    }
    return _temp.join(':');
}
// 还原混合的IPv6完整格式
export function iPv6Translation(ipv46Arr) {
    if (ipv46Arr) {
        const spacelen = 6 - (ipv46Arr.length - 1) + 1;
        let spaceStr = '';
        for (let j = 0; j < spacelen; j++) {
            spaceStr += '0000:';
        }
        spaceStr = spaceStr.substr(0, spaceStr.length - 1);
        for (let i = 0; i < ipv46Arr.length - 1; i++) {
            switch (ipv46Arr[i].length) {
                case 0:
                    if (i === 0) {
                        ipv46Arr[i] = '0000';
                    } else {
                        ipv46Arr[i] = spaceStr;
                    }
                    break;
                case 1:
                    ipv46Arr[i] = '000' + ipv46Arr[i];
                    break;
                case 2:
                    ipv46Arr[i] = '00' + ipv46Arr[i];
                    break;
                case 3:
                    ipv46Arr[i] = '0' + ipv46Arr[i];
                    break;
                default:
                    break;
            }
        }
        return ipv46Arr.join(':');
    }
}

/**
 * 获取单个vlan信息
 * @param vlan 单个vlan信息
 * @returns 组装的vlan信息
 */
export function getVlan(vlan) {
    const portData = {};
    portData['vlanId'] = vlan.ID !== null ? vlan.ID : '--';
    portData['vlanEnable'] = vlan.Enable !== null ? (vlan.Enable ? 'Enable' : 'Disable') : '--';
    portData['prEnable'] = vlan.Priority !== null ? (vlan.Priority) : '--';
    return `${portData['vlanId']}|${portData['vlanEnable']}|${portData['prEnable']}`;
}
/**
 *
 * @param info ipv4 或 ipv6的信息
 * @param type 类型标识（ipv4/ipv6）
 * @returns 组装的ipv4/ipv6信息
 */
export function _dealIpInfo(info, type) {
    let ipInfo = '';
    if (!info || info.length === 0) {
        return null;
    }
    if (type === 'ipv4') {
        for (const v of info) {
            const address = !v.Address ? '--' : v.Address;
            const subnetMask = !v.Mask ? '--' : v.Mask;
            const gateway = !v.Gateway ? '--' : v.Gateway;

            ipInfo += `${address}|${subnetMask}|${gateway} `;
        }
    } else if (type === 'ipv6') {
        for (const v of info) {
            const address = !v.Address ? '--' : v.Address;
            const prefix = !v.Prefix ? '--' : v.Prefix;
            const gateway = !v.Gateway ? '--' : v.Gateway;
            ipInfo += `${address}|${prefix}|${gateway} `;
        }
    }
    return ipInfo;
}

export function _dealUsefulString(info: string | null | undefined) {
    let results = '--';
    if (info !== null && info !== undefined && info !== '') {
        results = info;
    }
    return results;
}

export const JS_KEY_BACKSPACE = 0x08;
export const JS_KEY_TAB = 0x09;
export const JS_KEY_CLEAR = 0x0c;
export const JS_KEY_ENTER = 0x0d;
export const JS_KEY_SHIFT = 0x10;
export const JS_KEY_CTRL = 0x11;
export const JS_KEY_ALT = 0x12;
export const JS_KEY_BREAK = 0x13;
export const JS_KEY_CAPS = 0x14;
export const JS_KEY_ESC = 0x1b;
export const JS_KEY_SPACE = 0x20;
export const JS_KEY_PAGEUP = 0x21;
export const JS_KEY_PAGEDOWN = 0x22;
export const JS_KEY_END = 0x23;
export const JS_KEY_HOME = 0x24;
export const JS_KEY_LEFT = 0x25;
export const JS_KEY_UP = 0x26;
export const JS_KEY_RIGHT = 0x27;
export const JS_KEY_DOWN = 0x28;
export const JS_KEY_INSERT = 0x2d;
export const JS_KEY_DEL = 0x2e;
export const JS_KEY_0 = 0x30;
export const JS_KEY_1 = 0x31;
export const JS_KEY_2 = 0x32;
export const JS_KEY_3 = 0x33;
export const JS_KEY_4 = 0x34;
export const JS_KEY_5 = 0x35;
export const JS_KEY_6 = 0x36;
export const JS_KEY_7 = 0x37;
export const JS_KEY_8 = 0x38;
export const JS_KEY_9 = 0x39;
export const JS_KEY_FF_SEMI = 0x3b;
export const JS_KEY_FF_PLUS = 0x3d;
export const JS_KEY_A = 0x41;
export const JS_KEY_B = 0x42;
export const JS_KEY_C = 0x43;
export const JS_KEY_D = 0x44;
export const JS_KEY_E = 0x45;
export const JS_KEY_F = 0x46;
export const JS_KEY_G = 0x47;
export const JS_KEY_H = 0x48;
export const JS_KEY_I = 0x49;
export const JS_KEY_J = 0x4a;
export const JS_KEY_K = 0x4b;
export const JS_KEY_L = 0x4c;
export const JS_KEY_M = 0x4d;
export const JS_KEY_N = 0x4e;
export const JS_KEY_O = 0x4f;
export const JS_KEY_P = 0x50;
export const JS_KEY_Q = 0x51;
export const JS_KEY_R = 0x52;
export const JS_KEY_S = 0x53;
export const JS_KEY_T = 0x54;
export const JS_KEY_U = 0x55;
export const JS_KEY_V = 0x56;
export const JS_KEY_W = 0x57;
export const JS_KEY_X = 0x58;
export const JS_KEY_Y = 0x59;
export const JS_KEY_Z = 0x5a;
export const JS_KEY_LWIN = 0x5b;
export const JS_KEY_RWIN = 0x5c;
export const JS_KEY_MENU = 0x5d;
export const JS_KEY_0P = 0x60;
export const JS_KEY_1P = 0x61;
export const JS_KEY_2P = 0x62;
export const JS_KEY_3P = 0x63;
export const JS_KEY_4P = 0x64;
export const JS_KEY_5P = 0x65;
export const JS_KEY_6P = 0x66;
export const JS_KEY_7P = 0x67;
export const JS_KEY_8P = 0x68;
export const JS_KEY_9P = 0x69;
export const JS_KEY_MULP = 0x6a;
export const JS_KEY_PLUSP = 0x6b;
export const JS_KEY_ENTERP = 0x6c;
export const JS_KEY_MINP = 0x6d;
export const JS_KEY_DELP = 0x6e;
export const JS_KEY_DIVP = 0x6f;
export const JS_KEY_F1 = 0x70;
export const JS_KEY_F2 = 0x71;
export const JS_KEY_F3 = 0x72;
export const JS_KEY_F4 = 0x73;
export const JS_KEY_F5 = 0x74;
export const JS_KEY_F6 = 0x75;
export const JS_KEY_F7 = 0x76;
export const JS_KEY_F8 = 0x77;
export const JS_KEY_F9 = 0x78;
export const JS_KEY_F10 = 0x79;
export const JS_KEY_F11 = 0x7a;
export const JS_KEY_F12 = 0x7b;
export const JS_KEY_NUM = 0x90;
export const JS_KEY_SCROLL = 0x91;
export const JS_KEY_FF_MIN = 0xad;
export const JS_KEY_SEMI = 0xba;
export const JS_KEY_PLUS = 0xbb;
export const JS_KEY_COMMA = 0xbc;
export const JS_KEY_MIN = 0xbd;
export const JS_KEY_DOT = 0xbe;
export const JS_KEY_DIV = 0xbf;
export const JS_KEY_QUOTO = 0xc0;
export const JS_KEY_KANJI1 = 0xc1;
export const JS_KEY_BR1 = 0xdb;
export const JS_KEY_OR = 0xdc;
export const JS_KEY_BR2 = 0xdd;
export const JS_KEY_QUOT = 0xde;
export const JS_KEY_OR_1 = 0xe2;
export const JS_KEY_KANJI3 = 0xff;

export const USB_KEY_A = 0x04;
export const USB_KEY_B = 0x05;
export const USB_KEY_C = 0x06;
export const USB_KEY_D = 0x07;
export const USB_KEY_E = 0x08;
export const USB_KEY_F = 0x09;
export const USB_KEY_G = 0x0a;
export const USB_KEY_H = 0x0b;
export const USB_KEY_I = 0x0c;
export const USB_KEY_J = 0x0d;
export const USB_KEY_K = 0x0e;
export const USB_KEY_L = 0x0f;
export const USB_KEY_M = 0x10;
export const USB_KEY_N = 0x11;
export const USB_KEY_O = 0x12;
export const USB_KEY_P = 0x13;
export const USB_KEY_Q = 0x14;
export const USB_KEY_R = 0x15;
export const USB_KEY_S = 0x16;
export const USB_KEY_T = 0x17;
export const USB_KEY_U = 0x18;
export const USB_KEY_V = 0x19;
export const USB_KEY_W = 0x1a;
export const USB_KEY_X = 0x1b;
export const USB_KEY_Y = 0x1c;
export const USB_KEY_Z = 0x1d;
export const USB_KEY_1 = 0x1e;
export const USB_KEY_2 = 0x1f;
export const USB_KEY_3 = 0x20;
export const USB_KEY_4 = 0x21;
export const USB_KEY_5 = 0x22;
export const USB_KEY_6 = 0x23;
export const USB_KEY_7 = 0x24;
export const USB_KEY_8 = 0x25;
export const USB_KEY_9 = 0x26;
export const USB_KEY_0 = 0x27;
export const USB_KEY_ENTER = 0x28;
export const USB_KEY_ESC = 0x29;
export const USB_KEY_BACK = 0x2a;
export const USB_KEY_TAB = 0x2b;
export const USB_KEY_SPACE = 0x2c;
export const USB_KEY_MIN = 0x2d;
export const USB_KEY_PLUS = 0x2e;
export const USB_KEY_BR1 = 0x2f;
export const USB_KEY_BR2 = 0x30;
export const USB_KEY_OR = 0x31;
export const USB_KEY_SEMI = 0x33;
export const USB_KEY_QUOT = 0x34;
export const USB_KEY_QUOTO = 0x35;
export const USB_KEY_COMMA = 0x36;
export const USB_KEY_DOT = 0x37;
export const USB_KEY_DIV = 0x38;
export const USB_KEY_CAPS = 0x39;
export const USB_KEY_F1 = 0x3a;
export const USB_KEY_F2 = 0x3b;
export const USB_KEY_F3 = 0x3c;
export const USB_KEY_F4 = 0x3d;
export const USB_KEY_F5 = 0x3e;
export const USB_KEY_F6 = 0x3f;
export const USB_KEY_F7 = 0x40;
export const USB_KEY_F8 = 0x41;
export const USB_KEY_F9 = 0x42;
export const USB_KEY_F10 = 0x43;
export const USB_KEY_F11 = 0x44;
export const USB_KEY_F12 = 0x45;
export const USB_KEY_PRNT = 0x46;
export const USB_KEY_SCROLL = 0x47;
export const USB_KEY_BREAK = 0x48;
export const USB_KEY_INSERT = 0x49;
export const USB_KEY_HOME = 0x4a;
export const USB_KEY_PGUP = 0x4b;
export const USB_KEY_DEL = 0x4c;
export const USB_KEY_END = 0x4d;
export const USB_KEY_PGDN = 0x4e;
export const USB_KEY_RIGHT = 0x4f;
export const USB_KEY_LEFT = 0x50;
export const USB_KEY_DOWN = 0x51;
export const USB_KEY_UP = 0x52;
export const USB_KEY_NUMP = 0x53;
export const USB_KEY_DIVP = 0x54;
export const USB_KEY_MULP = 0x55;
export const USB_KEY_MINP = 0x56;
export const USB_KEY_PLUSP = 0x57;
export const USB_KEY_ENTERP = 0x58;
export const USB_KEY_1P = 0x59;
export const USB_KEY_2P = 0x5a;
export const USB_KEY_3P = 0x5b;
export const USB_KEY_4P = 0x5c;
export const USB_KEY_5P = 0x5d;
export const USB_KEY_6P = 0x5e;
export const USB_KEY_7P = 0x5f;
export const USB_KEY_8P = 0x60;
export const USB_KEY_9P = 0x61;
export const USB_KEY_0P = 0x62;
export const USB_KEY_DELP = 0x63;
export const USB_KEY_OR_1 = 0x64;
export const USB_KEY_MENU = 0x65;
export const USB_KEY_KANJI1 = 0x87;
export const USB_KEY_KANJI2 = 0x88;
export const USB_KEY_KANJI3 = 0x89;
export const USB_KEY_KANJI4 = 0x8a;
export const USB_KEY_KANJI5 = 0x8b;
export const USB_KEY_CTRL = 0xe0;
export const USB_KEY_SHIFT = 0xe1;
export const USB_KEY_ALT = 0xe2;
export const USB_KEY_ALTGRAPH = 0xe6;

export const uskeyboardTable = new Array(
    [JS_KEY_BACKSPACE, USB_KEY_BACK, 'Backspace'],
    [JS_KEY_TAB, USB_KEY_TAB, 'Tab'],
    [JS_KEY_ENTER, USB_KEY_ENTER, 'Enter'],
    [JS_KEY_SHIFT, USB_KEY_SHIFT, 'Shift'],
    [JS_KEY_CTRL, USB_KEY_CTRL, 'Control'],
    [JS_KEY_ALT, USB_KEY_ALT, 'Alt'],
    [JS_KEY_BREAK, USB_KEY_BREAK, 'Pause'],
    [JS_KEY_CAPS, USB_KEY_CAPS, 'CapsLock'],
    [JS_KEY_ESC, USB_KEY_ESC, 'Escape'],
    [JS_KEY_SPACE, USB_KEY_SPACE, 'Space'],
    [JS_KEY_PAGEUP, USB_KEY_PGUP, 'PageUp'],
    [JS_KEY_PAGEDOWN, USB_KEY_PGDN, 'PageDown'],
    [JS_KEY_END, USB_KEY_END, 'End'],
    [JS_KEY_HOME, USB_KEY_HOME, 'Home'],
    [JS_KEY_LEFT, USB_KEY_LEFT, 'ArrowLeft'],
    [JS_KEY_UP, USB_KEY_UP, 'ArrowUp'],
    [JS_KEY_RIGHT, USB_KEY_RIGHT, 'ArrowRight'],
    [JS_KEY_DOWN, USB_KEY_DOWN, 'ArrowDown'],
    [JS_KEY_INSERT, USB_KEY_INSERT, 'Insert'],
    [JS_KEY_DEL, USB_KEY_DEL, 'Delete'],
    [JS_KEY_KANJI1, USB_KEY_KANJI1, 'IntlRo'],
    [JS_KEY_KANJI3, USB_KEY_KANJI3, ''],
    [JS_KEY_OR_1, USB_KEY_OR_1, 'IntlBackslash', '', ''],

    [JS_KEY_0, USB_KEY_0, 'Digit0', '0', ')'],
    [JS_KEY_1, USB_KEY_1, 'Digit1', '1', '!'],
    [JS_KEY_2, USB_KEY_2, 'Digit2', '2', '@'],
    [JS_KEY_3, USB_KEY_3, 'Digit3', '3', '#'],
    [JS_KEY_4, USB_KEY_4, 'Digit4', '4', '$'],
    [JS_KEY_5, USB_KEY_5, 'Digit5', '5', '%'],
    [JS_KEY_6, USB_KEY_6, 'Digit6', '6', '^'],
    [JS_KEY_7, USB_KEY_7, 'Digit7', '7', '&'],
    [JS_KEY_8, USB_KEY_8, 'Digit8', '8', '*'],
    [JS_KEY_9, USB_KEY_9, 'Digit9', '9', '('],
    [JS_KEY_FF_SEMI, USB_KEY_SEMI, 'Semicolon', ';', ':'],
    [JS_KEY_FF_PLUS, USB_KEY_PLUS, 'Equal', '=', '+'],

    [JS_KEY_A, USB_KEY_A, 'KeyA', 'a', 'A'],
    [JS_KEY_B, USB_KEY_B, 'KeyB', 'b', 'B'],
    [JS_KEY_C, USB_KEY_C, 'KeyC', 'c', 'C'],
    [JS_KEY_D, USB_KEY_D, 'KeyD', 'd', 'D'],
    [JS_KEY_E, USB_KEY_E, 'KeyE', 'e', 'E'],
    [JS_KEY_F, USB_KEY_F, 'KeyF', 'f', 'F'],
    [JS_KEY_G, USB_KEY_G, 'KeyG', 'g', 'G'],
    [JS_KEY_H, USB_KEY_H, 'KeyH', 'h', 'H'],
    [JS_KEY_I, USB_KEY_I, 'KeyI', 'i', 'I'],
    [JS_KEY_J, USB_KEY_J, 'KeyJ', 'j', 'J'],
    [JS_KEY_K, USB_KEY_K, 'KeyK', 'k', 'K'],
    [JS_KEY_L, USB_KEY_L, 'KeyL', 'l', 'L'],
    [JS_KEY_M, USB_KEY_M, 'KeyM', 'm', 'M'],
    [JS_KEY_N, USB_KEY_N, 'KeyN', 'n', 'N'],
    [JS_KEY_O, USB_KEY_O, 'KeyO', 'o', 'O'],
    [JS_KEY_P, USB_KEY_P, 'KeyP', 'p', 'P'],
    [JS_KEY_Q, USB_KEY_Q, 'KeyQ', 'q', 'Q'],
    [JS_KEY_R, USB_KEY_R, 'KeyR', 'r', 'R'],
    [JS_KEY_S, USB_KEY_S, 'KeyS', 's', 'S'],
    [JS_KEY_T, USB_KEY_T, 'KeyT', 't', 'T'],
    [JS_KEY_U, USB_KEY_U, 'KeyU', 'u', 'U'],
    [JS_KEY_V, USB_KEY_V, 'KeyV', 'v', 'V'],
    [JS_KEY_W, USB_KEY_W, 'KeyW', 'w', 'W'],
    [JS_KEY_X, USB_KEY_X, 'KeyX', 'x', 'X'],
    [JS_KEY_Y, USB_KEY_Y, 'KeyY', 'y', 'Y'],
    [JS_KEY_Z, USB_KEY_Z, 'KeyZ', 'z', 'Z'],
    [JS_KEY_LWIN, 0, 'Meta'],
    [JS_KEY_RWIN, 0, 'Meta'],
    [JS_KEY_MENU, USB_KEY_MENU, 'ContextMenu'],

    [JS_KEY_0P, USB_KEY_0P, 'Numpad0', '0(Numpad)', 'Insert(Numpad)'],
    [JS_KEY_1P, USB_KEY_1P, 'Numpad1', '1(Numpad)', 'End(Numpad)'],
    [JS_KEY_2P, USB_KEY_2P, 'Numpad2', '2(Numpad)', 'ArrowDown(Numpad)'],
    [JS_KEY_3P, USB_KEY_3P, 'Numpad3', '3(Numpad)', 'PageDown(Numpad)'],
    [JS_KEY_4P, USB_KEY_4P, 'Numpad4', '4(Numpad)', 'ArrowLeft(Numpad)'],
    [JS_KEY_5P, USB_KEY_5P, 'Numpad5', '5(Numpad)', 'Clear(Numpad)'],
    [JS_KEY_6P, USB_KEY_6P, 'Numpad6', '6(Numpad)', 'ArrowRight(Numpad)'],
    [JS_KEY_7P, USB_KEY_7P, 'Numpad7', '7(Numpad)', 'Home(Numpad)'],
    [JS_KEY_8P, USB_KEY_8P, 'Numpad8', '8(Numpad)', 'PageUp(Numpad)'],
    [JS_KEY_9P, USB_KEY_9P, 'Numpad9', '9(Numpad)', 'PageUp(Numpad)'],
    [JS_KEY_MULP, USB_KEY_MULP, 'NumpadMultiply', '*(Numpad)'],
    [JS_KEY_PLUSP, USB_KEY_PLUSP, 'NumpadAdd', '+(Numpad)'],
    [JS_KEY_ENTERP, USB_KEY_ENTERP, 'NumpadEnter', 'Enter(Numpad)'],
    [JS_KEY_MINP, USB_KEY_MINP, 'NumpadSubtract', '-(Numpad)'],
    [JS_KEY_DELP, USB_KEY_DELP, 'NumpadDecimal', '.', 'Delete(Numpad)'],
    [JS_KEY_DIVP, USB_KEY_DIVP, 'NumpadDivide', '/(Numpad)'],
    [JS_KEY_F1, USB_KEY_F1, 'F1'],
    [JS_KEY_F2, USB_KEY_F2, 'F2'],
    [JS_KEY_F3, USB_KEY_F3, 'F3'],
    [JS_KEY_F4, USB_KEY_F4, 'F4'],
    [JS_KEY_F5, USB_KEY_F5, 'F5'],
    [JS_KEY_F6, USB_KEY_F6, 'F6'],
    [JS_KEY_F7, USB_KEY_F7, 'F7'],
    [JS_KEY_F8, USB_KEY_F8, 'F8'],
    [JS_KEY_F9, USB_KEY_F9, 'F9'],
    [JS_KEY_F10, USB_KEY_F10, 'F10'],
    [JS_KEY_F11, USB_KEY_F11, 'F11'],
    [JS_KEY_F12, USB_KEY_F12, 'F12'],
    [JS_KEY_NUM, USB_KEY_NUMP, 'NumLock'],
    [JS_KEY_SCROLL, USB_KEY_SCROLL, 'ScrollLock'],
    [JS_KEY_FF_MIN, USB_KEY_MIN, 'Minus', '-', '_'],
    [JS_KEY_SEMI, USB_KEY_SEMI, 'Semicolon', ';', ':'],
    [JS_KEY_PLUS, USB_KEY_PLUS, 'Equal', '=', '+'],
    [JS_KEY_COMMA, USB_KEY_COMMA, 'Comma', ',', '<'],
    [JS_KEY_MIN, USB_KEY_MIN, 'Minus', '-', '_'],
    [JS_KEY_DOT, USB_KEY_DOT, 'Period', '.', '>'],
    [JS_KEY_DIV, USB_KEY_DIV, 'Slash', '/', '?'],
    [JS_KEY_QUOTO, USB_KEY_QUOTO, 'Backquote', '`', '~'],
    [JS_KEY_BR1, USB_KEY_BR1, 'BracketLeft', '[', '{'],
    [JS_KEY_OR, USB_KEY_OR, 'Backslash', '\\', '|'],
    [JS_KEY_BR2, USB_KEY_BR2, 'BracketRight', ']', '}'],
    [JS_KEY_QUOT, USB_KEY_QUOT, 'Quote', '\'', '"']
);
