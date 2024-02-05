import { packet } from './packet';
export class Comunication {
    public _ip;
    public _port;
    public _client;
    public _sock = null;
    public _heartTimer = null;
    public _waitTimer = null;
    public _securityWarning = 1;
    public _reconnectTimes = 0;
    public _reconnectState = 0;
    public _reconnectInvalidUserTimes = 0;
    public _established = 0;
    public _remainKvmKeyAndIv = null;
    constructor(client, ip, port) {
        this._client = client;
        this._ip = ip;
        this._port = port;
        this._CreatConnection();
    }
    _CreatConnection() {
        this._sock = new WebSocket('wss://' + this._ip + ':' + this._port + '/websocket');
        this._sock.binaryType = 'arraybuffer';
        this._sock.onopen = (e) => {
            this._WSOpen(e);
        };
        this._sock.onclose = (e) => {
            this._WSClose(e);
        };
        this._sock.onerror = (e) => {
            this._WSError(e);
        };
        this._sock.onmessage = (e) => {
            this._WSMessage(e);
        };
    }

    _WSOpen(event) {
        this._securityWarning = 0;
        this._reconnectTimes = 0;
        clearInterval(this._waitTimer);
        this._Send(packet.getIVPatternPacket(0, this._client._security));
    }

    _WSClose(event) {
        clearInterval(this._heartTimer);
        if (this._sock != null) {
            this._sock.onopen = null;
            this._sock.onclose = null;
            this._sock.onerror = null;
            this._sock.onmessage = null;
            this._sock = null;
        }
        this._client._toolbar._virtualMedia._Reset();
        if (this._established === 1) {
            if (this._reconnectTimes++ < 16) {
                this._reconnectState = 1;
                this._CreatConnection();
            } else {
                this._client.showMessage('REMOTE_ERR_NETWORK_INTERRUPT');
            }
        }
    }

    _WSError(event) {
        if (this._securityWarning === 1) {
            this._securityWarning = 0;
            this._ResolveSecurityWarning();
        }
    }

    _WSMessage(event) {
        this._SendHeartBeat();
        const data = new Uint8Array(event.data);
        this._DataParse(data, data.length);
    }

    _ResolveSecurityWarning() {
        const self = this;
        let interval = null;
        let times = 0;

        function receiver(event) {
            if (event.data === 'Certificate OK.') {
                clearInterval(interval);
                window.removeEventListener('message', receiver);
                self._CreatConnection();
            }
        }
        const targetOrigin = 'https://' + this._ip + ':' + this._port;
        const popupWin = window.open(targetOrigin, '_blank');
        window.addEventListener('message', receiver);

        interval = setInterval(() => {
            popupWin.postMessage('hello', targetOrigin);
            if (times++ > 150) {
                clearInterval(interval);
                window.removeEventListener('message', receiver);
                this._client.showMessage('REMOTE_ERR_VERIFY_TIMEOUT');
            }
        }, 200);
    }

    _SendHeartBeat() {
        const data = packet.heartBeatPacket(0, this._client._security);
        this._Send(data);
    }
    _Send(data) {
        if (this._sock != null && Number(this._sock.readyState) === 1) {
            this._client._statusbar.updateSend(data.length);
            this._sock.send(data.buffer);
        }
    }
    _DataParse(bytes, length) {
        let head = 0;
        let start = 0;
        let state = 0;
        let dlen = 0;
        let result;
        this._client._statusbar.updateRecive(length);
        while (start < length) {
            switch (state) {
                case 0:
                    head <<= 8;
                    head += bytes[start++] & 0x00ff;
                    if ((head & 0x00ffffff) === 0x00fef600) {
                        state = 1;
                    }
                    break;
                case 1:
                    dlen = bytes[start++] & 0x00ff;
                    if (dlen < 3 || dlen > 250) {
                        state = 0;
                        head = dlen;
                    } else {
                        state = 2;
                    }
                    break;
                case 2:
                    result = bytes.slice(start + 2, start + dlen);
                    this._MsgDispatch(result);
                    start = start + dlen;
                    state = 0;
                    head = 0;
                    break;
                default:
                    state = 0;
                    head = 0;
                    break;
            }
        }
    }

