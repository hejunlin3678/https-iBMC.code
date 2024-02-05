export class IbmaInfo {

    private label: string;
    private value: string | boolean;
    constructor(label: string, value: string | boolean) {
        this.label = label;
        this.value = value;
    }

    public getLabel(): string {
        return this.label;
    }

    public setLabel(value: string) {
        this.label = value;
    }

    public getValue(): string | boolean {
        return this.value;
    }

    public setValue(value: string | boolean) {
        this.value = value;
    }

}
