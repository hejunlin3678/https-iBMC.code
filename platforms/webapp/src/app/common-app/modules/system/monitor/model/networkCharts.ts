export class NetworkCharts {

    private ports: string[];
    private times: string[];
    private datas: any[];

    constructor(ports: string[], times: string[], datas: any[]) {
        this.ports = ports;
        this.times = times;
        this.datas = datas;
    }

    public getPorts() {
        return this.ports;
    }

    public setPorts(value: string[]) {
        this.ports = value;
    }

    public getTimes() {
        return this.times;
    }

    public setTimes(value: string[]) {
        this.times = value;
    }

    public getDatas() {
        return this.datas;
    }

    public setDatas(value: string[]) {
        this.datas = value;
    }

}
