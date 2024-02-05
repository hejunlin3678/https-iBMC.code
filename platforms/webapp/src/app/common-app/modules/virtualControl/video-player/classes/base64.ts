export const base64 = {

    /* Convert data (an array of integers) to a Base64 string. */
    toBase64Table: 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/='.split(''),
    base64Pad: '=',

    encode(ascArr) {
        if (!Array.isArray(ascArr)) {
            return ascArr;
        }
        let outputStr = '';
        const outputIndexArr = [];
        const ascArrLength = ascArr.length;
        const _this = this;

        // 8与6的最小公倍数为24，每3个ascII码能转换为4个base64码；
        const extraAscLength = ascArrLength % 3;
        for (let i = 0; i < ascArrLength; i += 3) {
            outputIndexArr.push(ascArr[i] >>> 2);
            if (ascArr[i + 1] >= 0) {
                outputIndexArr.push((ascArr[i] << 6 >>> 2 & 0x3f) | (ascArr[i + 1] >>> 4));
            } else {
                outputIndexArr.push(ascArr[i] << 6 >>> 2 & 0x3f);
                break;
            }
            if (ascArr[i + 2] >= 0) {
                outputIndexArr.push((ascArr[i + 1] << 4 >>> 2 & 0x3f) | (ascArr[i + 2] >>> 6));
                outputIndexArr.push(ascArr[i + 2] << 2 >>> 2 & 0x3f);
            } else {
                outputIndexArr.push(ascArr[i + 1] << 4 >>> 2 & 0x3f);
            }
        }
        outputIndexArr.forEach((outputIndex) => {
            outputStr += _this.toBase64Table[outputIndex];
        });

        if (extraAscLength === 1) {
            outputStr += '==';
        }
        if (extraAscLength === 2) {
            outputStr += '=';
        }
        const b = new Date().getTime();
        return outputStr;
    }
};
