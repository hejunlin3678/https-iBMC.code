import { packet } from './packet';
import { UdfImageBuilder } from './udfImageBuilder';
import { SCSIProcessor } from './scsiProcessor';
export class VirtualMedia {
    public _ip = null;
    public _port = null;
    public _socket = null;
    public _state = null;
    public _timeout = 0;
    public _monitorTimer = null;
    public _disconnectSet = 0;
    public _connectingSet = 0;
    public _opendeviceSet = 0;
    public _establishedSet = 0;
    public _toolbar = null;
    public _client = null;
    public _security = null;
    public _ufiProcessor = null;
    public _sffProcessor = null;
    public _needReconnectCdrom = false;
    public _needReconnectFloppy = false;
    public _cdromImage = null;
    public _cdromDirectory = null;
    public _cdromConnect = null;
    public _cdromInsert = null;
    public _floppyImage = null;
    public _floppyConnect = null;
    public _floppyInsert = null;
    public _udfTimer = null;
    public _udfBuilder = null;
    public _securityWarning = 1;
    public _cdromImageFile = null;
    public _cdromDirectoryFile = null;
    public _floppyImageFile = null;

    public ACK = 0;
    public REGISTER = 1;
    public CREATE = 2;
    public TRANS_UFI = 3;
    public TRANS_SFF = 4;
    public CLOSE_VM = 5;
    public HEART = 6;
    public CLOSE_DEV = 7;
    public UFI_END = 0xfe;
    public SFF_END = 0xff;

    public PROTOCOL_SFF = 0;
    public PROTOCOL_UFI = 1;

    public STATE_IDLE = 0;
    public STATE_KEY_REQUEST = 1;
    public STATE_KEY_RESPONSE = 2;
    public STATE_STATE_REQUEST = 3;
    public STATE_STATE_RESPONSE = 4;
    public STATE_PORT_REQUEST = 5;
    public STATE_PORT_RESPONSE = 6;
    public STATE_CONNECTING = 7;
    public STATE_CONNECTED = 8;
    public STATE_AUTHING = 9;
    public STATE_ESTABLISHED = 10;
    public STATE_ERROR = 11;

    public CDROM_IMAGE_LINK = 1;
    public CDROM_DIRECTORY_LINK = 2;
    public FLOPPY_IMAGE_LINK = 4;

    public ACK_AUTH_OK = 0x00;
    public ACK_DEVICE_CREAT = 0x10;
    public ACK_DEVICE_FAIL = 0x11;
    public ACK_VM_BUSY = 0x31;
    public _sock = null;
    constructor(client, toolbar, security, ip, port) {
        this._ip = ip;
        this._port = port;
        this._state = this.STATE_IDLE;
        this._toolbar = toolbar;
        this._client = client;
        this._security = security;

        this._WidgetInit();
        this._EventBind();

    }
    _WaitUDFReady() {
        if (this._udfBuilder == null) {
            this._udfBuilder = new UdfImageBuilder();
            this._udfBuilder.setSourceFiles([this._cdromDirectoryFile]);
            this._udfBuilder.setImageIdentifier('UDF-Disc');
            this._udfBuilder.execute();
        } else if (this._udfBuilder.isISOReady()) {
            clearInterval(this._udfTimer);
            if (this._establishedSet & this.CDROM_DIRECTORY_LINK) {
                this._cdromDirectory.styleOptions.disable = false;
                this._cdromConnect.disable = false;
                this._cdromInsert.disable = false;
            } else {
                this._cdromDirectory.styleOptions.disable = false;
                this._cdromConnect.disable = false;
                this._cdromInsert.disable = true;
                this._client.scope.cdDvd[0].disable = false;
                this._client.scope.cdDvd[1].disable = false;
            }
        }
    }

