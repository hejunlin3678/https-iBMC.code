export class UpgradeModel {
    constructor() { }
    private firmBtnRest: boolean;
    private firmBtnBack: boolean;
    private firmFlag: boolean;
    private configFinish: boolean;
    private tipMessage: string;
    private upgradeResult: string;
    private addFileBtn: boolean;

    get getFileBtn(): boolean {
        return this.addFileBtn;
    }

    public setFileBtn(value: boolean) {
        this.addFileBtn = value;
    }

    get getFirmBtnRest(): boolean {
        return this.firmBtnRest;
    }

    public setFirmBtnRest(value: boolean) {
        this.firmBtnRest = value;
    }

    get getFirmBtnBack(): boolean {
        return this.firmBtnBack;
    }

    public setFirmBtnBack(value: boolean) {
        this.firmBtnBack = value;
    }

    get getFirmFlag(): boolean {
        return this.firmFlag;
    }

    public setFirmFlag(value: boolean) {
        this.firmFlag = value;
    }

    get getConfigFinish(): boolean {
        return this.configFinish;
    }

    public setConfigFinish(value: boolean) {
        this.configFinish = value;
    }

    get getTipMessage(): string {
        return this.tipMessage;
    }

    public setTipMessage(value: string) {
        this.tipMessage = value;
    }

    get getUpgradeResult(): string {
        return this.upgradeResult;
    }

    public setUpgradeResult(value: string) {
        this.upgradeResult = value;
    }


}
