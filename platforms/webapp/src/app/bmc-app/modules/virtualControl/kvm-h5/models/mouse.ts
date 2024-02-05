import { packet } from './packet';
export class Mouse {
    public _client;
    public _remotemstscX = 0;
    public _remotemstscY = 0;
    public _button = 0;
    constructor(client) {
        this._client = client;
        this._EventBind();
    }
    _EventBind() {
        const canvas = document.getElementById('image_panel');
        canvas.onmousedown = (e) => {
            this._MouseDown(e);
        };
        canvas.onmouseup = (e) => {
            this._MouseUp(e);
        };
        canvas.onmouseenter = (e) => {
            this._MouseEnter(e);
        };
        canvas.onmouseleave = (e) => {
            this._MouseExit(e);
        };
        canvas.onmousemove = (e) => {
            this._MouseMove(e);
        };
    }

    _AreaDetction(e) {
        const xStart = (1024 - 800) / 2;
        const xEnd = (1024 - 800) / 2 + 800;
        const xStartFull = (window.screen.width - 800) / 2;
        const xEndFull = (window.screen.width - 800) / 2 + 800;

        if (
            (Number(this._client._display._fullscreen) === 0 &&
                e.offsetX > xStart &&
                e.offsetX < xEnd &&
                e.offsetY < 4 &&
                e.offsetY > 0) ||
            (Number(this._client._display._fullscreen) === 1 &&
                e.clientX > xStartFull &&
                e.clientX < xEndFull &&
                e.clientY < 4 &&
                e.clientY > 0)
        ) {
            const floatToolbar =  document.getElementById('toolbar');
            floatToolbar.style.display = 'block';
        }
    }

    _CalculatedCoordinate(e) {
        if (typeof e.offsetX !== 'undefined' && typeof e.offsetY !== 'undefined') {
            return;
        }

        const target = e.target || e.srcElement;
        const rect = target.getBoundingClientRect();
        const offsetX = e.clientX - rect.left;
        const offsetY = e.clientY - rect.top;
        e.offsetX = offsetX;
        e.offsetY = offsetY;
    }

    _SendRelativeMouse(x, y) {
        let i = 0;
        const arrayX = new Array(8);
        const arrayY = new Array(8);
        let tmpX = x;
        let tmpY = y;
        if (tmpX >= 0) {
            for (i = 0; i < 8; i++) {
                if (tmpX >= 240) {
                    arrayX[i] = 240;
                    tmpX = tmpX - 240;
                } else {
                    arrayX[i] = tmpX;
                    tmpX = 0;
                }
            }
        } else {
            for (i = 0; i < 8; i++) {
                if (tmpX <= -240) {
                    arrayX[i] = -240;
                    tmpX = tmpX + 240;
                } else {
                    arrayX[i] = tmpX;
                    tmpX = 0;
                }
            }
        }
        if (tmpY >= 0) {
            for (i = 0; i < 8; i++) {
                if (tmpY >= 240) {
                    arrayY[i] = 240;
                    tmpY = tmpY - 240;
                } else {
                    arrayY[i] = tmpY;
                    tmpY = 0;
                }
            }
        } else {
            for (i = 0; i < 8; i++) {
                if (tmpY <= -240) {
                    arrayY[i] = -240;
                    tmpY = tmpY + 240;
                } else {
                    arrayY[i] = tmpY;
                    tmpY = 0;
                }
            }
        }
        for (i = 0; i < 8; i++) {
            if (arrayX[i] === 0 && arrayY[i] === 0) {
                break;
            }
            const data = packet.mouseRelPacket(0, this._client._security, arrayX[i], arrayY[i], this._button);
            this._client._comunication._Send(data);
        }
    }

    _MouseDown(e) {
        this._CalculatedCoordinate(e);
        if (
            e.offsetX < this._client._display._vpX ||
            e.offsetX > this._client._display._vpW + this._client._display._vpX ||
            e.offsetY < this._client._display._vpY ||
            e.offsetY > this._client._display._vpH + this._client._display._vpY
        ) {
            return;
        }
        this._client._toolbar.hideAllSubMenu();
        if (Number(e.button) === 0) {
            this._button = this._button | 0x01;
        } else if (Number(e.button) === 1) {
            this._button = this._button | 0x04;
        } else if (Number(e.button) === 2) {
            this._button = this._button | 0x02;
        }
        if (this._client._toolbar._acceleration) {
            const x = ((e.offsetX - this._client._display._vpX) * 3000) / this._client._display._vpW;
            const y = ((e.offsetY - this._client._display._vpY) * 3000) / this._client._display._vpH;
            const data = packet.mouseAbsPacket(0, this._client._security, x, y, this._button);
            this._client._comunication._Send(data);
        } else {
            const data2 = packet.mouseRelPacket(0, this._client._security, 0, 0, this._button);
            this._client._comunication._Send(data2);
        }
    }

    _MouseUp(e) {
        this._CalculatedCoordinate(e);
        if (
            e.offsetX < this._client._display._vpX ||
            e.offsetX > this._client._display._vpW + this._client._display._vpX ||
            e.offsetY < this._client._display._vpY ||
            e.offsetY > this._client._display._vpH + this._client._display._vpY
        ) {
            return;
        }
        if (Number(e.button) === 0) {
            this._button = this._button & 0x06;
        } else if (Number(e.button) === 1) {
            this._button = this._button & 0x03;
        } else if (Number(e.button) === 2) {
            this._button = this._button & 0x05;
        }
        if (this._client._toolbar._acceleration) {
            const x = ((e.offsetX - this._client._display._vpX) * 3000) / this._client._display._vpW;
            const y = ((e.offsetY - this._client._display._vpY) * 3000) / this._client._display._vpH;
            const data = packet.mouseAbsPacket(0, this._client._security, x, y, this._button);
            this._client._comunication._Send(data);
        } else {
            const data2 = packet.mouseRelPacket(0, this._client._security, 0, 0, this._button);
            this._client._comunication._Send(data2);
        }
    }

    _MouseMove(e) {
        this._CalculatedCoordinate(e);
        this._AreaDetction(e);

        if (
            (this._remotemstscX === e.offsetX && this._remotemstscY === e.offsetY) ||
            e.offsetX < this._client._display._vpX ||
            e.offsetX > this._client._display._vpW + this._client._display._vpX ||
            e.offsetY < this._client._display._vpY ||
            e.offsetY > this._client._display._vpH + this._client._display._vpY
        ) {
            return;
        }

        if (this._client._toolbar._acceleration) {
            const x = ((e.offsetX - this._client._display._vpX) * 3000) / this._client._display._vpW;
            const y = ((e.offsetY - this._client._display._vpY) * 3000) / this._client._display._vpH;
            const data = packet.mouseAbsPacket(0, this._client._security, x, y, this._button);
            this._client._comunication._Send(data);
        } else {
            this._SendRelativeMouse(e.offsetX - this._remotemstscX, e.offsetY - this._remotemstscY);
        }
        this._remotemstscX = e.offsetX;
        this._remotemstscY = e.offsetY;
    }

    _MouseEnter(e) {
        this._CalculatedCoordinate(e);
        this._remotemstscX = e.offsetX;
        this._remotemstscY = e.offsetY;
    }

    _MouseExit(e) {}

    _MouseWheel(e) {}
}
