export class NetworkDns {
    constructor() { }
    // DNS
    private ipVersion: string;
    private dnsAddress: string;
    private dnsDomain: string;
    private dnsServer: string;
    private dnsServer1: string;
    private dnsServer2: string;
    private netModelV4: string;
    private netModelV6: string;


    // DNS
    public getIpVersion() {
        return this.ipVersion;
    }
    public setIpVersion(value) {
        this.ipVersion = value;
    }

    public getDnsAddress() {
        return this.dnsAddress;
    }
    public setDnsAddress(value) {
        this.dnsAddress = value;
    }

    public getDnsDomain() {
        return this.dnsDomain;
    }
    public setDnsDomain(value) {
        this.dnsDomain = value;
    }

    public getDnsServer() {
        return this.dnsServer;
    }
    public setDnsServer(value) {
        this.dnsServer = value;
    }

    public getDnsServer1() {
        return this.dnsServer1;
    }
    public setDnsServer1(value) {
        this.dnsServer1 = value;
    }

    public getDnsServer2() {
        return this.dnsServer2;
    }
    public setDnsServer2(value) {
        this.dnsServer2 = value;
    }

    public getNetModelV4() {
        return this.netModelV4;
    }
    public setNetModelV4(value) {
        this.netModelV4 = value;
    }

    public getNetModelV6() {
        return this.netModelV6;
    }
    public setNetModelV6(value) {
        this.netModelV6 = value;
    }
}
