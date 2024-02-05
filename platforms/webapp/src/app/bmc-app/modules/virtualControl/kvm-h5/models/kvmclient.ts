import { StatusBar } from './statusbar';
import { ToolBar } from './toolbar';
import { Display } from './display';
import { Mouse } from './mouse';
import { Keyboard } from './keyboard';
import { Security } from './security';
import { Comunication } from './comunication';

 class KvmClient {
        public _language = null;
        public _display = null;
        public _mouse = null;
        public _keyboard = null;
        public _statusbar = null;
        public _toolbar = null;
        public _comunication = null;
        public _security = null;
        public scope;
        public showMessage;
        constructor(scope, showMessage) {
            this.scope = scope;
            this.showMessage = showMessage;
            document.oncontextmenu = (e) => {
                return false;
            };
            document.addEventListener('keydown', (e) => {
                this._toolbar.setDefineKey(e);
                // tslint:disable-next-line: deprecation
                const keyCode = e.keyCode || e.which || e.charCode;
                if (keyCode !== 0x7b) {
                    if (e && e.preventDefault) {
                        e.preventDefault();
                    } else {
                        // tslint:disable-next-line: deprecation
                        window.event.returnValue = false;
                    }
                }
            });
            document.addEventListener('keyup', (e) => {
                // tslint:disable-next-line: deprecation
                const keyCode = e.keyCode || e.which || e.charCode;
                if (keyCode !== 0x7b) {
                    if (e && e.preventDefault) {
                        e.preventDefault();
                    } else {
                        // tslint:disable-next-line: deprecation
                        window.event.returnValue = false;
                    }
                }
            });
        }

        run(
            language,
            ip,
            kvmPort,
            vmmPort,
            kvmEncrypt,
            vmmEncrypt,
            verifyId,
            keyAndIv,
            privilege,
            verifyIdExt,
            SN
        ) {
            this._language = language;
            this._security = new Security(this, kvmEncrypt, vmmEncrypt, verifyId, keyAndIv, verifyIdExt);
            this._statusbar = new StatusBar(this, ip, SN);
            this._toolbar = new ToolBar(this, ip, vmmPort, privilege, language);
            this._display = new Display(this);
            this._mouse = new Mouse(this);
            this._keyboard = new Keyboard(this);
            this._comunication = new Comunication(this, ip, kvmPort);
        }
    }
    export function openKvm(scope, showMessage) {
        if (typeof Uint8Array.prototype.slice === 'undefined') {
            Uint8Array.prototype.slice = function (start, end) {
                const t = new Uint8Array(end - start);
                for (let i = start; i < end; i++) {
                    t[i - start] = this[i];
                }
                return t;
            };
        }

        const client = new KvmClient(scope, showMessage);
        const kvmParams = scope.kvmParams;
        client.run(
            kvmParams.local,
            kvmParams.IPA,
            kvmParams.port,
            kvmParams.vmmPort,
            0,
            0,
            kvmParams.verifyValue,
            kvmParams.decrykey,
            kvmParams.privilege,
            kvmParams.verifyValueExt,
            kvmParams.SN
        );
    }
