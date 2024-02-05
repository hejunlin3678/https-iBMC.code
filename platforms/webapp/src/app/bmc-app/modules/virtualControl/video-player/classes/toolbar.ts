/* player client
 * toolbar.js
 */
export class ToolBar {
    public _client;
    public cvx;
    public _data = null;
    public _indexcount = 20;
    // 总帧
    public _totalframe = 0;
    // 包头量化表类型
    public _dqtzType = 0;

    // IE不支持Map属性 采用下面的数组来实现之前的功能
    public _iframepos = new Array();
    public _indexpos = new Array();
    public _speeds = [0.25, 0.5, 2, 4];
    public _speedcmd = [0x01, 0x02, 0x03, 0x04];


    // 这部分仅仅为运行_DivStream函数而提供的全局变量
    public _start = 0;
    public _state = 0;
    public _resultstart = 0;
    public _head = 0;
    public _dlen = 0;
    public _rdlen = 0;
    public _sublen = 0;
    // 存储每一帧数据
    public _result = null;

    public _tasktimer = null;
    // 默认系数为1；
    public _speed = 1;
    // 是否播放完
    public _isfinished = false;
    // 是否加载文件
    public _isloadfile = false;
    // 是否正在播放；
    public _isplaying = true;
    // 是否全屏
    public _fullscreen = false;
    // 文件读取对象
    public _reader = null;
    // 是否循环播放
    public _isLoop = false;
    // 无效数据的数量
    public _invalidDataNum = 0;