    _WidgetInit() {
        const slef = this;
        this._cdromImage = this._client.scope.isoCdrom;
        this._cdromDirectory = this._client.scope.locationFile;
        this._cdromConnect = this._client.scope._cdromConnect;
        this._cdromInsert = this._client.scope._cdromInsert;
        this._floppyImage = this._client.scope.isoFloppy;
        this._floppyConnect = this._client.scope._floppyConnect;
        this._floppyInsert = this._client.scope._floppyInsert;

        this._cdromImage.uploaderConfig.onAddItemSuccess = (fileItem) => {
            fileItem.mouseenter = function () {
                if (this.disable) {
                    return;
                }
                this.isHover = !0;
            };
            slef._cdromImageFile = fileItem.file._file;
            if (slef._establishedSet & slef.CDROM_IMAGE_LINK) {
                slef._cdromConnect.disable = false;
                slef._cdromInsert.disable = false;
            } else {
                slef._cdromConnect.disable = false;
                slef._cdromInsert.disable = true;
            }
        };

        this._cdromDirectory.uploaderConfig.onAddItemSuccess = (fileItem) => {
            fileItem.mouseenter = function () {
                if (this.disable) {
                    return;
                }
                this.isHover = !0;
            };
            slef._cdromDirectoryFile = fileItem.file._file;
            slef._udfBuilder = null;
            slef._cdromDirectory.styleOptions.disable = true;
            slef._cdromConnect.disable = true;
            slef._cdromInsert.disable = true;
            slef._client.scope.cdDvd[0].disable = true;
            slef._client.scope.cdDvd[1].disable = true;
            slef._udfTimer = setInterval(() => {
                slef._WaitUDFReady();
            }, 500);
        };

        this._floppyImage.uploaderConfig.onAddItemSuccess = (fileItem) => {
            fileItem.mouseenter = function () {
                if (this.disable) {
                    return;
                }
                this.isHover = !0;
            };
            slef._floppyImageFile = fileItem.file._file;
            if (fileItem.file.size !== 0x200 * 0x0b40) {
                if (slef._establishedSet & slef.FLOPPY_IMAGE_LINK) {
                    slef._floppyConnect.disable = false;
                } else {
                    slef._floppyConnect.disable = true;
                }
                slef._floppyInsert.disable = true;
                slef._client.showMessage('REMOTE_ERR_FLOPPY_FORMAT');
            } else {
                if (slef._establishedSet & slef.FLOPPY_IMAGE_LINK) {
                    slef._floppyConnect.disable = false;
                    slef._floppyInsert.disable = false;
                } else {
                    slef._floppyConnect.disable = false;
                    slef._floppyInsert.disable = true;
                }
            }
        };
    }

