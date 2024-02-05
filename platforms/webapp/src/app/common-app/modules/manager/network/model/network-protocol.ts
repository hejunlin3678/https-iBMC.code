export class NetworkProtocol {
    constructor() { }
    // 网络协议
    private netIPVersion: string;
    private netModelV4: string;
    private ipAddressV4: string;
    private maskCodeV4: string;
    private gatewayV4: string;
    private addMacV4: string;
    private netModelV6: string;
    private ipAddressV6: string;
    private maskCodeV6: string;
    private gatewayV6: string;
    private addressValueV6: string;
    private iPAddresslabel: string;
    private ipAddressShow: string;

    // 网络协议
    public getNetIPVersion() {
        return this.netIPVersion;
    }
    public setNetIPVersion(value) {
        this.netIPVersion = value;
    }

    public getNetModelV4() {
        return this.netModelV4;
    }
    public setNetModelV4(value) {
        this.netModelV4 = value;
    }

    public getIpAddressV4() {
        return this.ipAddressV4;
    }
    public setIpAddressV4(value) {
        this.ipAddressV4 = value;
    }

    public getMaskCodeV4() {
        return this.maskCodeV4;
    }
    public setMaskCodeV4(value) {
        this.maskCodeV4 = value;
    }

    public getGatewayV4() {
        return this.gatewayV4;
    }
    public setGatewayV4(value) {
        this.gatewayV4 = value;
    }

    public getAddMacV4() {
        return this.addMacV4;
    }
    public setAddMacV4(value) {
        this.addMacV4 = value;
    }

    public getNetModelV6() {
        return this.netModelV6;
    }
    public setNetModelV6(value) {
        this.netModelV6 = value;
    }

    public getIpAddressV6() {
        return this.ipAddressV6;
    }
    public setIpAddressV6(value) {
        this.ipAddressV6 = value;
    }

    public getMaskCodeV6() {
        return this.maskCodeV6;
    }
    public setMaskCodeV6(value) {
        this.maskCodeV6 = value;
    }

    public getGatewayV6() {
        return this.gatewayV6;
    }
    public setGatewayV6(value) {
        this.gatewayV6 = value;
    }

    public getAddressValueV6() {
        return this.addressValueV6;
    }
    public setAddressValueV6(value) {
        this.addressValueV6 = value;
    }

    public getIPAddresslabel() {
        return this.iPAddresslabel;
    }
    public setIPAddresslabel(value) {
        this.iPAddresslabel = value;
    }

    public getIpAddressShow() {
        return this.ipAddressShow;
    }
    public setIpAddressShow(value) {
        this.ipAddressShow = value;
    }
}
