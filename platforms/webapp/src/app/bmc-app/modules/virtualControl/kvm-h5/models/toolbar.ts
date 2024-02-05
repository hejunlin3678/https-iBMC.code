import { packet } from './packet';
import { VirtualMedia } from './virtualmedia';
import { Image2rep } from './image2rep';
import {
    uskeyboardTable, JS_KEY_A, JS_KEY_Z, JS_KEY_0, JS_KEY_9, JS_KEY_SEMI, JS_KEY_QUOTO, JS_KEY_BR1,
    JS_KEY_QUOT, JS_KEY_FF_SEMI, JS_KEY_FF_PLUS, JS_KEY_FF_MIN, JS_KEY_0P, JS_KEY_DIVP, JS_KEY_OR_1
} from 'src/app/common-app/utils/common';

export class ToolBar {
    public _fixed = 1;
    public _fullscreen = 0;
    public _bootoption = 0;
    public _acceleration = 0;
    public _single = 0;
    public _record = 0;
    public _definition = 0;
    public _keyboardState = 0;
    public _floatTimer = null;
    public _virtualMedia;
    public _slider = null;
    public _defineKeyWindow = null;
    public _client;
    public _recorder = null;
    public _lang;
    public _keyboardlayout = 0;
    // 点击的键鼠模式
    public _mouseId = 0;
    // 记录是否响应成功,0表示未响应，1为超时，2为响应
    public _responseFlag = 0;
    public _umsInterval = null;

