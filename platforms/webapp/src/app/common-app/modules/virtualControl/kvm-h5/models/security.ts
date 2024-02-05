import CryptoJS from 'crypto-js';
export class Security {
    public _kvmTagId;

    public _kvmTagIdExt;

    public _kvmReconnectCode = null;

    public _kvmAuthCode = null;

    public _kvmNegotiationIv = null;

    public _kvmNegotiationKey = null;

    public _kvmCryptIv = null;

    public _kvmCryptKey = null;

    public _kvmCryptKeyExt = null;

    /* 此项只允许通过脚本配置，不可直接删除、修改 */
    public _kvmEncryptionConfig = false;

    public _kvmEncryption;

    public _vmmAuthCode = null;

    public _vmmCryptIv = null;

    public _vmmCryptKey = null;

    public _vmmEncryption;

    public _client;

    public _hmac = CryptoJS.algo.SHA256;

    public _iterations = 10000;

    constructor(client, kvmEncrypt, vmmEncrypt, verifyId, keyAndIv, verifyIdExt) {
        this._kvmTagId = verifyId;
        this._kvmTagIdExt = verifyIdExt;
        this._kvmEncryption = kvmEncrypt;
        this._vmmEncryption = vmmEncrypt;
        this._client = client;
        this._InitSecurity(keyAndIv);
    }

    needConsultation() {
        if (this._kvmTagIdExt !== '') {
            return true;
        } else {
            return false;
        }
    }

    setHmac(hmac) {
        if (Number(hmac) === 1 || Number(hmac) === 2) {
            this._hmac = CryptoJS.algo.SHA1;
        } else {
            this._hmac = CryptoJS.algo.SHA256;
        }
    }

    setIterations(iterations) {
        this._iterations = iterations;
    }

    getKvmTagId() {
        return this._kvmTagId;
    }

    getKvmAuthCode() {
        return this._kvmAuthCode;
    }

    getKvmReconnectCode() {
        const temp = this._kvmReconnectCode;
        return temp;
    }

    isKvmEncryption() {
        if (this._kvmEncryptionConfig === true) {
            return Number(this._kvmEncryption) === 1;
        }
        return true;
    }

    isVmmEncryption() {
        return Number(this._vmmEncryption) === 1;
    }

    _WordToUint8(data) {
        const arr = [];
        const words = data.words;
        const OUTPUT_BYTE_LENGTH = 8;
        const INPUT_BYTE_LENGTH = 32;
        const ratio = INPUT_BYTE_LENGTH / OUTPUT_BYTE_LENGTH;
        const sigBytesLength = data.sigBytes;
        for (let i = sigBytesLength / ratio - 1; i >= 0; i--) {
            for (let j = 0; j < ratio; j++) {
                arr.unshift(words[i] >>> j * OUTPUT_BYTE_LENGTH & 0xff);
            }
        }
        return new Uint8Array(arr);
    }

