


import { ISwitch, IOptions } from 'src/app/common-app/models';
import { VolumeStaticModel } from '../models/static-data/volume-static.model';

export function isEmptyBoolean(value: any, trueKey: string, falseKey: string): string {
    if (typeof value === 'boolean') {
        if (value) {
            return trueKey;
        } else {
            return falseKey;
        }
    }
    return null;
}

export function isEmptySwitch(value: any, trueKey: ISwitch, falseKey: ISwitch): ISwitch {
    if (typeof value === 'boolean') {
        if (value) {
            return trueKey;
        } else {
            return falseKey;
        }
    }
    return null;
}

export function unitConversion(sizeBytes, level, calculationMethod, isBoolean) {
    if (!sizeBytes) {
        return 0;
    }
    let sbyte = null;
    if (calculationMethod === 1) {
        if (level === 1) {
            sbyte = sizeBytes * 1024;
        } else if (level === 2) {
            sbyte = sizeBytes * 1024 * 1024;
        } else if (level === 3) {
            sbyte = sizeBytes * 1024 * 1024 * 1024;
        } else if (level === 4) {
            sbyte = sizeBytes * 1024 * 1024 * 1024 * 1024;
        }
    } else if (calculationMethod === 2) {
        if (level === 1) {
            sbyte = sizeBytes / 1024;
        } else if (level === 2) {
            sbyte = sizeBytes / 1024 / 1024;
        } else if (level === 3) {
            sbyte = sizeBytes / 1024 / 1024 / 1024;
        } else if (level === 4) {
            sbyte = sizeBytes / 1024 / 1024 / 1024 / 1024;
        }
    }
    if (isBoolean) {
        sbyte = sbyte.toFixed(3);
    }
    return sbyte && parseFloat(sbyte) ? parseFloat(sbyte) : null;
}

export function getUnitConversionInt(val: number): string {
    if (val !== 0 && !val) {
        return '--';
    }
    if (val < 1024 && val >= 0) {
        return val + ' B';
    } else if (val >= 1024 && val < 1048576) {
        return parseInt(String(val / 1024), 10) + ' KB';
    } else if (val >= 1048576 && val < 1073741824) {
        return parseInt(String(val / 1048576), 10) + ' MB';
    } else if (val >= 1073741824 && val < 1099511627776) {
        return parseInt(String(val / 1073741824), 10) + ' GB';
    } else if (val >= 1099511627776) {
        return parseInt(String(val / 1099511627776), 10) + ' TB';
    }
    return '--';
}

export function getUnitConversionByte(val: number): string {
    if (val !== 0 && !val) {
        return '--';
    }
    if (val < 1024 && val >= 0) {
        return val + ' B';
    } else if (val >= 1024 && val < 1048576) {
        return (val / 1024).toFixed(3) + ' KB';
    } else if (val >= 1048576 && val < 1073741824) {
        return (val / 1048576).toFixed(3) + ' MB';
    } else if (val >= 1073741824 && val < 1099511627776) {
        return (val / 1073741824).toFixed(3) + ' GB';
    } else if (val >= 1099511627776) {
        return (val / 1099511627776).toFixed(3) + ' TB';
    }
    return '--';
}

/**
 * 根据物理盘容量，换算成MB、GB、TB
 * @param capacityNum 物理盘容量
 */
export function computeCapacity(capacityNum: number): { capacity: number, unit: IOptions } {
    let capacity = null;
    let unit = VolumeStaticModel.capacityUnit[2];
    if (capacityNum < 1073741824 && capacityNum >= 0) {
        capacity = Number((capacityNum / 1024 / 1024).toFixed(3));
        unit = VolumeStaticModel.capacityUnit[0];
    } else if (capacityNum < 1099511627776 && capacityNum >= 1073741824) {
        capacity = Number((capacityNum / 1024 / 1024 / 1024).toFixed(3));
        unit = VolumeStaticModel.capacityUnit[1];
    } else {
        capacity = Number((capacityNum / 1024 / 1024 / 1024 / 1024).toFixed(3));
        unit = VolumeStaticModel.capacityUnit[2];
    }
    return { capacity, unit };
}

/**
 * 根据单位和容量换算成B单位数据
 * @param capacity 容量
 * @param unit 单位
 */
export function transCapacity(capacity: number, unit: string) {
    let num = 0;
    switch (unit) {
        case 'MB':
            num = capacity * 1024 * 1024;
            break;
        case 'GB':
            num = capacity * 1024 * 1024 * 1024;
            break;
        case 'TB':
            num = capacity * 1024 * 1024 * 1024 * 1024;
            break;
        default:
            break;
    }
    return num;
}


/**
 * 删除参数多余字段
 * 属性值为空字符串、null、undefined的属性
 * 支持递归删除，即有些空属性是由于代码执行中产生的，这些属性也要删除
 * @param {*} object
 */
export function deleteEmptyProperty(prame: {}) {
    const obj = prame;
    for (const i in obj) {
        if (Object.prototype.hasOwnProperty.call(obj, i)) {
            const value = obj[i];
            if ((value === 0 || value) && typeof value === 'object') {
                if (Array.isArray(value)) {
                    if (value.length === 0) {
                        delete obj[i];
                        continue;
                    }
                }
                deleteEmptyProperty(value);
                if (isEmpty(value)) {
                    delete obj[i];
                }
            } else {
                if (value === '' || value === null || value === undefined) {
                    delete obj[i];
                }
            }
        }
    }
    return obj;
}

function isEmpty(obj: {}) {
    for (const key in obj) {
        if (Object.prototype.hasOwnProperty.call(obj, key)) {
            return false;
        }
    }
    return true;
}

// 200成功提示词解析方法
export function successTip(msgId: string, i18n: any) {
    const alertObj = {
        type: 'success',
        label: 'COMMON_SUCCESS'
    };
    const successObj = i18n.instant(msgId);
    if (successObj && typeof successObj === 'object') {
        alertObj.label = successObj.successTips ? successObj.successTips : 'COMMON_SUCCESS';
    } else if (successObj) {
        alertObj.label = successObj;
    }
    return alertObj;
}

// 数组按照某个对象属性排序
export function sortBy(field: string) {
    return (a, b) => {
        if (a[field] == null) {
            return -1;
        } else if (b[field] == null) {
            return 1;
        }

        const numTest = /^[0-9]$/;
        const afield = a[field].split('');
        const bfield = b[field].split('');
        const len = afield.length > bfield.length ? bfield.length : afield.length;

        for (let i = 0; i < len; i++) {
            // 将遇到的第一个数字开始截取，然后将剩余的字符串转换为数字
            if (numTest.test(afield[i]) && numTest.test(bfield[i])) {
                if (parseInt(a[field].substring(i), 10) > parseInt(b[field].substring(i), 10)) {
                    return 1;
                } else if (parseInt(a[field].substring(i), 10) < parseInt(b[field].substring(i), 10)) {
                    return -1;
                }
            }

            if (afield[i] > bfield[i]) {
                return 1;
            } else if (afield[i] < bfield[i]) {
                return -1;
            }
        }
        return afield.length > bfield.length ? 1 : -1;
    };
}

