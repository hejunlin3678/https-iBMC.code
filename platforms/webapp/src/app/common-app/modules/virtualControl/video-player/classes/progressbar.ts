/* player client
 * progressbar.js
 */
export class ProgressBar {
    public _client;
    public _totalframe = 0;

    public _totalframetime = 0;
    public _isskipframe;
    public _skipframe;

    public _slider = null;
    constructor(client) {
        this._client = client;
        this._CreateSlider();
    }

    // 创建进度条
    _CreateSlider() {
        const self = this;
        this._slider = self._client.scope.slider;
        this._slider.change = (value) => {
            // 处理跳帧相关信息
            self._isskipframe = true;
            self._skipframe = value;

            const speed = Number( document.getElementById('speed').innerText.split('x')[1]);
            const time = parseInt(String(value * 80 / 1000 / speed), 10);
            document.getElementById('currentFrameTime').innerText = parseInt(String(time / 60), 10) + ' : ' + time % 60;
            if (value < self._totalframe) {
                if (self._client._isRemote) {
                    if (!self._client._toolbar._isplaying || self._client._display.getIsFinished()) {
                        self._client._display.clearIsFinished();
                        self._client._communication.sendContinuing();
                        self._client._toolbar.updatePlayDisplay(1);
                    }
                    self._client._toolbar._isplaying = true;
                } else {
                    self._client._toolbar._isfinished = false;
                    clearInterval(self._client._toolbar._tasktimer);
                    self._client._toolbar._tasktimer = 0;
                    self._client._toolbar._PlayClick();
                }
            }
        };
    }

    // 每播放一帧 更新一下slider
    updateSlider(totalframe) {
        const text = document.getElementById('speed').innerText;
        const speed = Number(text.split('x')[1]);
        this._totalframe = totalframe;
        this._totalframetime = parseInt(String(this._totalframe * 80 / 1000 / speed), 10);

        const currentframe =  this._client._display.getPlayingSeq();
        const currentframetime = parseInt(String(currentframe * 80 / 1000 / speed), 10);
        this._slider.value1 = currentframe;
        this._slider.max = this._totalframe;
        document.getElementsByClassName('ti3-slider-tip')[0].classList.remove('show');
        document.getElementsByClassName('ti3-slider-tip')[0].classList.add('hide');

       // 更新progressbar两个标签时间的值s
       document.getElementById('totalFrameTime').innerText =
        parseInt(String(this._totalframetime / 60), 10) + ' : ' + this._totalframetime % 60;
       document.getElementById('currentFrameTime').innerText =
        parseInt(String(currentframetime / 60), 10) + ' : ' + currentframetime % 60;
    }

    // 获取是否跳帧
    isSkipFrame() {
        return this._isskipframe;
    }

    clearIsSkipFrame() {
        this._isskipframe = false;
    }

    // 获取跳帧的目的位置
    getSkipFramePosition() {
        return this._skipframe;
    }
}
