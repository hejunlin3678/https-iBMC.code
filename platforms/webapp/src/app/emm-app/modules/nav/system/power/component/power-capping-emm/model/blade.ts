export class Blade {
    public id: string;
    public actualPower: number | string;
    public capValue: number;
    public recommendMinCapValue: number;
    public isManual: boolean;
    public failurePolicy: string;
    constructor(param: Blade) {
        const {
            id,
            actualPower,
            capValue,
            recommendMinCapValue,
            isManual,
            failurePolicy
        } = param;
        this.id = id;
        this.actualPower = actualPower;
        this.capValue = capValue;
        this.recommendMinCapValue = recommendMinCapValue;
        this.isManual = isManual;
        this.failurePolicy = failurePolicy;
    }
}