    _EventBind() {
        const self = this;
        self._client.scope.rChange = () => {
            if (self._client.scope.cdDvdSelected.value === 'iso') {
                self._client.scope.isoCdrom.styleOptions.disable = false;
                self._client.scope.locationFile.styleOptions.disable = true;
                if (self._cdromImageFile !== null) {
                    self._cdromConnect.disable = false;
                    self._cdromInsert.disable = true;
                } else {
                    self._cdromConnect.disable = true;
                    self._cdromInsert.disable = true;
                }
            } else {
                self._client.scope.isoCdrom.styleOptions.disable = true;
                self._client.scope.locationFile.styleOptions.disable = false;
                if (self._cdromDirectoryFile !== null) {
                    self._cdromConnect.disable = false;
                    self._cdromInsert.disable = true;
                } else {
                    self._cdromConnect.disable = true;
                    self._cdromInsert.disable = true;
                }
            }
        };
        this._cdromConnect.click = () => {
            const flag =
                self._client.scope.cdDvdSelected.value === 'iso' ? self.CDROM_IMAGE_LINK : self.CDROM_DIRECTORY_LINK;
            if (self._establishedSet & self.CDROM_IMAGE_LINK || self._establishedSet & self.CDROM_DIRECTORY_LINK) {
                self._disconnectSet = self._disconnectSet | flag;
                self._needReconnectCdrom = false;
                if (self._monitorTimer == null) {
                    self._monitorTimer = setInterval(() => {
                        self._VMMLinkMonitor();
                    }, 1000);
                }
            } else {
                self._cdromImage.styleOptions.disable = true;
                self._cdromDirectory.styleOptions.disable = true;
                self._cdromConnect.disable = true;
                self._cdromInsert.disable = true;
                self._client.scope.cdDvd[0].disable = true;
                self._client.scope.cdDvd[1].disable = true;
                self._connectingSet = self._connectingSet | flag;
                if (self._monitorTimer == null) {
                    self._monitorTimer = setInterval(() => {
                        self._VMMLinkMonitor();
                    }, 1000);
                }
            }
        };
        this._cdromInsert.click = () => {
            self._cdromConnect.disable = true;
            self._cdromInsert.disable = true;
            if (self._sffProcessor._device.isEject()
                && self._cdromInsert.text === 'REMOTE_INSERT'
            ) {
                if (self._client.scope.cdDvdSelected.value === 'iso') {
                    self._sffProcessor._device._Init(self._cdromImageFile);
                    self._cdromImage.styleOptions.disable = true;
                } else {
                    self._sffProcessor._device._Init(self._udfBuilder.fileBlob);
                    self._cdromDirectory.styleOptions.disable = true;
                }
                setTimeout(() => {
                    self._cdromInsert.text = 'REMOTE_EJECT';
                    self._cdromConnect.disable = false;
                    self._cdromInsert.disable = false;
                }, 3000);
            } else {
                self._sffProcessor._device._Init(null);
                setTimeout(() => {
                    if (self._client.scope.cdDvdSelected.value === 'iso') {
                        self._cdromImage.styleOptions.disable = false;
                    } else {
                        self._cdromDirectory.styleOptions.disable = false;
                    }
                    self._cdromInsert.text = 'REMOTE_INSERT';
                    self._cdromConnect.disable = false;
                    self._cdromInsert.disable = false;
                }, 3000);
            }
        };
        this._floppyConnect.click = () => {
            if (self._establishedSet & self.FLOPPY_IMAGE_LINK) {
                self._disconnectSet = self._disconnectSet | self.FLOPPY_IMAGE_LINK;
                self._needReconnectFloppy = false;
                if (self._monitorTimer == null) {
                    self._monitorTimer = setInterval(() => {
                        self._VMMLinkMonitor();
                    }, 1000);
                }
            } else {
                self._floppyImage.styleOptions.disable = true;
                self._floppyConnect.disable = true;
                self._floppyInsert.disable = true;
                self._client.scope.floppy[0].disable = true;
                self._connectingSet = self._connectingSet | self.FLOPPY_IMAGE_LINK;
                if (self._monitorTimer == null) {
                    self._monitorTimer = setInterval(() => {
                        self._VMMLinkMonitor();
                    }, 1000);
                }
            }
        };
        this._floppyInsert.click = () => {
            self._floppyConnect.disable = true;
            self._floppyInsert.disable = true;
            if (
                self._ufiProcessor._device.isEject()
                && self._floppyInsert.text === 'REMOTE_INSERT'
            ) {
                self._ufiProcessor._device._Init(self._floppyImageFile);
                self._floppyImage.styleOptions.disable = true;
                setTimeout(() => {
                    self._floppyInsert.text = 'REMOTE_EJECT';
                    self._floppyConnect.disable = false;
                    self._floppyInsert.disable = false;
                }, 3000);
            } else {
                self._ufiProcessor._device._Init(null);
                setTimeout(() => {
                    self._floppyImage.styleOptions.disable = false;
                    self._floppyInsert.text = 'REMOTE_INSERT';
                    self._floppyConnect.disable = false;
                    self._floppyInsert.disable = false;
                }, 3000);
            }
        };
    }
    _SendStateRequest() {
        const data = packet.getVmmState(0, this._security);
        this._client._comunication._Send(data);
        this._timeout = 0;
        this._state++;
    }

    _SendPortRequest() {
        const data = packet.vmmPortPacket(0, this._security);
        this._client._comunication._Send(data);
        this._timeout = 0;
        this._state++;
    }

