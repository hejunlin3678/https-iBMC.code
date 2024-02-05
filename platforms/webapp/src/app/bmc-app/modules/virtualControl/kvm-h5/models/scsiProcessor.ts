import { MassStorageDevice } from './massStorageDevice';

export class SCSIProcessor {
    public PACK_HEAD_SIZE =  12;
    public TYPE_COMMOND =  0;
    public TYPE_DATA =  1;
    public STATE_DATA_CONTINUE =  1;
    public STATE_DATA_END =  3;
    public TEST_UNIT_READY_EXP =  0x4a;
    public TEST_UNIT_READY =  0x00;
    public REZERO_UNIT =  0x01;
    public REQUEST_SENSE =  0x03;
    public FORMAT_UNIT =  0x04;
    public INQUIRY =  0x12;
    public START_STOP =  0x1b;
    public SEND_DIAGNOSTIC =  0x1d;
    public ALLOW_MEDIUM_REMOVAL =  0x1e;
    public READ_FORMAT_CAPACITY =  0x23;
    public READ_CAPACITY =  0x25;
    public READ_10 =  0x28;
    public SEEK_10 =  0x2b;
    public VERIFY =  0x2f;
    public READ_TOC =  0x43;
    public MODE_SELECT_10 =  0x55;
    public MODE_SENSE_10 =  0x5a;
    public READ_12 =  0xa8;

    public MEDIUM_NOT_PRESENT = 0x3a;
    public NOT_READY_TRANSITION = 0x28;
    public INVALID_FIELD_IN_CDB = 0x24;
    public INVALID_LUN = 0x25;
    public LBA_OUT_OF_RANGE = 0x21;
    public WRITE_PROTECT = 0x27;
    public TOTAL_BLOCKS = 0x0b40;

    // scsi sense key
    public NO_SENSE = 0;
    public NOT_READY = 2;
    public ILLEGAL_REQUEST = 5;
    public UNIT_ATTENTION = 6;
    public DATA_PROTECT = 7;

    public _vm = null;
    public _device = null;
    public _protocol = null;
    public _usbId = null;
    public _packetSize = null;
    public _blockSize = null;
    public _transCmd = null;
    public _endCmd = null;
    public _sensekey = null;
    public _inquiry = null;
    public capacityList = null;



    constructor (vm, file, protocol) {
        this._vm = vm;
        this._device = new MassStorageDevice(this, file);
        this._protocol = protocol;
        this._usbId = 0;
        this._packetSize = 0;
        this._blockSize = 0;
        this._transCmd = 0;
        this._endCmd = 0;
        this._sensekey = new Uint8Array([0x70, 0, 0, 0x06, 0, 0, 0, 0x0a, 0, 0, 0, 0, 0x29, 0, 0, 0, 0, 0]);
        this._inquiry = null;
        this.capacityList = new Uint8Array([0, 0, 0, 0x10, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0x0b, 0x40, 0, 0, 2, 0]);
        this._Init();
    }

    _Init () {
        if (this._protocol === 0) {
            this._packetSize = 32768;
            this._blockSize = 2048;
            this._transCmd = this._vm.TRANS_SFF;
            this._endCmd = this._vm.SFF_END;
            this._inquiry = new Uint8Array([
                0x05,
                0x80,
                0,
                0x21,
                0x1f,
                0,
                0,
                0,
                0x56,
                0x69,
                0x72,
                0x74,
                0x75,
                0x61,
                0x6c,
                0x20,
                0x44,
                0x56,
                0x44,
                0x2d,
                0x52,
                0x4f,
                0x4d,
                0x20,
                0x56,
                0x4d,
                0x20,
                0x31,
                0x2e,
                0x31,
                0x2e,
                0x30,
                0x20,
                0x32,
                0x32,
                0x35
            ]);
        } else {
            this._packetSize = 4096;
            this._blockSize = 512;
            this._transCmd = this._vm.TRANS_UFI;
            this._endCmd = this._vm.UFI_END;
            this._inquiry = new Uint8Array([
                0,
                0x80,
                0,
                1,
                0x1f,
                0,
                0,
                0,
                0x56,
                0x69,
                0x72,
                0x74,
                0x75,
                0x61,
                0x6c,
                0x20,
                0x46,
                0x4c,
                0x4f,
                0x50,
                0x50,
                0x59,
                0x20,
                0x56,
                0x4d,
                0x20,
                0x31,
                0x2e,
                0x31,
                0x2e,
                0x30,
                0x20,
                0x20,
                0x20,
                0x20,
                0x20
            ]);
        }
    }