    constructor(client, ip, port, privilege, lang) {
        this._virtualMedia = new VirtualMedia(client, this, client._security, ip, port);
        this._client = client;
        this._lang = lang;
        this._WidgetInit();
        this._EventBind();
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

    _WidgetInit() {
        const self = this;

        this._slider = self._client.scope.slider;
        this._slider.changeStop = (value) => {
            let val = value;
            if (value < 10) {
                val = 10;
            } else if (value >= 60 && value <= 70) {
                val = 70;
            }
            const data = packet.definitionPacket(
                0,
                self._client._security,
                val
            );
            if (self._client._comunication) {
                self._client._comunication._Send(data);
            }
        };
        if (this.supportFullscreen()[1]) {
            self._client.scope.fullscreenTitle = 'REMOTE_FULL_TIP';
        } else {
            self._client.scope.fullscreenTitle = 'REMOTE_FULL_TIP_NOT_SUPPORT';
            const btnFullScreenDiv = document.getElementById('btn_fullscreen').getElementsByTagName('div');
            for (let i = 0; i < btnFullScreenDiv.length; i++) {
                const btnFullScreenDivItem = btnFullScreenDiv.item(i) as HTMLElement;
                btnFullScreenDivItem.classList.remove('fullscreen-img');
                btnFullScreenDivItem.classList.add('not-support-fullscreen');
            }
        }
    }

    _EventBind() {
        const self = this;
        const floatToolbar = document.getElementsByClassName('float-toolbar');
        const _cdrom = document.getElementById('btn_cdrom');
        const _floppy = document.getElementById('btn_floppy');
        _cdrom.onclick = () => {
            const element = document.getElementById('isoCdromId').getElementsByClassName('ti3-aui-upload-btn')[0] as HTMLElement;
            element.innerText = '';
            const element2 = document.getElementById('locationFileId').getElementsByClassName('ti3-aui-upload-btn')[0] as HTMLElement;
            element2.innerText = '';
        };
        _floppy.onclick = () => {
            const element3 = document.getElementById('isoFloppyId').getElementsByClassName('ti3-aui-upload-btn')[0] as HTMLElement;
            element3.innerText = '';
        };
        for (let i = 0; i < floatToolbar.length; i++) {
            const element: HTMLElement = floatToolbar.item(i) as HTMLElement;
            element.onmouseout = () => {
                if (self._fixed === 0) {
                    self._floatTimer = setTimeout(() => {
                        element.style.display = 'none';
                    }, 1000);
                }
            };
            element.onmouseover = () => {
                if (self._fixed === 0) {
                    clearTimeout(self._floatTimer);
                }
            };
        }
        const mainMenu = document.getElementsByClassName('main-menu-item');
        for (let i = 0; i < mainMenu.length; i++) {
            const element = mainMenu.item(i) as HTMLElement;
            element.onclick = () => {
                self._MainMenuEvent(element);
                // 点击软驱按钮，查询软驱使能
                if (element.parentElement.id === 'btn_floppy') {
                    self._GetFloppyState();
                }
            };
        }

        document.getElementById('btn_float').onclick = () => {
            self._FixedEvent();
        };

        document.getElementById('btn_fullscreen').onclick = () => {
            self._client._display.toggleFullScreen();
        };

        document.getElementById('btn_record').onclick = () => {
            self._RecordEvent();
        };

        const powerSubmenu = document.getElementById('power_submenu').getElementsByTagName('li');
        for (let i = 0; i < powerSubmenu.length; i++) {
            powerSubmenu.item(i).onclick = () => {
                self._ConfirmPowerWindow(i);
            };
        }

        const bootSubmenus = Array.from(document.getElementById('boot_submenu').getElementsByTagName('li'));
        for (let i = 0; i < bootSubmenus.length; i++) {
            bootSubmenus[i].onclick = () => {
                self._BootEvent(i);
            };
        }
        const keyboardSubmenu = document.getElementById('keyboard_submenu').getElementsByTagName('li');
        for (let i = 0; i < keyboardSubmenu.length; i++) {
            keyboardSubmenu.item(i).onclick = () => {
                self._KeyboardEvent(i);
            };
        }
        const mouseSubmenu = document.getElementById('mouse_submenu').getElementsByTagName('li');
        for (let i = 0; i < mouseSubmenu.length; i++) {
            mouseSubmenu.item(i).onclick = () => {
                self._MouseEvent(i);
            };
        }
        const keyboardlayoutSubmenu = document.getElementById('keyboardlayout_submenu').getElementsByTagName('li');
        for (let i = 0; i < keyboardlayoutSubmenu.length; i++) {
            keyboardlayoutSubmenu.item(i).onclick = () => {
                self._KeyboardLayoutEvent(i);
            };
        }
        const keyboardlayoutSubmenuLi = document.getElementById('keyboardlayout_submenu').getElementsByTagName('li');
        const li = keyboardlayoutSubmenuLi[self._keyboardlayout].firstChild as HTMLElement;
        li.style.display = 'block';
        document.getElementById('btn_help').onclick = () => {
            if (this._client.scope.currnetLang === 'fr-fr') {
                this._client.scope.currnetLang = 'fre-fr';
            } else if (this._client.scope.currnetLang === 'ja-ja') {
                this._client.scope.currnetLang = 'jap-ja';
            }
            const url = window.location.href.split('#')[0] + 'help/' + this._client.scope.currnetLang + '/bmc_help_0046.html';
            window.open(url);
        };
    }

    hideAllSubMenu() {
        const subMenu = document.getElementsByClassName('sub-menu');
        for (let i = 0; i < subMenu.length; i++) {
            const submenuEl = subMenu.item(i) as HTMLElement;
            submenuEl.style.display = 'none';
        }
    }

    _MainMenuEvent(o) {
        const subMenus = o.parentElement.querySelectorAll('.sub-menu');
        let visible;
        for (const subMenu of subMenus) {
            if (subMenu.style.display === 'block') {
                visible = subMenu;
            }
        }
        this.hideAllSubMenu();
        if (!visible) {
            for (const subMenu of subMenus) {
                subMenu.style.display = 'block';
            }
        }
    }

    _GetFloppyState() {
        const data = packet.getFloppyState(0, this._client._security);
        this._client._comunication._Send(data);
    }

    _FixedEvent() {
        this._fixed = 1 - this._fixed;
        const btnFloatDiv = document.getElementById('btn_float').getElementsByTagName('div');
        const removeClass = this._fixed ? 'fixed-img' : 'float-img';
        const addClass = this._fixed ? 'float-img' : 'fixed-img';
        for (let i = 0; i < btnFloatDiv.length; i++) {
            const btnFullScreenDivItem = btnFloatDiv.item(i) as HTMLElement;
            btnFullScreenDivItem.classList.remove(removeClass);
            btnFullScreenDivItem.classList.add(addClass);
        }
    }

    fullscreenAction(flag) {
        const self = this;
        const body = document.getElementsByTagName('body')[0];
        const statusPanel = document.getElementById('status_panel');
        const btnFullScreenDivs = Array.from(document.getElementById('btn_fullscreen').getElementsByTagName('div'));
        const btnFloats = Array.from(document.getElementById('btn_float').getElementsByTagName('div'));
        const floatToolbar = document.getElementById('toolbar');
        if (flag === 1) {
            this._fixed = 0;
            this._fullscreen = 1;
            body.classList.remove('content');
            statusPanel.classList.remove('status-normal');
            statusPanel.classList.add('status-full-screen');
            for (const btnFull of btnFullScreenDivs) {
                btnFull.classList.remove('fullscreen-img');
                btnFull.classList.add('normalscreen-img');
            }
            for (const btnFloat of btnFloats) {
                btnFloat.classList.remove('float-img');
                btnFloat.classList.add('fixed-img');
            }
            document.getElementById('image_panel').style.marginTop = '0px';
            floatToolbar.style.top = '0px';
            floatToolbar.style.left = (window.screen.width - 700) / 2 + 'px';
            this._client.scope.fullscreenTitle = 'REMOTE_EXIT_FULL_TIP';
        } else {
            this._fullscreen = 0;
            body.classList.add('content');
            statusPanel.classList.remove('status-full-screen');
            statusPanel.classList.add('status-normal');
            for (const btnFull of btnFullScreenDivs) {
                btnFull.classList.remove('normalscreen-img');
                btnFull.classList.add('fullscreen-img');
            }
            document.getElementById('image_panel').style.marginTop = '50px';
            floatToolbar.style.top = '50px';
            floatToolbar.style.left = '162px';
            this._client.scope.fullscreenTitle = 'REMOTE_FULL_TIP';
        }
    }

    _PowerEvent(index) {
        let cmd = 0;
        switch (index) {
            case 0:
                cmd = packet.CMD_POWER_ON;
                break;
            case 1:
                cmd = packet.CMD_POWER_FORCE_OFF;
                break;
            case 2:
                cmd = packet.CMD_POWER_OFF;
                break;
            case 3:
                cmd = packet.CMD_POWER_FORCE_REBOOT;
                break;
            case 4:
                cmd = packet.CMD_POWER_FORCE_OFF_AND_ON;
                break;
            default:
                return;
        }
        const data = packet.powerControlPacket(0, this._client._security, cmd);
        this._client._comunication._Send(data);
        this.hideAllSubMenu();
    }

    _BootEvent(index) {
        let cmd = 0;
        switch (index) {
            case 0:
                cmd = packet.CMD_BOOT_NORMAL;
                break;
            case 1:
                cmd = packet.CMD_BOOT_HARD_DISK;
                break;
            case 2:
                cmd = packet.CMD_BOOT_CDROM;
                break;
            case 3:
                cmd = packet.CMD_BOOT_FLOPPY;
                break;
            case 4:
                cmd = packet.CMD_BOOT_PXE;
                break;
            case 5:
                cmd = packet.CMD_BOOT_BIOS;
                break;
            default:
                return;
        }
        const data = packet.bootOptionPacket(0, this._client._security, cmd);
        this._client._comunication._Send(data);
        this.hideAllSubMenu();
    }

    _KeyboardLayoutEvent(index) {
        switch (index) {
            case 0:
                this._keyboardlayout = 0;
                break;
            case 1:
                this._keyboardlayout = 1;
                break;
            case 2:
                this._keyboardlayout = 2;
                break;
            case 3:
                this._keyboardlayout = 3;
                break;
            case 4:
                this._keyboardlayout = 4;
                break;
            default:
                return;
        }
        this.hideAllSubMenu();
        const keyboardlayoutSubmenu = Array.from(document.getElementById('keyboardlayout_submenu').getElementsByTagName('ul'));

        for (const keyboardlayout of keyboardlayoutSubmenu) {
            const lis = Array.from(keyboardlayout.childNodes);
            for (const liDom of lis) {
                const liFirst = liDom.firstChild as HTMLElement;
                liFirst.style.display = 'none';
            }
        }

        const keyboardlayoutSubmenuLi = document.getElementById('keyboardlayout_submenu').getElementsByTagName('li');
        const li = keyboardlayoutSubmenuLi[index].firstChild as HTMLElement;
        li.style.display = 'block';
    }

    _KeyboardEvent(index) {
        switch (index) {
            case 0:
                this._client._keyboard.sendAltTab();
                break;
            case 1:
                this._client._keyboard.sendCtrlEsc();
                break;
            case 2:
                this._client._keyboard.sendCtrlShift();
                break;
            case 3:
                this._client._keyboard.sendCtrlSpace();
                break;
            case 4:
                this._client._keyboard.sendCtrlAltDel();
                break;
            case 5:
                this._ShowDefineKeyWindow();
                break;
            default:
                return;
        }
        this.hideAllSubMenu();
    }

    _MouseEvent(index) {
        const self = this;
        self.hideAllSubMenu();

        // 防止多次点击时flag出错
        if (self._umsInterval) {
            clearInterval(self._umsInterval);
        }

        self._mouseId = index;
        let data = null;
        // 默认为未响应状态
        self._responseFlag = 0;
        data = packet.getUmsNotifyState(0, self._client._security);
        self._client._comunication._Send(data);

        // 1s内无响应自动下发键鼠模式设置
        self._umsInterval = setInterval(() => {
            // 未响应
            if (self._responseFlag !== 2) {
                // 响应状态为超时
                self._responseFlag = 1;
                self._SetMouseMode();
            }
            clearInterval(self._umsInterval);
        }, 1000);
    }

    _ShowDialog(data) {
        const self = this;
        if (self._responseFlag === 1) {
            // 请求超时响应，不做任何处理
            return;
        }
        self._responseFlag = 2;
        // 显示键鼠模式修改提示
        let flag = 0;
        if (self._client._security.isKvmEncryption()) {
            const temp = self._client._security.kvmDecrypt(data);
            flag = temp[1] * 256 + temp[0];
        } else {
            flag = data[1] * 256 + data[0];
        }
        if (flag === 1) {
            const tiMessage = self._client.scope.tiMessage;
            const instance = tiMessage.open({
                type: 'prompt',
                id: 'mouseMode',
                closeable: false,
                okButton: {
                    show: true,
                    text: self._client.scope.translate.instant('COMMON_OK'),
                    disabled: false,
                    autofocus: false,
                    click() {
                        self._SetMouseMode();
                        instance.close();
                    }
                },
                cancelButton: {
                    show: true,
                    text: self._client.scope.translate.instant('COMMON_CANCEL'),
                    disabled: false,
                    autofocus: false,
                    click() {
                        self.hideAllSubMenu();
                        instance.dismiss();
                    }
                },
                title: self._client.scope.translate.instant('COMMON_CONFIRM'),
                contentTitle: '',
                content: self._client.scope.translate.instant('MOUSE_MODE_SET_TIP')
            });
        } else if (flag === 0) {
            self._SetMouseMode();
        }
    }

    _SetMouseMode() {
        let data = null;
        const index = this._mouseId;

        if (index === 0) {
            data = packet.mouseModePacket(0, this._client._security, 1 - this._acceleration);
            this._client._comunication._Send(data);
        } else if (index === 1) {
            this._single = 1 - this._single;
            this._client._display.switchCursor(this._single);
            const mouseSubmenuLi = document.getElementById('mouse_submenu').getElementsByTagName('li');
            const spanNode = mouseSubmenuLi[1].children[0] as HTMLElement;
            if (this._single) {
                data = packet.mouseModePacket(0, this._client._security, 0);
                this._client._comunication._Send(data);
                spanNode.style.display = 'block';
            } else {
                spanNode.style.display = 'none';
            }
        } else if (index === 2) {
            data = packet.powerControlPacket(0, this._client._security, packet.CMD_USB_REST);
            this._client._comunication._Send(data);
        }
    }
    _RecordEvent() {
        const btnRecordDiv = document.getElementById('btn_record').children[0];
        if (this._record === 0 && this._recorder == null) {
            this._recorder = new Image2rep();
            this._recorder.startRecord();
            this._record = 1;
            btnRecordDiv.classList.remove('record-img');
            btnRecordDiv.classList.add('recording-img');
            const data = packet.videoStart(0, this._client._security);
            this._client._comunication._Send(data);
        } else if (this._record === 1 && this._recorder != null) {
            this._recorder.stopRecord();
            this._recorder = null;
            this._record = 0;
            btnRecordDiv.classList.remove('recording-img');
            btnRecordDiv.classList.add('record-img');
            const data = packet.videoStop(0, this._client._security);
            this._client._comunication._Send(data);
        }
    }

    setBootOption(option) {
        let index = 0;
        switch (option) {
            case packet.CMD_BOOT_NORMAL:
                index = 0;
                break;
            case packet.CMD_BOOT_PXE:
                index = 4;
                break;
            case packet.CMD_BOOT_HARD_DISK:
                index = 1;
                break;
            case packet.CMD_BOOT_CDROM:
                index = 2;
                break;
            case packet.CMD_BOOT_BIOS:
                index = 5;
                break;
            case packet.CMD_BOOT_FLOPPY:
                index = 3;
                break;
            default:
                return;
        }

        const bootSubmenuLi = Array.from(document.getElementById('boot_submenu').getElementsByTagName('li'));
        for (const li of bootSubmenuLi) {
            const spanNode = li.children[0] as HTMLElement;
            spanNode.style.display = 'none';
        }
        (bootSubmenuLi[index].children[0] as HTMLElement).style.display = 'block';
    }

    setMouseAcceleration(status) {
        this._acceleration = status;
        const mouseSubmenuLi = Array.from(document.getElementById('mouse_submenu').getElementsByTagName('li'));
        if (status) {
            (mouseSubmenuLi[0].firstChild as HTMLElement).style.display = 'block';
            (mouseSubmenuLi[1].firstChild as HTMLElement).style.display = 'none';
            this._client._display.switchCursor(0);
            this._single = 0;
        } else {
            (mouseSubmenuLi[0].firstChild as HTMLElement).style.display = 'none';
        }
    }

    setFloppyState(data) {
        let floppyState = 1;
        if (data.length > 1) {
            if (this._client._security.isKvmEncryption()) {
                const temp = this._client._security.kvmDecrypt(data);
                floppyState = temp[0];
            } else {
                floppyState = data[0];
            }
        } else {
            floppyState = data[0];
        }

        const floppyTable = document.getElementById('floppy_submenu').children[1] as HTMLElement;
        const tipDiv = document.getElementById('floppy_submenu').children[2] as HTMLElement;
        if (floppyState === 0) {
            floppyTable.style.display = 'none';
            tipDiv.style.display = 'block';
        } else {
            floppyTable.style.display = 'block';
            tipDiv.style.display = 'none';
        }
    }

    setKeyboardLight(status) {
        this._keyboardState = status;
    }

    setDefinition(status) {
        const current = this._slider.value;
        if (
            status < 10 ||
            status > 100 ||
            (current >= status && current < status + 10) ||
            (status === 10 && current <= 10)
        ) {
            return;
        }
        this._slider.value = status;
    }

    _ShowDefineKeyWindow() {
        const self = this;
        const tiMessage = self._client.scope.tiMessage;
        const domSanitizer = self._client.scope.domSanitizer;
        const inputTemplate = `<div style="padding-top:14px;font-size:0;">
            <input id="txt_define1" type="text" style="height:28px;width:80px;font-size: 14px;" />
            <input id="txt_define2" type="text" style="height:28px;width:80px;font-size: 14px;" />
            <input id="txt_define3" type="text" style="height:28px;width:80px;font-size: 14px;" />
            <input id="txt_define4" type="text" style="height:28px;width:80px;font-size: 14px;" />
            <input id="txt_define5" type="text" style="height:28px;width:80px;font-size: 14px;" />
            <input id="txt_define6" type="text" style="height:28px;width:80px;font-size: 14px;" />
            </div>`;
        const instance = tiMessage.open({
            closeable: false,
            modalClass: 'keybord',
            id: 'keybord',
            okButton: {
                show: true,
                text: self._client.scope.translate.instant('COMMON_OK'),
                disabled: false,
                autofocus: false,
                click() {
                    const txtDefine1 = document.getElementById('txt_define1');
                    const txtDefine2 = document.getElementById('txt_define2');
                    const txtDefine3 = document.getElementById('txt_define3');
                    const txtDefine4 = document.getElementById('txt_define4');
                    const txtDefine5 = document.getElementById('txt_define5');
                    const txtDefine6 = document.getElementById('txt_define6');
                    self._client._keyboard.sendSelfDef(
                        txtDefine1.getAttribute('keyCode'),
                        txtDefine1.getAttribute('key'),
                        txtDefine2.getAttribute('keyCode'),
                        txtDefine2.getAttribute('key'),
                        txtDefine3.getAttribute('keyCode'),
                        txtDefine3.getAttribute('key'),
                        txtDefine4.getAttribute('keyCode'),
                        txtDefine4.getAttribute('key'),
                        txtDefine5.getAttribute('keyCode'),
                        txtDefine5.getAttribute('key'),
                        txtDefine6.getAttribute('keyCode'),
                        txtDefine6.getAttribute('key')
                    );
                    instance.close();
                }
            },
            cancelButton: {
                show: true,
                text: self._client.scope.translate.instant('COMMON_CANCEL'),
                disabled: false,
                autofocus: false,
                click() {
                    instance.dismiss();
                }
            },
            title: self._client.scope.translate.instant('HOME_SELF_DEFINE'),
            contentTitle: '',
            content: domSanitizer.bypassSecurityTrustHtml(inputTemplate)
        });
    }

    _ConfirmPowerWindow(thisobj) {
        const self = this;
        const tiMessage = self._client.scope.tiMessage;
        let confirmContent = null;
        switch (thisobj) {
            case 0:
                confirmContent = 'WEB_COMFRIM_CONTEXT';
                break;
            case 1:
                confirmContent = 'STRONG_DOWN_POWER_TIP';
                break;
            case 2:
                confirmContent = 'WEB_COMFRIM_CONTEXT';
                break;
            case 3:
                confirmContent = 'POWER_REST_START_TIP';
                break;
            case 4:
                confirmContent = 'POWER_OFF_POWER_ON_TIP';
                break;
            default:
                confirmContent = 'WEB_COMFRIM_CONTEXT';
                return;
        }
        const instance = tiMessage.open({
            type: 'prompt',
            id: 'kvmPower',
            closeable: false,
            okButton: {
                show: true,
                text: self._client.scope.translate.instant('COMMON_OK'),
                disabled: false,
                autofocus: false,
                click() {
                    self._PowerEvent(thisobj);
                    instance.close();
                }
            },
            cancelButton: {
                show: true,
                text: self._client.scope.translate.instant('COMMON_CANCEL'),
                disabled: false,
                autofocus: false,
                click() {
                    instance.dismiss();
                }
            },
            title: self._client.scope.translate.instant('COMMON_CONFIRM'),
            contentTitle: '',
            content: self._client.scope.translate.instant(confirmContent)
        });
    }

    setDefineKey(e) {
        const target = e.srcElement || e.originalTarget;
        const eltId = target.id;
        let keysym;
        const keyLocation = Number(e.location || e.keyLocation);
        if (typeof e.key !== 'undefined' && e.key !== 'MozPrintableKey' && e.key !== 'Alt' && keyLocation !== 2) {
            keysym = e.key;
        } else if ((e.key !== 'MozPrintableKey' && e.key === 'Alt' && keyLocation === 2) || e.key === 'AltGraph') {
            // 意大利键盘模式下，按下右边Alt键，组合键输入框内应显示为"AltGraph"
            keysym = 'AltGraph';
        } else {
            keysym = this._GetKeysym(e);
        }
        if (
            eltId === 'txt_define1' ||
            eltId === 'txt_define2' ||
            eltId === 'txt_define3' ||
            eltId === 'txt_define4' ||
            eltId === 'txt_define5' ||
            eltId === 'txt_define6'
        ) {
            const input = document.getElementById(eltId) as HTMLInputElement;
            input.value = keysym;
            document.getElementById(eltId).setAttribute('key', keysym);
            document.getElementById(eltId).setAttribute('keyCode', e.keyCode || e.which || e.charCode);
        }
    }

    _GetKeysym(e) {
        const keyCode = Number(e.keyCode || e.which || e.charCode);
        for (const uskeyboard of uskeyboardTable) {
            if (uskeyboard[0] === keyCode) {
                if (
                    (keyCode >= JS_KEY_A && keyCode <= JS_KEY_Z) ||
                    (keyCode >= JS_KEY_0 && keyCode <= JS_KEY_9) ||
                    (keyCode >= JS_KEY_SEMI && keyCode <= JS_KEY_QUOTO) ||
                    (keyCode >= JS_KEY_BR1 && keyCode <= JS_KEY_QUOT) ||
                    keyCode === JS_KEY_FF_SEMI ||
                    keyCode === JS_KEY_FF_PLUS ||
                    keyCode === JS_KEY_FF_MIN
                ) {
                    return e.shiftKey ? uskeyboard[4] : uskeyboard[3];
                } else if ((keyCode >= JS_KEY_0P && keyCode <= JS_KEY_DIVP) || keyCode === JS_KEY_OR_1) {
                    return uskeyboard[3];
                } else {
                    return uskeyboard[2];
                }
            }

        }
        return 'Unknow';
    }
}
