/* player client
 * security.js
 */

import CryptoJS from 'crypto-js';

export class Security {
    public _client;
    public _key;
    public _compress;
    public _salt;
    public _codekey;
    constructor(client, key, compress, salt) {
        this._client = client;
        this._key = key;
        this._compress = compress;
        this._salt = salt;
    }
    _Uint8ToWord(d) {
        const len = d.length;
        const words = [];
        let flag = 0;
        for (let i = 0; i < len; i++) {
            if (i % 4 === 0) {
                words[i >>> 2] = 0;
                flag = 16777216;
            } else if (i % 4 === 1) {
                flag = 65536;
            } else if (i % 4 === 2) {
                flag = 256;
            } else {
                flag = 1;
            }
            words[i >>> 2] += d[i] * flag;
        }
        return CryptoJS.lib.WordArray.create(words, len);
    }
    _WordToUint8(data) {
        const arr = [];
        const words = data.words;
        const length = words.length;
        const bytesLen = 8;
        const sigBytesLength = data.sigBytes;
        for (let i = length - 1; i >= 0; i--) {
            for (let j = 0; j < sigBytesLength / length; j++) {
                arr.unshift(words[i] >>> j * bytesLen & 0xff);
            }
        }
        return new Uint8Array(arr);
    }

    _HexStringToBytes(hexstring) {
        const self = this;

        if (hexstring == null || hexstring === '') {
            return;
        }
        const hexstr = hexstring.toUpperCase();
        const len = hexstr.length / 2;
        const bytes = new Uint8Array(len);
        const chars = hexstr.split('');
        let pos = 0;
        for (let i = 0; i < chars.length; i++) {
            pos = i * 2;
            bytes[i] = self._CharToByte(chars[pos]) * 16 + self._CharToByte(chars[pos + 1]);
        }
        return bytes;
    }

    _CharToByte(char) {
        return '0123456789ABCDEF'.indexOf(char);
    }

    _PBKDF2 (key, salt, size) {
        const self = this;

        let saltW = self._HexStringToBytes(salt);
        saltW = self._Uint8ToWord(saltW);

        const result = CryptoJS.PBKDF2(key, saltW, {keySize: size / 4, hasher: CryptoJS.algo.SHA256, iterations: 10000});
        const encrypt = this._WordToUint8(result);
        return encrypt;
    }

    getCodeKey() {
        const self = this;
        this._codekey = self._PBKDF2(this._key, this._salt, 56);
        return this._codekey;
    }

    // 对加密数据进行解密
    videoDecrypt (data) {
        let key = new Uint8Array([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08]);
        let iv = new Uint8Array([0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0]);

        key = this._codekey.slice(0, 16);
        iv = this._codekey.slice(40, 56);

        const keyW = this._Uint8ToWord(key);
        const ivW = this._Uint8ToWord(iv);
        const dataW = this._Uint8ToWord(data);
        const dcBase64String = dataW.toString(CryptoJS.enc.Base64);

        const decryptedData = CryptoJS.AES.decrypt(dcBase64String, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        return this._WordToUint8(decryptedData);
    }

}
