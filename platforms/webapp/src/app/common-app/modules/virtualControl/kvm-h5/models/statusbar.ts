export class StatusBar {
    public _sendBytes = 0;
    public _reciveBytes = 0;
    public _frameRate = 0;
    public _owner;
    public _IP;
    public _SN;
    constructor(owner, IP, SN) {
        this._owner = owner;
        this._IP = IP;
        this._SN = SN;
        document.getElementById('status_ip').innerText = this._IP;
        document.getElementById('status_sn').innerHTML = this._SN; // SN可能含有特殊字符
        document.getElementById('status_sn').setAttribute('title', document.getElementById('status_sn').innerText);
        document.getElementById('status_ip').setAttribute('title', this._IP);
        setInterval(() => {
            this._ResetStatus();
        }, 1000);
    }

    _ResetStatus() {
        document.getElementById('send_bytes').innerText = String(this._sendBytes);
        document.getElementById('recive_bytes').innerText = String(this._reciveBytes);
        document.getElementById('frame_rate').innerText = String(this._frameRate);
        this._sendBytes = 0;
        this._reciveBytes = 0;
        this._frameRate = 0;
    }

    updateSend(sendBytes) {
        if (typeof sendBytes === 'number') {
            this._sendBytes += sendBytes;
        }
    }

    updateRecive(reciveBytes) {
        if (typeof reciveBytes === 'number') {
            this._reciveBytes += reciveBytes;
        }
    }

    updateFrame(frameRate) {
        this._frameRate += frameRate;
    }
}
