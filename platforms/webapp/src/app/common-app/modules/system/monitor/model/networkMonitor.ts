import { NetworkCharts } from './networkCharts';

export class NetworkMonitor {
    private id: number;
    private label: string;
    private charts: NetworkCharts;

    constructor(id: number, label: string, charts: NetworkCharts) {
        this.id = id;
        this.label = label;
        this.charts = charts;
    }

    public getId() {
        return this.id;
    }

    public setId(value: number) {
        this.id = value;
    }

    public getLabel() {
        return this.label;
    }

    public setLabel(value: string) {
        this.label = value;
    }

    public getCharts() {
        return this.charts;
    }

    public setCharts(value: NetworkCharts) {
        this.charts = value;
    }

}
