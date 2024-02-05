export class UpgradeModel {
    constructor() { }
    private mainModel: string;

    get getMainModel(): string {
        return this.mainModel;
    }

    public setMainModel(value) {
        this.mainModel = value;
    }
}