    _MsgDispatch(data) {
        switch (data[0]) {
            case packet.H_IV_PATTERN:
                let sendData = null;
                this._remainKvmKeyAndIv = data.slice(2, 18);
                if (this._client._security.needConsultation()) {
                    sendData = packet.getSuitePacket(0, this._client._security);
                    this._Send(sendData);
                } else {
                    this._client._security.generateAuthCode();
                    sendData = packet.connectPacket(0, this._client._security);
                    this._Send(sendData);
                    this._established = 1;
                }
                break;
            case packet.H_FLOPPY_STATE:
                this._client._toolbar.setFloppyState(data.slice(2, data.length));
                break;
            case packet.C_CMD_RSP_SUITE:
                this._ProcSuiteList(data);
                break;
            case packet.H_CMD_IMAGE:
                this._client._display.draw(data);
                break;
            case packet.H_CMD_KEYBOARD_STATE:
                this._client._toolbar.setKeyboardLight(data[2]);
                break;
            case packet.H_CMD_MOUSE_MODE:
                this._client._toolbar.setBootOption(data[1]);
                this._client._toolbar.setMouseAcceleration(data[2]);
                break;
            case packet.H_CMD_DEFINITION:
                this._client._toolbar.setDefinition(data[2]);
                break;
            case packet.H_CMD_REPORT_KVMKEY:
                const self = this;
                let cdromState = false;
                let floppyState = false;
                if (data.length > 128) {
                    this._reconnectInvalidUserTimes = 0;
                    this._client._security.generateReconnectKey(data.slice(2, 130));
                    cdromState = this._client._toolbar._virtualMedia._needReconnectCdrom;
                    floppyState = this._client._toolbar._virtualMedia._needReconnectFloppy;
                    if (cdromState && floppyState) {
                        this._client._toolbar._virtualMedia._Reset();
                        this._client._toolbar._virtualMedia._cdromConnect.click();
                        this._client._toolbar._virtualMedia._floppyConnect.click();
                    } else if (cdromState) {
                        this._client._toolbar._virtualMedia._Reset();
                        this._client._toolbar._virtualMedia._cdromConnect.click();
                    } else if (floppyState) {
                        this._client._toolbar._virtualMedia._Reset();
                        this._client._toolbar._virtualMedia._floppyConnect.click();
                    }
                } else {
                    this._client._security.generateKvmKeyAndIv(data.slice(2, 50), this._remainKvmKeyAndIv);
                }
                break;
            case packet.H_CMD_VMM_PORT:
                this._client._toolbar._virtualMedia.parserVmmPort(data.slice(2, data.length), 0);
                break;
            case packet.H_CMD_VMM_CODEKEY:
                this._client._toolbar._virtualMedia.parserVmmCodekey(data.slice(2, data.length), 0);
                break;
            case packet.H_CMD_NO_PRIVILEGE:
                this._NoPrivilege(data[2]);
                break;
            case packet.H_CMD_CONNECT_STATE:
                this._ConnectState(data[2]);
                break;
            case packet.H_CMD_UMS_NOTIFY_STATE:
                this._client._toolbar._ShowDialog(data.slice(2, data.length));
                break;
            case packet.H_CMD_VMM_STATE:
                this._client._toolbar._virtualMedia.parserVmmState(data.slice(2, data.length), 0);
                break;
            default:
        }
    }

    _ProcSuiteList(data) {
        let pack = null;
        const count = data[2];
        let iterations = 0;
        let hmac = 0;
        const offset = 3;
        let i = 0;
        while (i < count && (i + 1) * 5 + offset <= data.length) {
            hmac = data[i * 5 + offset];
            iterations = data[i * 5 + offset + 1] * 0x1000000;
            iterations += data[i * 5 + offset + 2] * 0x10000;
            iterations += data[i * 5 + offset + 3] * 0x100;
            iterations += data[i * 5 + offset + 4];
            if (Number(hmac) === 3) {
                pack = packet.setSuitePacket(0, this._client._security, iterations, hmac);
                this._Send(pack);
                this._client._security.setHmac(hmac);
                this._client._security.setIterations(iterations);
                break;
            }
            i++;
        }

        this._client._security.generateAuthCode();
        pack = packet.connectPacket(0, this._client._security);
        this._Send(pack);
        this._established = 1;
    }

    _NoPrivilege(index) {
        let des = null;
        switch (index) {
            case 0:
                des = 'REMOTE_PRI_NO_KVM';
                break;
            case 1:
                des = 'REMOTE_PRI_NO_POWER';
                break;
            case 2:
                des = 'REMOTE_PRI_NO_VMM';
                this._client._toolbar._virtualMedia.parserVmmCodekey(null, 1);
                break;
            case 3:
                des = 'REMOTE_PRI_NO_SETTING';
                break;
            default:
                return;
        }

        this._client.showMessage(des);
    }

    _ConnectState(state) {
        let des = null;
        let needReconnect = 0;
        switch (state) {
            case 2:
                des = 'REMOTE_ERR_OVER_MAX';
                break;
            case 3:
                if (this._reconnectState === 1) {
                    if (this._reconnectInvalidUserTimes++ > 5) {
                        des = 'REMOTE_ERR_NETWORK_INTERRUPT';
                    } else {
                        needReconnect = 1;
                    }
                } else {
                    des = 'REMOTE_ERR_INVALID_USER';
                }

                break;
            case 4:
                des = 'REMOTE_ERR_OUT_RANGE';
                break;
            case 5:
                des = 'REMOTE_ERR_SERVER_DISABLED';
                break;
            case 7:
                des = 'REMOTE_ERR_USER_DEL';
                break;
            case 0x11:
                des = 'REMOTE_ERR_TIMEOUT';
                break;
            default:
                return;
        }
        if (this._sock != null) {
            // 断开光驱及软驱
            if (this._client._toolbar._virtualMedia._cdromConnect.text === 'HOME_DISCONNECT') {
                this._client._toolbar._virtualMedia._cdromConnect.click();
            }

            if (this._client._toolbar._virtualMedia._floppyConnect.text === 'HOME_DISCONNECT') {
                this._client._toolbar._virtualMedia._floppyConnect.click();
            }

            this._sock.close();
            this._sock.onopen = null;
            this._sock.onclose = null;
            this._sock.onerror = null;
            this._sock.onmessage = null;
            this._sock = null;
        }

        if (needReconnect === 1) {
            this._CreatConnection();
        }

        if (des != null) {
            this._client.showMessage(des);
        }
    }
}
