/* player client
 * communication.js
 */
import { dataPacket } from './packet';
export class Communication {
    public _client;
    public _sock = null;
    public _ip;
    public _port;
    public _videoType;
    // 总的帧数
    public _totalframe = 0;
    // 量化值
    public _dqtzType = 0;
    // 索引包存放索引大小
    public _indexcount = 20;
    public _iframepos = [];
    public _indexpos = [];
    public _heartBeatTask = null;
    public _securityWarning = 1;
    public _isConnect = false;

    // 构建websocket对象
    constructor(client, ip, port, videoType) {
        this._client = client;
        this._ip = ip;
        this._port = port;
        this._videoType = videoType;
        this._CreatConnection();
    }
    // 创建webScoket对象
    _CreatConnection() {
        const self = this;
        this._sock = new WebSocket('wss://' + this._ip + ':' + this._port + '/');
        this._sock.onopen = (e) => { self._WSOpen(e); };
        this._sock.onclose = (e) => { self._WSClose(e); };
        this._sock.onerror = (e) => { self._WSError(e); };
        this._sock.onmessage = (e) => { self._WSMessage(e); };
    }
    // 以下为websocket相对应的回调事件处理
    // 连接之后发送 连接命令 发送心跳包
    _WSOpen(e) {
        const self = this;
        self.sendSetCipherSuiteMsg(0, 1); // 0表示使用AES-CBC算法,1表示使用pkcs7padding
        self._client._security.setPaddingStyle(1); // 1表示使用pkcs7padding
        self._SendConnect();
        this._heartBeatTask = setInterval(() => { self._SendHeartBeat(); }, 2000);
        this._isConnect = true;
    }

    _WSClose(e) {
        const self = this;
        self._SendDisConnect();
        this._sock = null;
        clearInterval(this._heartBeatTask);
        if (this._isConnect) {
            this._client._toolbar.updatePlayDisplay(0);
            document.getElementById('playStop').className = 'playImg';
            this._client._showMessage('VIDEO_FORCE_CLOSE_CONNECT');
        }
        this._isConnect = false;
    }

    _WSError(e) {
        if (this._securityWarning === 1) {
            this._securityWarning = 0;
            this._ResolveSecurityWarning();
            return;
        }
    }

