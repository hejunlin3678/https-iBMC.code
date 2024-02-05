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
    // 备用ipv4地址
    private ipAddressV4Standby: string;
    // 备用ipv4子网掩码
    private maskCodeV4Standby: string;
    // 备用ipv6地址
    private ipAddressV6Standby: string;
    // 备用ipv6前缀
    private prefixV6Standby: string;
    // 备用ipv6链路本地地址
    private addressValueV6Standby: string;
    // 备用ipv6 IP地址2
    private iPAddresslabelV6Standby: string;
    // 浮动ipv4地址
    private ipAddressV4Float: string;
    // 浮动ipv4子网掩码
    private maskCodeV4Float: string;
    // 浮动ipv6地址
    private ipAddressV6Float: string;
    // 浮动ipv6前缀
    private prefixV6Float: string;
    // 备板是否在位
    private standbyStatus: boolean;


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
    public getIpAddressV4Standby() {
        return this.ipAddressV4Standby;
    }
    public setIpAddressV4Standby(value) {
        this.ipAddressV4Standby = value;
    }
    public getMaskCodeV4Standby() {
        return this.maskCodeV4Standby;
    }
    public setMaskCodeV4Standby(value) {
        this.maskCodeV4Standby = value;
    }
    public getIpAddressV6Standby() {
        return this.ipAddressV6Standby;
    }
    public setIpAddressV6Standby(value) {
        this.ipAddressV6Standby = value;
    }
    public getPrefixV6Standby() {
        return this.prefixV6Standby;
    }
    public setPrefixV6Standby(value) {
        this.prefixV6Standby = value;
    }
    public getAddressValueV6Standby() {
        return this.addressValueV6Standby;
    }
    public setAddressValueV6Standby(value) {
        this.addressValueV6Standby = value;
    }
    public getIPAddresslabelV6Standby() {
        return this.iPAddresslabelV6Standby;
    }
    public setIPAddresslabelV6Standby(value) {
        this.iPAddresslabelV6Standby = value;
    }
    public getIpAddressV4Float() {
        return this.ipAddressV4Float;
    }
    public setIpAddressV4Float(value) {
        this.ipAddressV4Float = value;
    }
    public getMaskCodeV4Float() {
        return this.maskCodeV4Float;
    }
    public setMaskCodeV4Float(value) {
        this.maskCodeV4Float = value;
    }
    public getIpAddressV6Float() {
        return this.ipAddressV6Float;
    }
    public setIpAddressV6Float(value) {
        this.ipAddressV6Float = value;
    }
    public getPrefixV6Float() {
        return this.prefixV6Float;
    }
    public setPrefixV6Float(value) {
        this.prefixV6Float = value;
    }
    public getStandbyStatust() {
        return this.standbyStatus;
    }
    public setStandbyStatus(value) {
        this.standbyStatus = value;
    }
}
export interface IPInfo {
    Active?: any[];
    Standby?: any[];
    Float?: any[];
}
