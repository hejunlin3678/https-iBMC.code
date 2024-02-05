export class UpgradeModel {
    constructor() { }
    private activeModeSupported: boolean;
    private activeMode: string;
    private updatingFlag: boolean;

    get getActiveModeSupported(): boolean {
        return this.activeModeSupported;
    }

    public setActiveModeSupported(value) {
        this.activeModeSupported = value;
    }

    get getActiveMode(): string {
        return this.activeMode;
    }

    public setActiveMode(value) {
        this.activeMode = value;
    }

    get getUpdatingFlag(): boolean {
        return this.updatingFlag;
    }

    public setUpdatingFlag(value) {
        this.updatingFlag = value;
    }
}