    constructor(client) {
        this._client = client;
        this.cvx = client.cvx;
        this._EventBind();
        this._ADDFullscreenlistener();
        this.rightMenuInit();
    }
    // 为播放界面控件对象的事件绑定
    _EventBind () {
        document.getElementById('btnPlay').onclick = () => {
            this._PlayClick(false);
        };
        document.getElementById('btnSpeed').onclick = () => {
            this._SpeedClick();
        };
        document.getElementById('btnSlow').onclick = () => {
            this._SlowClick();
        };
        document.getElementById('btnLoop').onclick = () => {
            this._LoopClick();
        };
        document.getElementById('btnOpen').onclick = () => {
            this._OpenClick();
        };
        document.getElementById('btnCut').onclick = () => {
            this._CutClick();
        };
        document.getElementById('btnFullscreen').onclick = () => {
            this._FullScreenClick(false);
        };
        document.getElementById('playPic').onclick = () => {
            this._PlayClick(false);
        };
        // 打开文件绑定事件
        document.getElementById('file').onchange = () => {
            this._ReadFile();
        };
    }
    _SpeedClick () {
        const self = this;
        // 如果暂停状态，不支持设置速度
        const classList = document.getElementById('playStop').classList;
        const playButton = Object.values(classList).includes('playImg') || Object.values(classList).includes('playPressedImg');
        if (playButton) {
            return;
        }
        document.getElementById('btnSpeed').firstChild['classList'].remove('speedImg');
        document.getElementById('btnSpeed').firstChild['classList'].add('speedPressedImg');
        document.getElementById('btnSlow').firstChild['classList'].remove('slowPressedImg');
        document.getElementById('btnSlow').firstChild['classList'].add('slowImg');
        // 如果为在线播放
        if (this._client._isRemote) {
            if (this._speed >= 4) {
                this._speed = 4;
            } else {
                this._speed = this._speed * 2;
            }
            document.getElementById('speed').innerText = 'x' + this._speed;
            this._client._communication.sendSpeedFrame(this._speedcmd[this._speeds.indexOf(this._speed)]);
        } else {
            if (!this._isfinished && this._isloadfile) {
                if (this._speed >= 4) {
                    this._speed = 4;
                } else {
                    this._speed = this._speed * 2;
                }
                // 更新速度标签
                document.getElementById('speed').innerText = 'x' + this._speed;

                if (this._tasktimer != null) {
                    const data = self._data;
                    // 更新播放速度时，清除原有的定时器任务，开启新的定时器任务 就是更新周期
                    clearInterval(this._tasktimer);
                    this._tasktimer = setInterval(() => {
                        self._MyTask(data);
                    }, 80 / this._speed);
                    self.updatePlayDisplay(1);
                }
            }
        }
    }
    _SlowClick () {
        const self = this;

        // 如果暂停状态，不支持设置速度
        const classList = document.getElementById('playStop').classList;
        const playButton = Object.values(classList).includes('playImg') || Object.values(classList).includes('playPressedImg');
        if (playButton) {
            return;
        }
        document.getElementById('btnSlow').firstChild['classList'].remove('slowImg');
        document.getElementById('btnSlow').firstChild['classList'].add('slowPressedImg');
        document.getElementById('btnSpeed').firstChild['classList'].remove('speedPressedImg');
        document.getElementById('btnSpeed').firstChild['classList'].add('speedImg');
        // 如果为在线播放
        if (this._client._isRemote) {
            if (this._speed <= 0.25) {
                this._speed = 0.25;
            } else {
                this._speed = this._speed / 2;
            }
            document.getElementById('speed').innerText = 'x' + this._speed;
            this._client._communication.sendSpeedFrame(this._speedcmd[this._speeds.indexOf(this._speed)]);

        } else {
            if (!this._isfinished && this._isloadfile) {
                if (this._speed <= 0.25) {
                    this._speed = 0.25;
                } else {
                    this._speed = this._speed / 2;
                }
                // 更新速度标签
                document.getElementById('speed').innerText = 'x' + this._speed;

                if (this._tasktimer != null) {
                    const data = self._data;
                    clearInterval(this._tasktimer);
                    this._tasktimer = setInterval(() => {
                        self._MyTask(data);
                    }, 80 / this._speed);
                    self.updatePlayDisplay(1);
                }
            }
        }
    }
    _PlayClick (noNeedApply) {
        const self = this;
        if (this._client._isRemote) {
            // 防止初始阶段，还未设置包头量化表的类型，就点击播放，导致解析乱码
            if (this._client.scope.isInited === false) {
                return;
            }
            if (this._isplaying) {
                this._client._communication.sendSuspend();
                this._isplaying = false;
                self.updatePlayDisplay(0);
            } else {
                this._client._communication.sendContinuing();
                this._isplaying = true;
                self.updatePlayDisplay(1);
            }
            // 如果播放完之后点击播放按钮，从头开始播放
            if (this._client._display.getIsFinished()) {
                this._client._communication.sendJumpingFrame(1);
                this._isplaying = true;
                this._client._display.clearIsFinished();
                self.updatePlayDisplay(1);
            }
        } else {
            if (!this._isloadfile) {
                this._client._toolbar.updatePlayDisplay(0);
                document.getElementById('playStop').className = 'playImg';
                this._client._showMessage('VIDEO_REP_FILE_IMPORT');
                return;
            }
            if (!this._isfinished) {
                if (this._tasktimer === 0) {
                    const data = self._data;
                    this._tasktimer = setInterval(() => {
                        self._MyTask(data);
                    }, 80 / this._speed);
                    self.updatePlayDisplay(1);
                } else {
                    clearInterval(this._tasktimer);
                    /*
                     * 因为定时任务setInterval的返回值是定时器ID，不管是取消还是执行返回值都是数字，这里通过取消定时器任务后，将this._tasktimer
                     * 的值置为0，用来判断定时器是否运行
                     */
                    this._tasktimer = 0;
                    self.updatePlayDisplay(0);
                }
            } else {
                this._isfinished = false;
                const videoData = self._data;
                self._InitDivStream();
                if (this._tasktimer != null) {
                    clearInterval(this._tasktimer);
                    this._tasktimer = setInterval(() => {
                        self._MyTask(videoData);
                    }, 80 / this._speed);
                    self.updatePlayDisplay(1);
                }
            }
        }
    }
    _LoopClick () {
        const self = this;
        this._isLoop = !this._isLoop;
        const loop =  document.getElementById('btnLoop').firstChild;
        if (this._isLoop) {
            loop['classList'].remove('noLoopImg');
            loop['classList'].add('loopImg');
            self._client.scope.rightMenu.options[1].label = 'VIDEO_NO_LOOP';
        } else {
            loop['classList'].add('noLoopImg');
            loop['classList'].remove('loopImg');
            self._client.scope.rightMenu.options[1].label = 'VIDEO_LOOP';
        }
    }
    // 本地文件上传点击
    _OpenClick () {
        document.getElementById('file').click();
    }
    _CutClick () {
        const userAgent = navigator.userAgent;
        const linkScreenshot = document.createElement('a');
        const imgUrl = this.cvx.toDataURL('image/jpeg').replace('image/jpeg', 'image/octet-stream');
        linkScreenshot.href = imgUrl;
        linkScreenshot.download = 'screenshot.jpg';
        const event = document.createEvent('MouseEvents');
        event.initEvent('click', true, false);
        linkScreenshot.dispatchEvent(event);
    }
    supportFullscreen() {
        const docElm = document.getElementById('divFullscreen');
        const el = docElm as HTMLElement & {
            mozRequestFullScreen(): Promise<void>;
            webkitRequestFullscreen(): Promise<void>;
            msRequestFullscreen(): Promise<void>;
        };
        const isFullscreen =
            // W3C
            document['fullscreenElement'] ||
            // IE
            document['msFullscreenElement'] ||
            // 火狐
            document['mozFullScreenElement'] ||
            // 谷歌
            document['webkitFullscreenElement'] ||
            false;
        return [isFullscreen, el];
    }
    _FullScreenClick (noNeedApply) {
        if (
            document['fullscreenElement'] ||
            document['msFullscreenElement'] ||
            document['mozFullScreenElement'] ||
            document['webkitFullscreenElement']
        ) {
            this._fullscreen = true;
        } else {
            this._fullscreen = false;
        }
        if (!this._fullscreen) {
            this._client._display.enterFullScreen(noNeedApply);
        } else {
            this._client._display.exitFullScreen(noNeedApply);
        }
    }
    // 添加全屏监听事件
    _ADDFullscreenlistener () {
        const self = this;
        if (document.fullscreenEnabled) {
            document.addEventListener('fullscreenchange', (event) => {
                self._Callback();
            });
        }
        if (document['webkitFullscreenEnabled']) {
            document.addEventListener('webkitfullscreenchange', (event) => {
                self._Callback();
            });
        }
        if (document['mozFullScreenEnabled']) {
            document.addEventListener('mozfullscreenchange', (event) => {
                self._Callback();
            });
        }
        if (document['msFullscreenEnabled']) {
            document.addEventListener('MSFullscreenChange', (event) => {
                self._Callback();
            });
        }
    }
    _Callback () {
        if ( document['fullscreenElement'] ||
            document['msFullscreenElement'] ||
            document['mozFullScreenElement'] ||
            document['webkitFullscreenElement']
        ) {
            this._client._display.enterFullScreen(false);
        } else {
            this._client._display.exitFullScreen(false);
        }
    }
    // 从本地读取录像文件
    _ReadFile () {
        const self = this;
        const file = document.getElementById('file')['files'][0];

        if (file) {
            this._isfinished = false;
            // 每次调用该函数时都要清除掉正在执行的任务，并初始化相关参数
            if (this._tasktimer != null) {
                clearInterval(this._tasktimer);
                this._tasktimer = 0;
                this._InitDivStream();
            }
            const fileName = file.name;
            // 只获取后缀名为rep文件名
            if (fileName.substring(fileName.lastIndexOf('.') + 1) === 'rep') {
                if (typeof FileReader !== 'undefined') {
                    this._reader = new FileReader();
                    // 数据加载完后响应此事件
                    this._reader.onloadend = (e) => {
                        if (self._reader.onloadend != null) {
                            self._reader.onloadend = null;
                            self._isloadfile = true;
                            self._data = e.target.result;
                            self._data = new Uint8Array(self._data);
                            const data = self._data;
                            self._DataPrase(data);
                        }
                    };
                    this._reader.readAsArrayBuffer(file);
                    this._client.scope.fileName = file.name;
                    self.updatePlayDisplay(1);
                }
            } else {
                this._client._toolbar.updatePlayDisplay(0);
                document.getElementById('playStop').className = 'playImg';
                this._client._showMessage('VIDEO_FILE_IS_NOT_REP');
            }
        }
    }
    // 录像文件数据解析
    _DataPrase (data) {
        const self = this;
        // 解析包头文件
        this._ReadFileTopData(data);
        // 解析索引包
        this._ReadIndexData(data);
        // 此处开始只处理图像数据
        this._tasktimer = setInterval(() => {
            self._MyTask(data);
        }, 80 / this._speed);
    }
    _MyTask (data) {
        const self = this;
        // 如果检测到跳帧，则将图像指针移动到离其最近的I帧位置开始播放
        if (this._client._progressbar.isSkipFrame()) {
            this._start = this._indexpos[this._iframepos.indexOf(
                self.findIFrame(this._client._progressbar.getSkipFramePosition(), this._iframepos)
            )];
            this._state = 0;
            this._resultstart = 0;
            this._head = 0;
            this._dlen = 0;
            this._rdlen = 0;
            this._sublen = 0;
            this._result = null;
            // 处理完后清除掉跳帧标志
            this._client._progressbar.clearIsSkipFrame();
        }
        const isframe = self._DivStream(data);
        if (isframe) {
            switch (this._result[0]) {
                case 0x01:
                    // 包头文件
                    break;
                case 0x02:
                    // 索引包文件
                    break;
                case 0x03:
                    // 解决本地播放poweroff花屏问题
                    const dqtz = (this._result[1] & 0xf0) >> 4;
                    if (dqtz > 0) {
                        this._dqtzType = dqtz - 1;
                    }
                    this._client._display.Draw(this._result);
                    this._client._progressbar.updateSlider(this._totalframe);
                    break;
                default:
                    break;
            }
        } else {
            // 数据读取完，停止定时器任务
            if (!this._isLoop) {
                clearInterval(this._tasktimer);
            }
        }
    }
    // 包头文件数据解析
    _ReadFileTopData (data) {
        const self = this;
        // 文件头数据，一共52个字节
        const bytes = data.subarray(0, 52);
        if (bytes[6] !== 0x01) {
            return;
        }
        // 8-11字节存储所有帧的数量
        let byte = bytes.subarray(8, 8 + 4);
        this._totalframe = self._BytesToInt(byte);

        // 获取包头量化表的类型,与java比，前面多6字节
        byte = bytes.subarray(29, 29 + 1);
        this._dqtzType = self._BytesToInt(byte);
    }
    // 索引数据解析 读取所有的索引数据，并存放在map中，
    _ReadIndexData (data) {
        let seekpos = 52;
        while (seekpos > 0) {
            const indexpacketlength = 7 + this._indexcount * 12 + 8;
            let bytes = data.subarray(seekpos, seekpos + indexpacketlength);
            bytes = bytes.subarray(6, bytes.length);
            let nextindexpos = 0;
            let framepos;
            let indexpos;
            if (bytes[0] !== 0x02) {
                return;
            }
            // 获取一个索引包中的20个I帧信息
            for (let i = 0; i < this._indexcount; i++) {
                framepos = bytes.subarray(1 + i * (4 + 8), 1 + i * (4 + 8) + 4);
                indexpos = bytes.subarray(1 + i * (4 + 8) + 4, 1 + i * (4 + 8) + 4 + 8);
                framepos = this._BytesToInt(framepos);
                indexpos = this._BytesToInt(indexpos);
                if (Number(framepos) === 0 && Number(indexpos) === 0) {
                    return;
                }

                // 替换Map类
                if (this._iframepos.indexOf(framepos) === -1 && this._indexpos.indexOf(indexpos) === -1) {
                    this._iframepos.push(framepos);
                    this._indexpos.push(indexpos);
                }
            }
            nextindexpos = bytes.subarray(1 + this._indexcount * (4 + 8), 1 + this._indexcount * (4 + 8) + 8);
            nextindexpos = this._BytesToInt(nextindexpos);
            // 通过nextindexpos获取下一个索引包
            seekpos = nextindexpos;
        }
    }
    // 将文件中的数据分割成对应的包，去掉头部数据中的0xfe 0xf6共六个字节
    _DivStream (data) {
        while (this._start < data.length) {
            switch (this._state) {
                case 0:
                    this._head += data[this._start++] & 0xff;
                    if ((this._head & 0xffff) === 0xfef6) {
                        this._state = 1;
                    } else {
                        this._invalidDataNum++;
                    }
                    this._head <<= 8;
                    break;
                case 1:
                    if (this._start < data.length) {
                        this._dlen += (data[this._start++] & 0xff) << ((3 - this._sublen) * 8);
                        this._sublen++;
                    }

                    if (this._sublen > 3) {
                        this._sublen = 0;
                        this._resultstart = 0;
                        this._result = new Array(this._dlen - 6);
                        this._dlen = 0;
                        this._state = 2;
                    }
                    break;
                case 2:
                    if (this._result) {
                        this._result[this._resultstart++] = data[this._start++];
                        this._rdlen++;
                        if (this._rdlen >= this._result.length) {
                            this._state = 0;
                            this._head = 0;
                            this._rdlen = 0;
                            return true;
                        }
                    }
                    break;
                default:
                    this._state = 0;
                    this._head = 0;
                    break;
            }
        }
        this._start = 0;
        this._isfinished = true;
        this.updatePlayDisplay(0);
        // 没有一组数据是有效数据，则说明不是有效的录像文件
        if (this._invalidDataNum === data.length) {
            this._client._toolbar.updatePlayDisplay(0);
            document.getElementById('playStop').className = 'playImg';
            this._client._showMessage('VIDEO_REP_FILE_ERROR');
        }
        this._invalidDataNum = 0;
        if (this._isLoop) {
            this._PlayClick(false);
        }
        return false;
    }
    _InitDivStream () {
        this._start = 0;
        this._state = 0;
        this._resultstart = 0;
        this._head = 0;
        this._dlen = 0;
        this._rdlen = 0;
        this._sublen = 0;
        this._result = null;
    }

