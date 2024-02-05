export class AirInletTemperature {

    private inletTemperature: number | string;
    private time: string[];
    private inletTempCelsius: number[];

    constructor(inletTemperature: number | string, time: string[], inletTempCelsius: number[]) {
        this.inletTemperature = inletTemperature;
        this.time = time;
        this.inletTempCelsius = inletTempCelsius;
    }

    public getInletTemperature(): number | string {
        return this.inletTemperature;
    }

    public setInletTemperature(value: number | string) {
        this.inletTemperature = value;
    }

    public setTime(value: string[]) {
        this.time = value;
    }

    public getTime(): string[] {
        return this.time;
    }

    public setInletTempCelsius(value: number[]) {
        this.inletTempCelsius = value;
    }

    public getInletTempCelsius(): number[] {
        return this.inletTempCelsius;
    }
}