    _Uint8ToString(a) {
        const b = [];
        for (let e = a.length, d = 0; d < e; d++) {
            b.push(String.fromCharCode(a[d] & 255));
        }
        return b.join('');
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

    _HexToInt(h) {
        if (h === 'a' || h === 'A') {
            return 10;
        } else if (h === 'b' || h === 'B') {
            return 11;
        } else if (h === 'c' || h === 'C') {
            return 12;
        } else if (h === 'd' || h === 'D') {
            return 13;
        } else if (h === 'e' || h === 'E') {
            return 14;
        } else if (h === 'f' || h === 'F') {
            return 15;
        } else {
            return h;
        }
    }

    _InitSecurity(keyAndIv) {
        if (keyAndIv.length === 64) {
            this._kvmNegotiationKey = new Uint8Array(16);
            this._kvmNegotiationIv = new Uint8Array(16);
            for (let i = 0; i < 16; i++) {
                this._kvmNegotiationKey[i] =
                    (parseInt(this._HexToInt(keyAndIv[2 * i]), 10) << 4) +
                    parseInt(this._HexToInt(keyAndIv[2 * i + 1]), 10);
                this._kvmNegotiationIv[i] =
                    (parseInt(this._HexToInt(keyAndIv[2 * i + 32]), 10) << 4) +
                    parseInt(this._HexToInt(keyAndIv[2 * i + 33]), 10);
            }
        }
    }

    generateAuthCode() {
        if (this._kvmEncryption || this._kvmEncryptionConfig === false) {
            if (this._kvmTagIdExt !== '') {
                this._kvmAuthCode = this._PBKDF2(this._kvmTagIdExt, this._kvmNegotiationIv, 24);
            } else {
                this._kvmAuthCode = this._PBKDF2(this._kvmTagId, this._kvmNegotiationIv, 24);
            }
        }
    }

    kvmGetKey(originKey, originIv) {
        let key; let iv;

        if (this._kvmEncryptionConfig === true) {
            if (Number(this._kvmEncryption) === 0) {
                key = new Uint8Array([
                    0x01,
                    0x02,
                    0x03,
                    0x04,
                    0x05,
                    0x06,
                    0x07,
                    0x08,
                    0x01,
                    0x02,
                    0x03,
                    0x04,
                    0x05,
                    0x06,
                    0x07,
                    0x08
                ]);
                iv = new Uint8Array([0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0]);
                key[0] = (this._kvmTagId >> 24) & 0xff;
                key[1] = (this._kvmTagId >> 16) & 0xff;
                key[2] = (this._kvmTagId >> 8) & 0xff;
                key[3] = this._kvmTagId & 0xff;

                return [key, iv];
            }
        }

        key = originKey;
        iv = originIv;

        return [key, iv];
    }

    kvmDecrypt(data) {
        let key; let iv;
        let keyAndIv;

        keyAndIv = this.kvmGetKey(this._kvmCryptKey, this._kvmCryptIv);
        key = keyAndIv[0];
        iv = keyAndIv[1];

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

    kvmEncrypt(data) {
        let key; let iv;
        let keyAndIv;

        keyAndIv = this.kvmGetKey(this._kvmCryptKey, this._kvmCryptIv);
        key = keyAndIv[0];
        iv = keyAndIv[1];

        const alignLength = data.length & 0x0f ? (data.length + 16) & 0xfff0 : data.length;
        const dataAlign = new Uint8Array(alignLength);
        for (let i = 0; i < data.length; i++) {
            dataAlign[i] = data[i];
        }

        const keyW = this._Uint8ToWord(key);
        const ivW = this._Uint8ToWord(iv);
        const dataW = this._Uint8ToWord(dataAlign);

        const encryptedData = CryptoJS.AES.encrypt(dataW, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        const temp = CryptoJS.enc.Base64.parse(encryptedData.toString());
        return this._WordToUint8(temp);
    }

    kvmEncryptExt(data) {
        let key; let iv;
        let keyAndIv;

        keyAndIv = this.kvmGetKey(this._kvmCryptKeyExt, this._kvmCryptIv);
        key = keyAndIv[0];
        iv = keyAndIv[1];

        const alignLength = data.length & 0x0f ? (data.length + 16) & 0xfff0 : data.length;
        const dataAlign = new Uint8Array(alignLength);
        for (let i = 0; i < data.length; i++) {
            dataAlign[i] = data[i];
        }

        const keyW = this._Uint8ToWord(key);
        const ivW = this._Uint8ToWord(iv);
        const dataW = this._Uint8ToWord(dataAlign);

        const encryptedData = CryptoJS.AES.encrypt(dataW, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        const temp = CryptoJS.enc.Base64.parse(encryptedData.toString());
        return this._WordToUint8(temp);
    }

    vmmDecrypt(data) {
        const keyW = this._Uint8ToWord(this._vmmCryptKey);
        const ivW = this._Uint8ToWord(this._vmmCryptIv);
        const dataW = this._Uint8ToWord(data);
        const dcBase64String = dataW.toString(CryptoJS.enc.Base64);

        const decryptedData = CryptoJS.AES.decrypt(dcBase64String, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        return this._WordToUint8(decryptedData);
    }

    vmmEncrypt(data) {
        const alignLength = data.length & 0x0f ? (data.length + 16) & 0xfff0 : data.length;
        const dataAlign = new Uint8Array(alignLength);
        for (let i = 0; i < data.length; i++) {
            dataAlign[i] = data[i];
        }

        const keyW = this._Uint8ToWord(this._vmmCryptKey);
        const ivW = this._Uint8ToWord(this._vmmCryptIv);
        const dataW = this._Uint8ToWord(dataAlign);

        const encryptedData = CryptoJS.AES.encrypt(dataW, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        const temp = CryptoJS.enc.Base64.parse(encryptedData.toString());
        return this._WordToUint8(temp);
    }

    _PBKDF2(data, salt, size) {
        const saltW = this._Uint8ToWord(salt);
        const result = CryptoJS.PBKDF2(data, saltW, {
            keySize: size / 4,
            hasher: this._hmac,
            iterations: this._iterations
        });
        const encrypt = this._WordToUint8(result);
        this.revertUint8Arr(encrypt);
        return encrypt;
    }

    generateKvmKeyAndIv(data, remainKvmKeyAndIv) {
        const copyedRemainArr = remainKvmKeyAndIv.slice(0);
        const keyW = this._Uint8ToWord(this._kvmNegotiationKey);
        const ivW = this._Uint8ToWord(this._kvmNegotiationIv);
        const dataW = this._Uint8ToWord(data);
        const dcBase64String = dataW.toString(CryptoJS.enc.Base64);
        let decryptedData = CryptoJS.AES.decrypt(dcBase64String, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        decryptedData = this._WordToUint8(decryptedData);
        const password = decryptedData.slice(0, 32);
        const result = this._PBKDF2(this._Uint8ToString(password), decryptedData.slice(32, 48), 48);
        this._kvmCryptKey = result.slice(0, 16);
        this._kvmCryptKeyExt = result.slice(16, 32);

        this.revertUint8Arr(copyedRemainArr);
        this._kvmCryptIv = copyedRemainArr;
    }

    revertUint8Arr(encrypt) {
        let swap = 0;
        for (let i = 0; encrypt.length % 4 === 0 && i < encrypt.length; i += 4) {
            swap = encrypt[i];
            encrypt[i] = encrypt[i + 3];
            encrypt[i + 3] = swap;
            swap = encrypt[i + 1];
            encrypt[i + 1] = encrypt[i + 2];
            encrypt[i + 2] = swap;
        }
    }

    generateReconnectKey(data) {
        const keyW = this._Uint8ToWord(this._kvmNegotiationKey);
        const ivW = this._Uint8ToWord(this._kvmNegotiationIv);
        const dataW = this._Uint8ToWord(data);
        const dcBase64String = dataW.toString(CryptoJS.enc.Base64);
        const decryptedData = CryptoJS.AES.decrypt(dcBase64String, keyW, {
            iv: ivW,
            mode: CryptoJS.mode.CBC,
            padding: CryptoJS.pad.NoPadding
        });
        this._kvmReconnectCode = this._WordToUint8(decryptedData);
    }

    generateVmmKeyAndIv(data) {
        const password = data.slice(0, 20);
        const result = this._PBKDF2(this._Uint8ToString(password), data.slice(20, 36), 56);
        this.revertUint8Arr(result);
        this._vmmAuthCode = result.slice(0, 24);
        this._vmmCryptKey = result.slice(24, 40);
        this._vmmCryptIv = result.slice(40, 56);
    }
}
