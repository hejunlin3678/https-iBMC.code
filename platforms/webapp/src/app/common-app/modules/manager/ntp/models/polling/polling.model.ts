export class Polling {
    private minPollingInterval: number;
    private maxPollingInterval: number;

    constructor(minPollingInterval: number, maxPollingInterval: number) {
        this.minPollingInterval = minPollingInterval;
        this.maxPollingInterval = maxPollingInterval;
    }

    set setMinPollingInterval(minPollingInterval: number) {
        this.minPollingInterval = minPollingInterval;
    }

    set setMaxPollingInterval(maxPollingInterval: number) {
        this.maxPollingInterval = maxPollingInterval;
    }

    get getMinPollingInterval(): number {
        return this.minPollingInterval;
    }

    get getMaxPollingInterval(): number {
        return this.maxPollingInterval;
    }
}
