export class Security {

    // 协议类型
    private protocolType: string;
    // 协议版本
    private protocolVersion: string;
    // 厂商
    private manufacturer: string;
    // 固件版本
    private firmwareVersion: string;
    // 自检
    private selfTestResult: string;

    constructor(protocolType: string, protocolVersion: string, manufacturer: string, firmwareVersion: string, selfTestResult: string) {
        this.protocolType = protocolType;
        this.protocolVersion = protocolVersion;
        this.manufacturer = manufacturer;
        this.firmwareVersion = firmwareVersion;
        this.selfTestResult = selfTestResult;
    }

}
