export class MassStorageDevice {
    public _file = null;
    public _size = null;
    public _state = null;
    public _zeroOffset = null;
    public _owner;
    public STATE_NOTPRESENT =  0;
    public STATE_CHANGE =  2;
    public STATE_READY =  3;
    public STATE_NOTREADY =  4;
    public STATE_BAD =  5;
    public _reader = null;
    constructor(owner, file) {
        this._owner = owner;
        this._Init(file);
    }


    isEject () {
        if (this._state === this.STATE_READY) {
            return false;
        } else {
            return true;
        }
    }

    _Init (file) {
        if (file) {
            this._file = file;
            this._size = file.size;
            this._state = this.STATE_NOTPRESENT;
            this._zeroOffset = 0;
        } else {
            this._file = null;
            this._size = -1;
            this._zeroOffset = 0;
            return;
        }

        const blob = this._file.slice(0, 512);
        this._reader = new FileReader();
        const self = this;
        this._reader.onload = function (e) {
            const head = new Uint8Array(e.target.result);
            if (
                head[0] === 67 &&
                head[1] === 80 &&
                head[2] === 81 &&
                head[3] === 82 &&
                head[4] === 70 &&
                head[5] === 66 &&
                head[6] === 76 &&
                head[7] === 79
            ) {
                this._zeroOffset = (head[14] & 0xff) | (head[15] << 8);
            }
        };
        this._reader.readAsArrayBuffer(blob);
    }

    GetDeviceSize () {
        if (this._state === this.STATE_READY) {
            return this._size - this._zeroOffset;
        } else {
            return -1;
        }
    }

    Read (start, length, cmd) {
        let data = null;
        if (start >= 0 && start + length + this._zeroOffset <= this._size && this.STATE_READY === this._state) {
            const blob = this._file.slice(start + this._zeroOffset, start + length + this._zeroOffset);
            this._reader = new FileReader();
            const self = this;
            this._reader.onload = (e) => {
                data = new Uint8Array(e.target.result);
                self._owner._SendData(data, data.length, true);
                self._owner._SetSenseKeys(0, 0, 0, 0);
                self._owner._CommandFinish(cmd);
            };
            this._reader.readAsArrayBuffer(blob);
        } else {
            this._owner._SetSenseKeys(5, 0x21, 0, 0);
            this._owner._CommandFinish(cmd);
        }
    }

    TestUnitReady () {
        let curState = this.STATE_READY;
        if (this._size < 0) {
            if (this.STATE_READY === this._state) {
                curState = this.STATE_CHANGE;
            } else if (this.STATE_CHANGE === this._state || this.STATE_NOTPRESENT === this._state) {
                curState = this.STATE_NOTPRESENT;
            }
        } else {
            if (this.STATE_NOTPRESENT === this._state) {
                curState = this.STATE_CHANGE;
            } else {
                curState = this.STATE_READY;
            }
        }
        this._state = curState;
        return curState;
    }

    SetDeviceState (state) {
        this._state = state;
    }
}