    _SetSenseKeys (senseKey, asc, ascq, info) {
        if (info === 0) {
            this._sensekey[0] = 0x70;
            this._sensekey[3] = 0x00;
            this._sensekey[4] = 0x00;
            this._sensekey[5] = 0x00;
            this._sensekey[6] = 0x00;
        } else {
            this._sensekey[0] = 0xf0;
            this._sensekey[3] = (info >> 24) & 0xff;
            this._sensekey[4] = (info >> 16) & 0xff;
            this._sensekey[5] = (info >> 8) & 0xff;
            this._sensekey[6] = info & 0xff;
        }
        this._sensekey[2] = senseKey;
        this._sensekey[12] = asc;
        this._sensekey[13] = ascq;
    }

    _CommandFinish (command) {
        let result = 0;
        if ((this._sensekey[2] !== 0 && command[0] !== this.REQUEST_SENSE) || this.TEST_UNIT_READY_EXP === command[0]) {
            result = 1;
        }
        const pack = new Uint8Array(this.PACK_HEAD_SIZE);
        pack[0] = this._endCmd;
        pack[1] = result;
        pack[2] = 0;
        pack[3] = this._usbId;

        this._vm._Send(pack);
    }

    _SendData (data, length, last) {
        let pos = 0;
        let dataPack = null;
        let headPack = null;
        let currentLength = 0;

        if (length === 0 && last) {
            headPack = this._DataPacket(this.TYPE_DATA, this.STATE_DATA_END, length);
            this._vm._Send(headPack);
        }

        while (length > 0) {
            if (this._packetSize < length) {
                currentLength = this._packetSize;
                headPack = this._DataPacket(this.TYPE_DATA, this.STATE_DATA_CONTINUE, currentLength);
            } else {
                currentLength = length;
                headPack = this._DataPacket(
                    this.TYPE_DATA,
                    last ? this.STATE_DATA_END : this.STATE_DATA_CONTINUE,
                    currentLength
                );
            }

            if (this._vm._security.isVmmEncryption()) {
                const encryptData = this._vm._security.vmmEncrypt(data.slice(pos, pos + currentLength));
                const dataLength = new Uint8Array(4);
                dataLength[0] = (currentLength >> 24) & 0xff;
                dataLength[1] = (currentLength >> 16) & 0xff;
                dataLength[2] = (currentLength >> 8) & 0xff;
                dataLength[3] = currentLength & 0xff;

                headPack[4] = ((encryptData.length + 4) >> 24) & 0xff;
                headPack[5] = ((encryptData.length + 4) >> 16) & 0xff;
                headPack[6] = ((encryptData.length + 4) >> 8) & 0xff;
                headPack[7] = (encryptData.length + 4) & 0xff;
                dataPack = new Uint8Array(headPack.length + 4 + encryptData.length);

                dataPack.set(headPack);
                dataPack.set(dataLength, headPack.length);
                dataPack.set(encryptData, headPack.length + 4);
            } else {
                dataPack = new Uint8Array(headPack.length + currentLength);
                for (let i = 0; i < headPack.length; i++) {
                    dataPack[i] = headPack[i];
                }
                for (let i = 0, j = i + headPack.length, k = pos + i; i < currentLength; i++, j++, k++) {
                    dataPack[j] = data[k];
                }
            }
            this._vm._Send(dataPack);
            pos += currentLength;
            length -= currentLength;
        }
    }