    _BytesToInt (bytes) {
        let result = 0;
        for (let i = 0; i < bytes.length; i++) {
            result += (parseInt(bytes[i], 10) & 0xff) << (8 * (bytes.length - i - 1));
        }
        return result;
    }

    getDQTZ () {
        return this._dqtzType;
    }

    findIFrame (iframepos, iframeposs) {
        if (iframeposs == null) {
            return 0;
        }

        for (let i = 0; i < iframeposs.length - 1; i++) {
            if (iframeposs[i] === iframepos) {
                return iframepos;
            } else {
                if (iframepos > iframeposs[i] && iframepos < iframeposs[i + 1]) {
                    return iframeposs[i];
                }
            }
        }

        return iframeposs[iframeposs.length - 1];
    }

    rightMenuInit () {
        const self = this;
        this._client.scope.rightMenu.change = (opt) => {
            switch (opt.id) {
                case 0:
                    self._FullScreenClick(true);
                    break;
                case 1:
                    self._LoopClick();
                    break;
                case 2:
                    self._PlayClick(true); // 不需要apply强制angular数据生效
                    break;
                case 3:
                    self._OpenClick();
                    break;
                case 4:
                    self._CutClick();
                    break;
            }
            document.getElementById('rightMenu').classList.remove('show');
            document.getElementById('rightMenu').classList.add('hide');
        };
    }

    updatePlayDisplay (isChangeToPlay) {
        const self = this;
        if (isChangeToPlay) {
            document.getElementById('playStop').className = 'stopImg';
            document.getElementById('playPic').classList.remove('show');
            document.getElementById('playPic').classList.add('hide');
            self._client.scope.playStatus = 'VIDEO_PLAY';
            self._client.scope.rightMenu.options[2].label = 'VIDEO_PAUSE';
        } else {
            document.getElementById('playStop').className = 'playPressedImg';
            document.getElementById('playPic').classList.remove('hide');
            document.getElementById('playPic').classList.add('show');
            self._client.scope.playStatus = 'VIDEO_PAUSE';
            self._client.scope.rightMenu.options[2].label = 'VIDEO_PLAY';
        }
    }
}