    // 读取到服务端的数据 并进行解析
    _WSMessage(e) {
        let _data = null;
        const reader = new FileReader();
        const self = this;
        reader.onload = (event) => {
            if (event.target.readyState === FileReader.DONE) {
                _data = event.target.result;
                _data = new Uint8Array(_data);
                self._DataParse(_data);
            }
        };
        reader.readAsArrayBuffer(e.data);
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
        const popUpWin = window.open('https://' + this._ip + ':' + this._port, '_blank');
        window.addEventListener('message', receiver);

        interval = setInterval(() => {
            popUpWin.postMessage('hello', '*');
            if (times++ > 150) {
                clearInterval(interval);
                window.removeEventListener('message', receiver);
                this._client._toolbar.updatePlayDisplay(0);
                document.getElementById('playStop').className = 'playImg';
                self._client._showMessage('VIDEO_ERR_VERIFY_TIMEOUT');
            }
        }, 200);
    }
    // 通信数据解析
    _DataParse(data) {
        const self = this;
        let head = 0;
        let start = 0;
        let resultstart = 0;
        let state = 0;
        let dlen = 0;
        let sublen = 0;
        let rdlen = 0;
        let result;
        while (start < data.length) {
            switch (state) {
                case 0:
                    head += data[start++] & 0xff;
                    if ((head & 0xffff) === 0xfef6) {
                        state = 1;
                    }
                    head <<= 8;
                    break;
                case 1:
                    if (start < data.length) {
                        dlen += (data[start++] & 0xff) << ((3 - sublen) * 8);
                        sublen++;
                    }
                    if (sublen > 3) {
                        resultstart = 0;
                        result = new Array(dlen - 6);
                        state = 2;
                    }
                    break;
                case 2:
                    if (result) {
                        result[resultstart++] = data[start++];
                        rdlen++;
                        if (rdlen >= result.length) {
                            self._MsgDispatch(result);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    // 处理来自服务端不同类型的数据的任务调度
    _MsgDispatch(data) {
        const self = this;
        switch (data[0]) {
            // 解析文件头
            case 0x01:
                self._GetFileTopData(data);
                break;
            // 解析索引文件
            case 0x02:
                self._GetIndexData(data);
                document.getElementById('playStop').className = 'stopImg';
                document.getElementById('playPic').classList.remove('show');
                document.getElementById('playPic').classList.add('hide');
                this._client.scope.playStatus = 'VIDEO_PLAY';
                break;
            // 解析图像文件
            case 0x03:
                this._client._display.Draw(data);
                // 处理是否跳帧
                if (this._client._progressbar.isSkipFrame()) {
                    // 找到最近的I帧
                    const iframepos = this._client._toolbar.findIFrame(this._client._progressbar.getSkipFramePosition(), this._iframepos);
                    self.sendJumpingFrame(iframepos);
                    this._client._progressbar.clearIsSkipFrame();
                }
                this._client._progressbar.updateSlider(this._totalframe);
                break;
            // 收到连接成功命令，发送连接到录像的命令
            case 0x04:
                // 1代表只播放,现只支持此一种模式
                const playmode = 1;
                self._SendVideoConnect(playmode);
                break;
            default:
        }
    }

    // 解析文件头数据
    _GetFileTopData(data) {
        const self = this;
        if (data[0] !== 0x01) {
            return;
        }
        // 获取总的帧数
        let byte = data.slice(2, 2 + 4);
        this._totalframe = self._BytesToInt(byte);

        // 获取包头量化表的类型
        byte = data.slice(23, 23 + 1);
        this._dqtzType = self._BytesToInt(byte);

        // 防止初始阶段，还未设置包头量化表的类型，就点击播放，导致解析乱码
        this._client.scope.isInited = true;
    }
    // 解析索引数据
    _GetIndexData(data) {
        if (data[0] !== 0x02) {
            return;
        }
        let framepos;
        let indexpos;
        // 获取一个索引包中的20个I帧信息
        for (let i = 0; i < this._indexcount; i++) {
            framepos = data.slice(1 + i * (4 + 8), 1 + i * (4 + 8) + 4);
            indexpos = data.slice(1 + i * (4 + 8) + 4, 1 + i * (4 + 8) + 4 + 8);

            framepos = this._BytesToInt(framepos);
            indexpos = this._BytesToInt(indexpos);

            if (framepos === 0 && indexpos === 0) {
                return;
            }
            // 替换Map类
            if (this._iframepos.indexOf(framepos) === -1 && this._indexpos.indexOf(indexpos) === -1) {
                this._iframepos.push(framepos);
                this._indexpos.push(indexpos);
            }

        }
    }
    _SendConnect() {
        const self = this;
        const data = dataPacket.connect(this._client._security.getCodeKey());
        self._Send(data);
    }

    // 心跳包的发送
    _SendHeartBeat() {
        const self = this;
        const data = dataPacket.heartBeat();
        self._Send(data);
    }

    // 发送连接到录像的命令
    _SendVideoConnect(playmode) {
        const self = this;
        const data = dataPacket.sendVideo(this._videoType, playmode);
        self._Send(data);
    }

    // 发送断开连接指令
    _SendDisConnect() {
        const self = this;
        const data = dataPacket.disConnect();
        self._Send(data);
    }

    // 发送播放速度指令
    sendSpeedFrame(speed) {
        const self = this;
        const data = dataPacket.speedFrame(speed);
        self._Send(data);
    }
    // 发送暂停录像发送指令
    sendSuspend() {
        const self = this;
        const data = dataPacket.suspend();
        self._Send(data);
    }

    // 发送继续播放指令
    sendContinuing() {
        const self = this;
        const data = dataPacket.continuing();
        self._Send(data);
    }

    // 发送跳帧指令
    sendJumpingFrame(skippoint) {
        const self = this;
        const data = dataPacket.jumpingFrame(skippoint);
        self._Send(data);
    }

    // 发送停止录像发送命令 (暂未调用)
    sendStop() {
        const self = this;
        const data = dataPacket.stop();
        self._Send(data);
    }

    // 发送重连指令 (暂未调用)
    sendReConnect(seq) {
        const self = this;
        const data = dataPacket.reConnect(seq);
        self._Send(data);
    }

    // 发送重连前发送的状态 (暂未调用)
    sendBeforReConnect(playmode, state, speed) {
        const self = this;
        const data = dataPacket.beforReConnect(this._videoType, playmode, state, speed);
        self._Send(data);
    }

    // 发送设置加密算法套件消息
    sendSetCipherSuiteMsg(algoType, paddingStyle) {
        const self = this;
        const data = dataPacket.setCipherAlgoSuite(algoType, paddingStyle);
        self._Send(data);
    }

    // 获取总的帧数
    getFrameTotal() {
        const totalframe = this._totalframe;
        return totalframe;
    }
    getDQTZ() {
        return this._dqtzType;
    }

    _Send(cmd) {
        if (this._sock != null && this._sock.readyState === 1) {
            this._sock.send(cmd);
        }
    }

    _BytesToInt(bytes) {
        let result = 0;
        for (let i = 0; i < bytes.length; i++) {
            result += (parseInt(bytes[i], 10) & 0xff) << (8 * (bytes.length - i - 1));
        }
        return result;
    }
}