    _DataPacket (type, state, length) {
        const pack = new Uint8Array(this.PACK_HEAD_SIZE);
        pack[0] = this._transCmd;
        pack[1] = state * 16 + type;
        pack[3] = this._usbId;
        pack[4] = (length >> 24) & 0xff;
        pack[5] = (length >> 16) & 0xff;
        pack[6] = (length >> 8) & 0xff;
        pack[7] = length & 0xff;
        return pack;
    }

    _Inquiry (command) {
        if ((command[1] & 0xe0) !== 0) {
            this._SetSenseKeys(this.ILLEGAL_REQUEST, this.INVALID_LUN, 0, 0);
        } else if ((command[1] & 0x01) !== 0) {
            this._SetSenseKeys(this.ILLEGAL_REQUEST, this.INVALID_FIELD_IN_CDB, 0, 0);
        } else {
            this._SendData(this._inquiry, this._inquiry.length, true);
            this._SetSenseKeys(0, 0, 0, 0);
        }
    }

    _Read (command) {
        const logicalBlockAddr = command[2] * 0x1000000 + command[3] * 0x10000 + command[4] * 0x100 + command[5];
        let logicalBlockNum = 0;
        if (command[0] === 0x28) {
            logicalBlockNum = command[7] * 0x100 + command[8];
        } else {
            logicalBlockNum = command[6] * 0x1000000 + command[7] * 0x10000 + command[8] * 0x100 + command[9];
        }
        const start = logicalBlockAddr * this._blockSize;
        const length = logicalBlockNum * this._blockSize;
        this._device.Read(start, length, command[0]);
    }

    _ReadCapacity () {
        const pack = new Uint8Array(8);
        switch (this._device.TestUnitReady()) {
            case this._device.STATE_NOTPRESENT:
                this._SetSenseKeys(this.NO_SENSE, this.MEDIUM_NOT_PRESENT, 0, 0);
                break;
            case this._device.STATE_CHANGE:
                this._SetSenseKeys(this.UNIT_ATTENTION, this.NOT_READY_TRANSITION, 0, 0);
                break;
            case this._device.STATE_READY:
                const lba = (this._protocol === this._vm.PROTOCOL_UFI) ?
                    (this.TOTAL_BLOCKS - 1) : (this._device.GetDeviceSize() / this._blockSize - 1);
                pack[0] = (lba >> 24) & 0xff;
                pack[1] = (lba >> 16) & 0xff;
                pack[2] = (lba >> 8) & 0xff;
                pack[3] = lba & 0xff;
                pack[4] = (this._blockSize >> 24) & 0xff;
                pack[5] = (this._blockSize >> 16) & 0xff;
                pack[6] = (this._blockSize >> 8) & 0xff;
                pack[7] = this._blockSize & 0xff;
                this._SetSenseKeys(0, 0, 0, 0);
                this._SendData(pack, pack.length, true);
                break;
            default:
                break;
        }
    }

    _ReadFormatCapacity () {
        if (this._device.STATE_CHANGE === this._device.TestUnitReady()) {
            this._device.SetDeviceState(this._device.STATE_READY);
            this._SetSenseKeys(this.UNIT_ATTENTION, this.NOT_READY_TRANSITION, 0, 0);
        } else {
            this._SetSenseKeys(0, 0, 0, 0);
        }
        const totalBlocks = this._protocol === this._vm.PROTOCOL_UFI ? this.TOTAL_BLOCKS : this._device.GetDeviceSize() / this._blockSize;
        this.capacityList[4] = (totalBlocks >> 24) & 0xff;
        this.capacityList[5] = (totalBlocks >> 16) & 0xff;
        this.capacityList[6] = (totalBlocks >> 8) & 0xff;
        this.capacityList[7] = totalBlocks & 0xff;
        this.capacityList[9] = (this._blockSize >> 16) & 0xff;
        this.capacityList[10] = (this._blockSize >> 8) & 0xff;
        this.capacityList[11] = this._blockSize & 0xff;
        this._SendData(this.capacityList, this.capacityList.length, true);
    }

