export class PowerCappingHistory {
    private powerPeakWattsList: object[];
    private powerAverageWattsList: object[];
    private timeList: string[];

    constructor(powerPeakWattsList, powerAverageWattsList, timeList) {
        this.powerPeakWattsList = powerPeakWattsList;
        this.powerAverageWattsList = powerAverageWattsList;
        this.timeList = timeList;
    }

    public getPowerPeakWattsList() {
        return this.powerPeakWattsList;
    }

    public setPowerPeakWattsList(value) {
        this.powerPeakWattsList = value;
    }

    public getPowerAverageWattsList() {
        return this.powerAverageWattsList;
    }

    public setPowerAverageWattsList(value) {
        this.powerAverageWattsList = value;
    }

    public getTimeList() {
        return this.timeList;
    }

    public setTimeList(value: string[]) {
        this.timeList = value;
    }
}
