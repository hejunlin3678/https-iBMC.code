/* player client
 * keyboard.js
 *
 */
const JS_KEY_SPACE = 0X20;
const JS_KEY_ENTER = 0x0D;

export class KeyBoard {
    public _layout = null;
    public _client;
    public ctx;
    public _canvas;
    constructor(client) {
        this._client = client;
        this._canvas = client.cvx;

        // 按键事件,绑定整个页面
        document.onkeypress = (e) => {
            this._KeyPress(e);
            return false;
        };
        // 全屏的点击事件
        document.getElementById('divFullscreen').onclick = (e) => {
            this._FullScreenClick();
            return false;
        };

        // 单击事件
        this._canvas.onclick = () => {
            this._Click();
            return false;
        };

        // 双击事件
        this._canvas.ondblclick = () => {
            this._DoubleClick();
            return false;
        };

        // 右键事件
        this._canvas.oncontextmenu = (e) => {
            e.preventDefault();
            this._ContextMenu(e);
            return false;
        };
    }
    // 处理空格或者回车键按下的时响应
    _KeyPress(e) {
        const keyCode = e.keyCode || e.which || e.charCode;
        if (keyCode === JS_KEY_ENTER || keyCode === JS_KEY_SPACE) {
            this._client._toolbar._PlayClick();
        }
    }

    _Click() {
        document.getElementById('rightMenu').classList.remove('show');
        document.getElementById('rightMenu').classList.add('hide');
        this._client._toolbar._PlayClick();
    }

    _FullScreenClick() {
        document.getElementById('rightMenu').classList.remove('show');
        document.getElementById('rightMenu').classList.add('hide');
    }

    _DoubleClick() {
        this._client._toolbar._FullScreenClick();
    }
    _ContextMenu(e) {
        const height = document.getElementById('rightMenu').offsetHeight;
        const optionsH = parseInt(String(height), 10);
        const x = e.clientX + 1;
        let y = e.clientY;
        if ((e.clientY + optionsH) > window.innerHeight) {
            y = window.innerHeight - optionsH - 40;
        }
        document.getElementById('rightMenu').setAttribute('style', `left: ${x}px; top: ${y}px`);
        document.getElementById('rightMenu').classList.remove('hide');
        document.getElementById('rightMenu').classList.add('show');
    }
}
