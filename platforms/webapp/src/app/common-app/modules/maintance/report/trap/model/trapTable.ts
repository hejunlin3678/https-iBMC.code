export class TrapTableInfo {
    private num: string|number;
    private serverAddr: string;
    private port: string|number;
    private trans: boolean;
    private status:  boolean;
    constructor(tableInfo) {
        this.num = tableInfo.num;
        this.serverAddr = tableInfo.serverAddr;
        this.port = tableInfo.port;
        this.trans = tableInfo.trans;
        this.status = tableInfo.status;
    }
}
