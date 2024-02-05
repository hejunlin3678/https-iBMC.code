import { IRadio } from '../common/ntp.interface';
import { NTPFormGroup } from './formGroup.model';
import { NTPStaticModel } from '../common/ntp-static.model';

export class NTPFunctionData {

    private ntpSwitch: boolean;
    private buttonDisable: boolean;
    private ntpCount: number;
    private ntpServes: NTPFormGroup;
    private ntpServesAlter: NTPFormGroup;
    private radioList: IRadio[];
    private radioSelect: IRadio;
    private serveDisable: boolean;
    private systemLocked: boolean;

    constructor(obj?: IShape) {
        this.buttonDisable = obj ? !obj.privilege : true;
        this.ntpSwitch = obj && obj.ntpSwitch || false;
        this.ntpCount = obj && obj.ntpCount || 3;
        if (obj && obj.ntpControl) {
            if (this.ntpCount === 3) {
                this.ntpServes = new NTPFormGroup(obj.ntpControl);
                this.ntpServesAlter = null;
            } else {
                this.ntpServes = new NTPFormGroup(obj.ntpControl);
                this.ntpServesAlter = new NTPFormGroup(obj.ntpControlAlt);
            }
        }
        // 生成DHCP按钮
        this.radioList = NTPStaticModel.getInstance().radioList;
        this.systemLocked = obj ? obj.systemLocked : false;
    }

    set setNTPSwitch(ntpSwitch: boolean) {
        this.ntpSwitch = ntpSwitch;
    }

    get getNTPSwitch(): boolean {
        return this.ntpSwitch;
    }

    get getNTPCount(): number {
        return this.ntpCount;
    }

    public setNTPVal(val: string, id: string, index: number): void {
        this.ntpServes[index].ntpServe.forEach((ntpServe) => {
            if (ntpServe.id === id) {
                ntpServe.value = val;
            }
        });
    }

    public setRaioDisable(disable: boolean, index: number) {
        const radioList = this.radioList;
        radioList[index].disable = this.buttonDisable || disable;
        this.radioList = radioList;
    }

    get getRadioList(): IRadio[] {
        return this.radioList;
    }

    get getNTPServes(): NTPFormGroup {
        return this.ntpServes;
    }

    get getNTPServesAlter(): NTPFormGroup {
        return this.ntpServesAlter;
    }

    set setRadioSelect(radio: IRadio) {
        this.radioSelect = radio;
        this.changeDisable();
    }

    set setSystemLocked(systemLocked: boolean) {
        this.systemLocked = systemLocked;
        this.changeDisable();
    }

    get getSystemLocked() {
        return this.getSystemLocked;
    }

    get getRadioSelect(): IRadio {
        return this.radioSelect;
    }

    get getServeDisable(): boolean {
        return this.serveDisable;
    }

    get getButtonDisable(): boolean {
        return this.buttonDisable;
    }

    private changeDisable() {
        if (this.buttonDisable || this.radioSelect.id === 'IPv4' || this.radioSelect.id === 'IPv6' || this.systemLocked) {
            this.serveDisable = true;
            this.ntpServes.ntpControls.map(ctrl => ctrl.disable());
            if (this.ntpServesAlter) {
                this.ntpServesAlter.ntpControls.map(ctrl => ctrl.disable());
            }
        } else {
            this.serveDisable = false;
            this.ntpServes.ntpControls.map(ctrl => ctrl.enable());
            if (this.ntpServesAlter) {
                this.ntpServesAlter.ntpControls.map(ctrl => ctrl.enable());
            }
        }
    }
}

interface INTPServe {
    ntpServe: INTP[];
}

interface INTP {
    id: string;
    value: string;
    label: string;
}

interface IShape {
    ntpSwitch: boolean;
    ntpCount: number;
    ntpControl: any;
    ntpControlAlt: any;
    privilege: boolean;
    systemLocked: boolean;
}