    _SendKeyRequest() {
        const data = packet.vmmCodekeyPacket(0, this._security);
        this._client._comunication._Send(data);
        this._timeout = 0;
        this._state++;
    }

    _CreateConnection() {
        const self = this;
        this._sock = new WebSocket('wss://' + this._ip + ':' + this._port + '/websocket');
        this._sock.binaryType = 'arraybuffer';
        this._sock.onopen = (e) => {
            self._WSOpen(e);
        };
        this._sock.onclose = (e) => {
            self._WSClose(e);
        };
        this._sock.onerror = (e) => {
            self._WSError(e);
        };
        this._sock.onmessage = (e) => {
            self._WSMessage(e);
        };
        this._timeout = 0;
        if (this._state === this.STATE_PORT_RESPONSE) {
            this._state++;
        }
    }

    _AuthConnection() {
        const dataLen = 24 + 1 + 4;
        const pack = new Uint8Array(12 + dataLen);
        let i = 0;
        let dataPos = 0;

        pack[0] = this.REGISTER;
        if (1) {
            pack[4] = (dataLen >> 24) & 0xff;
            pack[5] = (dataLen >> 16) & 0xff;
            pack[6] = (dataLen >> 8) & 0xff;
            pack[7] = dataLen & 0xff;

            for (dataPos = 12, i = 0; i < 24; i++ , dataPos++) {
                pack[dataPos] = this._security._vmmAuthCode[i];
            }
            pack[dataPos++] = 0;

            for (i = 0; i < 4; i++ , dataPos++) {
                pack[dataPos] = 0;
            }
            pack[8] = 3;
            pack[9] = 1;
            pack[10] = 1;
            pack[11] = 1;
        }

        this._Send(pack);
        this._timeout = 0;
        this._state++;
    }

    _SendHeartBeat() {
        if (this._timeout >= 5) {
            const data = new Uint8Array(12);
            data[0] = 6;
            this._Send(data);
        }
    }

    _OpenDevice() {
        if ((this._connectingSet & this.CDROM_IMAGE_LINK) === this.CDROM_IMAGE_LINK) {
            this._connectingSet = this._connectingSet & 0xfe;
            this._opendeviceSet = this._opendeviceSet | this.CDROM_IMAGE_LINK;
            const pack = new Uint8Array(12);
            pack[0] = this.CREATE;
            pack[1] = 2;
            this._Send(pack);
        } else if ((this._connectingSet & this.CDROM_DIRECTORY_LINK) === this.CDROM_DIRECTORY_LINK) {
            this._connectingSet = this._connectingSet & 0xfd;
            this._opendeviceSet = this._opendeviceSet | this.CDROM_DIRECTORY_LINK;
            const pack2 = new Uint8Array(12);
            pack2[0] = this.CREATE;
            pack2[1] = 2;
            this._Send(pack2);
        } else if ((this._connectingSet & this.FLOPPY_IMAGE_LINK) === this.FLOPPY_IMAGE_LINK) {
            this._connectingSet = this._connectingSet & 0xfb;
            this._opendeviceSet = this._opendeviceSet | this.FLOPPY_IMAGE_LINK;
            const pack3 = new Uint8Array(12);
            pack3[0] = this.CREATE;
            pack3[1] = 1;
            this._Send(pack3);
        }
    }