    _ModeSense (command) {
        const pc = (command[2] >> 6) & 0x03;
        const pageCode = command[2] & 0x3f;
        const dataBuffer = new Uint8Array(256);
        let size = 8;
        let mediumType = this._protocol === this._vm.PROTOCOL_SFF ? 0x70 : 0x94;
        mediumType = this._device.TestUnitReady() === this._device.STATE_NOTPRESENT ? 1 : mediumType;
        dataBuffer[2] = mediumType;
        dataBuffer[3] = 0x80;
        if (pageCode === 0x01 || pageCode === 0x3f) {
            dataBuffer[size + 0] = 0x01;
            dataBuffer[size + 1] = 0x0a;
            dataBuffer[size + 2] = 0x00;
            dataBuffer[size + 3] = 0x03;
            dataBuffer[size + 4] = 0x00;
            dataBuffer[size + 5] = 0x00;
            dataBuffer[size + 6] = 0x00;
            dataBuffer[size + 7] = 0x00;
            dataBuffer[size + 8] = 0x03;
            dataBuffer[size + 9] = 0x00;
            dataBuffer[size + 10] = 0x00;
            dataBuffer[size + 11] = 0x00;
            size += 12;
        }

        if (pageCode === 0x05 || pageCode === 0x3f) {
            dataBuffer[size + 0] = 0x05;
            dataBuffer[size + 1] = 0x1e;
            dataBuffer[size + 2] = 0x03;
            dataBuffer[size + 3] = 0xe8;
            dataBuffer[size + 4] = 0x02;
            dataBuffer[size + 5] = 0x12;
            dataBuffer[size + 6] = 0x02;
            dataBuffer[size + 7] = 0x00;
            dataBuffer[size + 8] = 0x00;
            dataBuffer[size + 9] = 0x50;
            dataBuffer[size + 10] = 0x00;
            dataBuffer[size + 11] = 0x00;
            dataBuffer[size + 12] = 0x00;
            dataBuffer[size + 13] = 0x00;
            dataBuffer[size + 14] = 0x00;
            dataBuffer[size + 15] = 0x00;
            dataBuffer[size + 16] = 0x00;
            dataBuffer[size + 17] = 0x00;
            dataBuffer[size + 18] = 0x00;
            dataBuffer[size + 19] = 0x08;
            dataBuffer[size + 20] = 0x1e;
            dataBuffer[size + 21] = 0x00;
            dataBuffer[size + 22] = 0x00;
            dataBuffer[size + 23] = 0x00;
            dataBuffer[size + 24] = 0x00;
            dataBuffer[size + 25] = 0x00;
            dataBuffer[size + 26] = 0x00;
            dataBuffer[size + 27] = 0x00;
            dataBuffer[size + 28] = 0x02;
            dataBuffer[size + 29] = 0x58;
            dataBuffer[size + 30] = 0x00;
            dataBuffer[size + 31] = 0x00;
            size += 32;
        }
        if (pageCode === 0x1b || pageCode === 0x3f) {
            dataBuffer[size + 0] = 0x1b;
            dataBuffer[size + 1] = 0x0a;
            dataBuffer[size + 2] = 0x80;
            dataBuffer[size + 3] = 0x01;
            dataBuffer[size + 4] = 0x00;
            dataBuffer[size + 5] = 0x00;
            dataBuffer[size + 6] = 0x00;
            dataBuffer[size + 7] = 0x00;
            dataBuffer[size + 8] = 0x00;
            dataBuffer[size + 9] = 0x00;
            dataBuffer[size + 10] = 0x00;
            dataBuffer[size + 11] = 0x00;
            size += 12;
        }

        if (pageCode === 0x1c || pageCode === 0x3f) {
            dataBuffer[size + 0] = 0x1c;
            dataBuffer[size + 1] = 0x06;
            dataBuffer[size + 2] = 0x00;
            dataBuffer[size + 3] = 0x05;
            dataBuffer[size + 4] = 0x00;
            dataBuffer[size + 5] = 0x00;
            dataBuffer[size + 6] = 0x00;
            dataBuffer[size + 7] = 0x00;
            size += 8;
        }

        if (this._protocol === 0) {
            size = 8;
            dataBuffer[1] = 6;
        } else {
            dataBuffer[1] = size - 2;
        }

        const data = dataBuffer.slice(0, size);
        this._SendData(data, data.length, true);
        this._SetSenseKeys(0, 0, 0, 0);
    }

