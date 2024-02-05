declare const Date;
Date.prototype['pattern'] = function(format) {
    const _this = this;
    const regReplaceArr = [
        {
            reg: /y+/,
            replaceFn(str) {
                return String(_this.getFullYear()).substr(-1 * str.length);
            }
        }, {
            reg: /M+/,
            replaceStr: _this.getMonth() + 1
        }, {
            reg: /d+/,
            replaceStr: _this.getDate()
        }, {
            reg: /E+/,
            replaceFn(str) {
                const weekDay = ['/u65e5', '/u4e00', '/u4e8c', '/u4e09', '/u56db', '/u4e94', '/u516d'];
                const day = weekDay[_this.getDay()];
                if (str.length > 2) {
                    return '/u661f/u671f' + day;
                }
                if (str.length === 2) {
                    return '/u5468' + day;
                }
                return day;
            }
        }, {
            reg: /H+/,
            replaceStr: _this.getHours()
        }, {
            reg: /h+/,
            replaceStr: _this.getHours() % 12 === 0 ? 12 : _this.getHours() % 12
        }, {
            reg: /m+/,
            replaceStr: _this.getMinutes()
        }, {
            reg: /s+/,
            replaceStr: _this.getSeconds()
        }, {
            reg: /q+/,
            replaceStr: Math.floor((_this.getMonth()) / 3 + 1)
        }, {
            reg: /S/,
            replaceStr: _this.getMilliseconds()
        }
    ];
    let formatedStr = format;
    regReplaceArr.forEach((item) => {
        formatedStr = formatedStr.replace(item.reg, (matchedStr) => {
            if (item.replaceFn) {
                return item.replaceFn(matchedStr);
            }
            const replaceStr = String(item.replaceStr);
            if (matchedStr.length > 1 && parseInt(replaceStr, 10) < 10) {
                return '0' + replaceStr;
            }
            return replaceStr;
        });
    });
    return formatedStr;
};
Date.prototype['format'] = function(format) {
    const o = {
        'M+': this.getUTCMonth() + 1,
        'd+': this.getUTCDate(),
        'h+': this.getUTCHours(),
        'm+': this.getUTCMinutes(),
        's+': this.getUTCSeconds(),
        'q+': Math.floor((this.getUTCMonth() + 3) / 3),
        'S': this.getUTCMilliseconds()
    };
    let tempFormat = '';
    if (/(y+)/.test(format)) {
        tempFormat = format.replace(RegExp.$1, (String(this.getUTCFullYear())).substr(4 - RegExp.$1.length));
    } else {
        tempFormat = format;
    }
    for (const k in o) {
        if (new RegExp('(' + k + ')').test(format)) {
            tempFormat = tempFormat.replace(RegExp.$1, RegExp.$1.length === 1 ? o[k] :
                ('00' + o[k]).substr((String(o[k])).length));
        }
    }
    return tempFormat;
};
export default Date;