    _CloseDevice() {
        if (this._disconnectSet & this.CDROM_IMAGE_LINK && this._establishedSet & this.CDROM_IMAGE_LINK) {
            this._disconnectSet = this._disconnectSet & 0xfe;
            this._establishedSet = this._establishedSet & 0xfe;
            this._sffProcessor = null;
            if (this._establishedSet === 0) {
                this._Reset();
            } else {
                const pack = new Uint8Array(12);
                pack[0] = this.CLOSE_VM;
                pack[1] = 2;
                pack[2] = 0;
                this._Send(pack);
                this._ResetCDROM();
            }
        } else if (
            this._disconnectSet & this.CDROM_DIRECTORY_LINK &&
            this._establishedSet & this.CDROM_DIRECTORY_LINK
        ) {
            this._disconnectSet = this._disconnectSet & 0xfd;
            this._establishedSet = this._establishedSet & 0xfd;
            this._sffProcessor = null;
            if (this._establishedSet === 0) {
                this._Reset();
            } else {
                const pack4 = new Uint8Array(12);
                pack4[0] = this.CLOSE_VM;
                pack4[1] = 2;
                pack4[2] = 0;
                this._Send(pack4);
                this._ResetCDROM();
            }
        } else if (this._disconnectSet & this.FLOPPY_IMAGE_LINK && this._establishedSet & this.FLOPPY_IMAGE_LINK) {
            this._disconnectSet = this._disconnectSet & 0xfb;
            this._establishedSet = this._establishedSet & 0xfb;
            this._ufiProcessor = null;
            if (this._establishedSet === 0) {
                this._Reset();
            } else {
                const pack5 = new Uint8Array(12);
                pack5[0] = this.CLOSE_VM;
                pack5[1] = 1;
                pack5[2] = 0;
                this._Send(pack5);
                this._ResetFloppy();
            }
        }
    }

    _ResetCDROM() {
        this._client.scope.cdDvd[0].disable = false;
        this._client.scope.cdDvd[1].disable = false;
        this._cdromConnect.disable = false;
        this._cdromInsert.disable = true;
        this._cdromConnect.text = 'HOME_CONNECT';
        this._cdromInsert.text = 'REMOTE_INSERT';
        if (
            (typeof this._cdromImageFile === 'undefined' || this._cdromImageFile == null) &&
            (typeof this._cdromDirectoryFile === 'undefined' || this._cdromDirectoryFile == null)
        ) {
            this._cdromConnect.disable = true;
        }
        if (this._client.scope.cdDvdSelected.value === 'iso') {
            this._cdromImage.styleOptions.disable = false;
            this._cdromDirectory.styleOptions.disable = true;
        } else {
            this._cdromImage.styleOptions.disable = true;
            this._cdromDirectory.styleOptions.disable = false;
        }
    }

    _ResetFloppy() {
        this._floppyImage.styleOptions.disable = false;
        this._floppyConnect.disable = false;
        this._floppyInsert.disable = true;
        this._floppyConnect.text = 'HOME_CONNECT';
        this._floppyInsert.text = 'REMOTE_INSERT';
        this._client.scope.floppy.disable = false;
        if (typeof this._floppyImageFile === 'undefined' || this._floppyImageFile == null) {
            this._floppyConnect.disable = true;
        }
    }

    _Reset() {
        clearInterval(this._monitorTimer);
        this._monitorTimer = null;
        this._state = this.STATE_IDLE;

        if (this._sock != null) {
            this._sock.close();
            this._sock.onopen = null;
            this._sock.onclose = null;
            this._sock.onerror = null;
            this._sock.onmessage = null;
            this._sock = null;
        }

        this._timeout = 0;
        this._disconnectSet = 0;
        this._connectingSet = 0;
        this._establishedSet = 0;
        this._ufiProcessor = null;
        this._sffProcessor = null;

        this._ResetCDROM();
        this._ResetFloppy();
    }

    _VMMLinkMonitor() {
        if (this._timeout++ > 20) {
            this._Reset();
        }

        switch (this._state) {
            case this.STATE_IDLE:
                this._SendKeyRequest();
                break;
            case this.STATE_KEY_RESPONSE:
                this._SendStateRequest();
                break;
            case this.STATE_STATE_RESPONSE:
                this._SendPortRequest();
                break;
            case this.STATE_PORT_RESPONSE:
                this._CreateConnection();
                break;
            case this.STATE_CONNECTED:
                this._AuthConnection();
                break;
            case this.STATE_ESTABLISHED:
                this._SendHeartBeat();
                this._OpenDevice();
                this._CloseDevice();
                break;
            case this.STATE_ERROR:
                break;
            default:
        }
    }