    _ReadTOC(command) {
        const isMSF = (command[1] & 0x02) === 2;
        let format = command[2] & 0x07;
        if (format === 0) {
            format = (command[9] >> 6) & 0x03;
        }
        const allocLength = command[7] * 256 + command[8];
        const startTrack = command[6];
        const dataBuffer = new Uint8Array(32);
        const totalBlocks = parseInt(String(this._device.GetDeviceSize() / this._blockSize), 10);
        const i = totalBlocks / 75 + 2;
        const m = i / 60;
        const s = i % 60;
        const f = totalBlocks % 75;
        let len = 4;

        if (format === 0) {
            if (startTrack > 1 && startTrack !== 0xaa) {}
            dataBuffer[2] = 1;
            dataBuffer[3] = 1;
            if (startTrack <= 1) {
                dataBuffer[len++] = 0;
                dataBuffer[len++] = 20;
                dataBuffer[len++] = 1;
                dataBuffer[len++] = 0;
                dataBuffer[len++] = 0;
                dataBuffer[len++] = 0;
                dataBuffer[len++] = isMSF ? 2 : 0;
                dataBuffer[len++] = 0;
            }
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 20;
            dataBuffer[len++] = 0xaa;
            dataBuffer[len++] = 0;
            if (!isMSF) {
                dataBuffer[len++] = (totalBlocks >> 24) & 0xff;
                dataBuffer[len++] = (totalBlocks >> 16) & 0xff;
                dataBuffer[len++] = (totalBlocks >> 8) & 0xff;
                dataBuffer[len++] = totalBlocks & 0xff;
            } else {
                dataBuffer[len++] = 0;
                dataBuffer[len++] = m;
                dataBuffer[len++] = s;
                dataBuffer[len++] = f;
            }
        } else if (format === 1) {
            dataBuffer[2] = 1;
            dataBuffer[3] = 1;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
            dataBuffer[len++] = 0;
        } else if (format === 2) {
            dataBuffer[2] = 1;
            dataBuffer[3] = 1;
            for (let j = 0; j < 4; j++) {
                dataBuffer[len++] = 1;
                dataBuffer[len++] = 0x14;
                dataBuffer[len++] = 0;
                if (j < 3) {
                    dataBuffer[len++] = 0xa0 + j;
                } else {
                    dataBuffer[len++] = 1;
                }
                dataBuffer[len++] = 0;
                dataBuffer[len++] = 0;
                dataBuffer[len++] = 0;
                if (j < 2) {
                    dataBuffer[len++] = 0;
                    dataBuffer[len++] = 1;
                    dataBuffer[len++] = 0;
                    dataBuffer[len++] = 0;
                } else if (j === 2) {
                    if (isMSF) {
                        dataBuffer[len++] = 0;
                        dataBuffer[len++] = m;
                        dataBuffer[len++] = s;
                        dataBuffer[len++] = f;
                    } else {
                        dataBuffer[len++] = (totalBlocks >> 24) & 0xff;
                        dataBuffer[len++] = (totalBlocks >> 16) & 0xff;
                        dataBuffer[len++] = (totalBlocks >> 8) & 0xff;
                        dataBuffer[len++] = totalBlocks & 0xff;
                    }
                } else {
                    dataBuffer[len++] = 0;
                    dataBuffer[len++] = 0;
                    dataBuffer[len++] = 0;
                    dataBuffer[len++] = 0;
                }
            }
        }
        dataBuffer[0] = ((len - 2) >> 8) & 0xff;
        dataBuffer[1] = (len - 2) & 0xff;
        this._SetSenseKeys(0, 0, 0, 0);
        if (allocLength < len) {
            len = allocLength;
        }
        this._SendData(dataBuffer.slice(0, len), len, true);
    }

