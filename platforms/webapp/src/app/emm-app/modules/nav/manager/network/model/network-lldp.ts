export class NetworkLldp {
    constructor() { }
    // lldp
    private lldpSupport: string;
    private lldpEnabled: string;
    private lldpWorkMode: string;
    private txDelaySeconds: string;
    private txIntervalSeconds: string;
    private txHold: string;

    // lldp
    get getLldpSupport(): string {
        return this.lldpSupport;
    }
    public setLldpSupport(value) {
        this.lldpSupport = value;
    }

    public getLldpEnabled() {
        return this.lldpEnabled;
    }
    public setLldpEnabled(value) {
        this.lldpEnabled = value;
    }

    public getLldpWorkMode() {
        return this.lldpWorkMode;
    }
    public setLldpWorkMode(value) {
        this.lldpWorkMode = value;
    }

    public getTxDelaySeconds() {
        return this.txDelaySeconds;
    }
    public setTxDelaySeconds(value) {
        this.txDelaySeconds = value;
    }

    public getTxIntervalSeconds() {
        return this.txIntervalSeconds;
    }
    public setTxIntervalSeconds(value) {
        this.txIntervalSeconds = value;
    }

    public getTxHold() {
        return this.txHold;
    }
    public setTxHold(value) {
        this.txHold = value;
    }
}