    parserVmmState(data, noPri) {
        if (noPri === 1) {
            this._Reset();
            return;
        }
        let vmmState = 1;
        if (this._security.isKvmEncryption()) {
            const temp = this._security.kvmDecrypt(data);
            vmmState = temp[1] * 256 + temp[0];
        } else {
            vmmState = data[1] * 256 + data[0];
        }

        if (vmmState === 0) {
            this._client.showMessage('SOCKET_CANNOT_CONNECT');
            this._Reset();
        }
        if (vmmState === 1 && this._state === this.STATE_STATE_REQUEST) {
            this._state++;
            this._timeout = 0;
        }
    }

    parserVmmPort(data, noPri) {
        if (noPri === 1) {
            this._Reset();
            return;
        }
        if (this._security.isKvmEncryption()) {
            const temp = this._security.kvmDecrypt(data);
            this._port = temp[1] * 256 + temp[0];
        } else {
            this._port = data[1] * 256 + data[0];
        }
        if (this._state === this.STATE_PORT_REQUEST) {
            this._state++;
            this._timeout = 0;
        }
    }

    parserVmmCodekey(data, noPri) {
        if (noPri === 1) {
            this._Reset();
            return;
        }

        if (this._security.isKvmEncryption()) {
            const temp = this._security.kvmDecrypt(data);
            this._security.generateVmmKeyAndIv(temp);
        } else {
            this._security.generateVmmKeyAndIv(data);
        }
        if (this._state === this.STATE_KEY_REQUEST) {
            this._state++;
            this._timeout = 0;
        }
    }

