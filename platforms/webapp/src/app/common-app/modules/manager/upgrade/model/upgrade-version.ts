export class UpgradeVersion {
    constructor() { }
    private taskMembers: string;
    private activeBMC: string;
    private backupBMC: string;
    private availableBMC: string;
    private unavailable: boolean;

    get getTaskMembers(): string {
        return this.taskMembers;
    }

    public setTaskMembers(value) {
        this.taskMembers = value;
    }

    get getActiveBMC(): string {
        return this.activeBMC;
    }

    public setActiveBMC(value: string) {
        this.activeBMC = value;
    }

    get getBackupBMC(): string {
        return this.backupBMC;
    }

    public setBackupBMC(value) {
        this.backupBMC = value;
    }

    get getAvailableBMC(): string {
        return this.availableBMC;
    }

    public setAvailableBMC(value) {
        this.availableBMC = value;
    }

    get getUnavailable(): boolean {
        return this.unavailable;
    }

    public setUnavailable(value) {
        this.unavailable = value;
    }

}