    _StartStop(command) {
        let isEject = false;
        let isStart = false;
        if ((command[4] & 0x02) === 0x02) {
            isEject = true;
        }
        if ((command[4] & 0x01) === 0x01) {
            isStart = true;
        }

        if (isEject && !isStart) {
            this._device.SetDeviceState(this._device.STATE_NOTPRESENT);
            this._device._Init(null);
            if (this._protocol === 0) {
                this._vm._cdromInsert.text = 'REMOTE_INSERT';
                if (this._vm._client.scope.cdDvdSelected.value === 'iso') {
                    this._vm._cdromImage.styleOptions.disable = false;
                } else {
                    this._vm._cdromDirectory.styleOptions.disable = false;
                }
            } else {
                this._vm._floppyImage.styleOptions.disable = false;
                this._vm._floppyInsert.text = 'REMOTE_INSERT';
            }
        } else if (isEject && isStart) {
            this._device.SetDeviceState(this._device.STATE_READY);
        }
        this._SetSenseKeys(0, 0, 0, 0);
    }

    _TestUnitReady() {
        switch (this._device.TestUnitReady()) {
            case this._device.STATE_CHANGE:
                this._SetSenseKeys(this.UNIT_ATTENTION, this.NOT_READY_TRANSITION, 0, 0);
                break;
            case this._device.STATE_READY:
                this._SetSenseKeys(0, 0, 0, 0);
                break;
            case this._device.STATE_BAD:
                this._SetSenseKeys(this.NOT_READY, 0x30, 0, 0);
                break;
            case this._device.STATE_NOTREADY:
                this._SetSenseKeys(this.NOT_READY, 0x04, 0, 0);
                break;
            case this._device.STATE_NOTPRESENT:
            default:
                this._SetSenseKeys(this.NOT_READY, this.MEDIUM_NOT_PRESENT, 0, 0);
        }
    }

    process (usbId, cmd) {
        this._usbId = usbId;
        switch (cmd[0]) {
            case this.READ_10:
            case this.READ_12:
                this._Read(cmd);
                return;
            case this.INQUIRY:
                this._Inquiry(cmd);
                break;
            case this.REQUEST_SENSE:
                this._SendData(this._sensekey, this._sensekey.length, true);
                break;
            case this.READ_CAPACITY:
                this._ReadCapacity();
                break;
            case this.READ_FORMAT_CAPACITY:
                this._ReadFormatCapacity();
                break;
            case this.MODE_SENSE_10:
                this._ModeSense(cmd);
                break;
            case this.READ_TOC:
                this._ReadTOC(cmd);
                break;
            case this.START_STOP:
                this._StartStop(cmd);
                break;
            case this.TEST_UNIT_READY:
            case this.TEST_UNIT_READY_EXP:
                this._TestUnitReady();
                break;
            case this.FORMAT_UNIT:
                this._SetSenseKeys(this.DATA_PROTECT, this.WRITE_PROTECT, 0, 0);
                break;
            case this.MODE_SELECT_10:
            case this.ALLOW_MEDIUM_REMOVAL:
            case this.SEEK_10:
                this._SetSenseKeys(0, 0, 0, 0);
                break;
            case this.SEND_DIAGNOSTIC:
            case this.REZERO_UNIT:
            case this.VERIFY:
                this._SetSenseKeys(0, 0, 0, 0);
                break;
            default:
                this._SetSenseKeys(this.ILLEGAL_REQUEST, 36, 0, 0);
                break;
        }
        this._CommandFinish(cmd);
    }
}

