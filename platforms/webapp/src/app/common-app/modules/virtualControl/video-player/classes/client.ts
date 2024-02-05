/* player client
 * client.ts
 */
import {Display} from './display';
import {Communication} from './communication';
import {Security} from './security';
import {KeyBoard} from './keyboard';
import {ToolBar} from './toolbar';
import {ProgressBar} from './progressbar';
class Client  {
    public _display = null;
    public _communication = null;
    public _keyboard = null;
    public _toolbar = null;
    public _progressbar = null;
    public _security = null;
    public _isRemote = 0;
    public _showMessage;
    public scope: any;
    public ctx: any;
    public cvx: any;

    constructor(scope, showMessage) {
        this.scope = scope;
        this.cvx = scope.cvx;
        this.ctx = scope.ctx;
        this._showMessage = showMessage;
    }

    public run(ip, port, videoType, key, compress, salt) {
        if (typeof Uint8Array.prototype.slice === 'undefined') {
            Uint8Array.prototype.slice = (start, end) => {
                const t = new Uint8Array(end - start);
                for (let i = start; i < end; i++) {
                    t[i - start] = this[i];
                }
                return t;
            };
        }
        if (videoType > 0) {
            this._isRemote = 1;
        } else {
            this._isRemote = 0;
        }
        // 如果是在线播放，禁掉btnLoop、btnOpen按钮
        if (this._isRemote === 1) {
            document.getElementById('btnLoop').style.display = 'none';
            document.getElementById('btnOpen').style.display = 'none';
        }
        this._display = new Display(this);
        // 如果是在线播放才加载communication和security这个两个模块
        if (this._isRemote === 1) {
            this._communication = new Communication(this, ip, port, videoType);
            this._security = new Security(this, key, compress, salt);
        }
        this._keyboard = new KeyBoard(this);
        this._toolbar = new ToolBar(this);
        this._progressbar = new ProgressBar(this);
    }
}

export function run(scope, showMessage) {
    const client = new Client(scope, showMessage);

    client.run(
        scope.videoParams.ip,
        scope.videoParams.port,
        scope.videoParams.videoType,
        scope.videoParams.videoKey,
        scope.videoParams.press,
        scope.videoParams.salt
    );
}