    _ResolveSecurityWarning() {
        const self = this;
        let interval = null;
        let times = 0;

        function receiver(event) {
            if (event.data === 'Certificate OK.') {
                clearInterval(interval);
                window.removeEventListener('message', receiver);
                self._CreateConnection();
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
                self._Reset();
            }
        }, 200);
    }

    _WSOpen(event) {
        this._securityWarning = 0;
        if (this._state === this.STATE_CONNECTING) {
            this._state++;
            this._timeout = 0;
        }
    }

    _WSClose(event) { }

    _WSError(event) {
        if (this._securityWarning === 1 && this._state === this.STATE_CONNECTING) {
            this._securityWarning = 0;
            this._ResolveSecurityWarning();
            return;
        } else {
            this._Reset();
        }
    }

    _WSMessage(event) {
        const data = new Uint8Array(event.data);
        this._MsgDispatch(data, data.length);
    }

    _Send(data) {
        if (this._sock != null && this._sock.readyState === 1) {
            this._sock.send(data.buffer);
        }
    }

    _ACKProcess(cmd) {
        if (this._state === this.STATE_AUTHING) {
            if (cmd[2] === this.ACK_AUTH_OK) {
                this._state++;
                this._timeout = 0;
            } else if (cmd[2] === this.ACK_VM_BUSY) {
                this._Reset();
                this._client.showMessage('REMOTE_ERR_BUSY_VM');
            } else {
                return;
            }
        } else if (this._state === this.STATE_ESTABLISHED) {
            if (cmd[2] === 0) {
                this._timeout = 0;
            } else if (cmd[2] === this.ACK_DEVICE_CREAT) {
                this._timeout = 0;
                if ((this._opendeviceSet & this.CDROM_IMAGE_LINK) === this.CDROM_IMAGE_LINK) {
                    this._needReconnectCdrom = true;
                    this._opendeviceSet = this._opendeviceSet & 0xfe;
                    this._establishedSet = this._establishedSet | this.CDROM_IMAGE_LINK;
                    this._sffProcessor = new SCSIProcessor(this, this._cdromImageFile, this.PROTOCOL_SFF);
                    this._cdromConnect.disable = false;
                    this._cdromConnect.text = 'HOME_DISCONNECT';
                    this._cdromInsert.disable = false;
                    this._cdromInsert.text = 'REMOTE_EJECT';
                } else if ((this._opendeviceSet & this.CDROM_DIRECTORY_LINK) === this.CDROM_DIRECTORY_LINK) {
                    this._needReconnectCdrom = true;
                    this._opendeviceSet = this._opendeviceSet & 0xfd;
                    this._establishedSet = this._establishedSet | this.CDROM_DIRECTORY_LINK;
                    this._sffProcessor = new SCSIProcessor(this, this._udfBuilder.fileBlob, this.PROTOCOL_SFF);
                    this._cdromConnect.disable = false;
                    this._cdromConnect.text = 'HOME_DISCONNECT';
                    this._cdromInsert.disable = false;
                    this._cdromInsert.text = 'REMOTE_EJECT';
                } else if ((this._opendeviceSet & this.FLOPPY_IMAGE_LINK) === this.FLOPPY_IMAGE_LINK) {
                    this._needReconnectFloppy = true;
                    this._opendeviceSet = this._opendeviceSet & 0xfb;
                    this._establishedSet = this._establishedSet | this.FLOPPY_IMAGE_LINK;
                    this._ufiProcessor = new SCSIProcessor(this, this._floppyImageFile, this.PROTOCOL_UFI);
                    this._floppyConnect.disable = false;
                    this._floppyInsert.disable = false;
                    this._floppyConnect.text = 'HOME_DISCONNECT';
                    this._floppyInsert.text = 'REMOTE_EJECT';
                }
            } else if (cmd[2] === this.ACK_DEVICE_FAIL) {
                this._timeout = 0;
                if ((this._opendeviceSet & this.CDROM_IMAGE_LINK) === this.CDROM_IMAGE_LINK) {
                    this._opendeviceSet = this._opendeviceSet & 0xfe;
                    if (this._establishedSet === 0) {
                        this._Reset();
                    } else {
                        this._ResetCDROM();
                    }
                } else if ((this._opendeviceSet & this.CDROM_DIRECTORY_LINK) === this.CDROM_DIRECTORY_LINK) {
                    this._opendeviceSet = this._opendeviceSet & 0xfd;
                    if (this._establishedSet === 0) {
                        this._Reset();
                    } else {
                        this._ResetCDROM();
                    }
                } else if ((this._opendeviceSet & this.FLOPPY_IMAGE_LINK) === this.FLOPPY_IMAGE_LINK) {
                    this._opendeviceSet = this._opendeviceSet & 0xfb;
                    if (this._establishedSet === 0) {
                        this._Reset();
                    } else {
                        this._ResetFloppy();
                    }
                }
            } else {
                return;
            }
        }
    }

    _CloseVM(type) {
        switch (type) {
            case 0:
            case 1:
            case 2: break;
            default:
        }
    }

    _MsgDispatch(bytes, length) {
        let start = 0;
        let dlen = 0;
        while (start < length) {
            switch (bytes[start] & 0xff) {
                case this.ACK:
                    this._ACKProcess(bytes.slice(start, start + 12));
                    start += 12;
                    break;
                case this.CLOSE_VM:
                    this._CloseVM((bytes[start + 1] >> 4) & 0xf);
                    start += 12;
                    break;
                case this.CLOSE_DEV:
                    this._CloseVM(0);
                    start += 12;
                    break;
                case this.TRANS_UFI:
                    dlen =
                        (bytes[start + 4] << 24) +
                        (bytes[start + 5] << 16) +
                        (bytes[start + 6] << 8) +
                        bytes[start + 7];
                    if (this._ufiProcessor != null) {
                        this._ufiProcessor.process(bytes[3], bytes.slice(12, 12 + dlen));
                    }
                    start += 12 + dlen;
                    break;
                case this.TRANS_SFF:
                    dlen =
                        (bytes[start + 4] << 24) +
                        (bytes[start + 5] << 16) +
                        (bytes[start + 6] << 8) +
                        bytes[start + 7];
                    if (this._sffProcessor != null) {
                        this._sffProcessor.process(bytes[3], bytes.slice(12, 12 + dlen));
                    }
                    start += 12 + dlen;
                    break;
                default:
                    start++;
                    break;
            }
        }
    }
}
