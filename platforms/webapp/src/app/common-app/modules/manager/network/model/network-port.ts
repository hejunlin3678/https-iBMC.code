export class NetworkPort {
    constructor() { }
    // 主机名
    private hostName: string;

    // 网口
    private productType: string;
    // 网口模式
    private schemaRadio: string;
    // NCSI模式
    private ncsiRadio: string;
    private networkPortType: string;
    // 专用网口
    private tempDedicated: string;
    private countDedicated: string;
    private tempCheckDedicated: string;
    // 汇聚网口
    private tempAggregation: string;
    private countAggregation: string;
    private tempCheckAggregation: string;
    // 板载网口
    private tempExtend: string;
    private countExtend: string;
    private tempCheckExtend: string;
    // 板载网口2
    private tempExtend2: string;
    private countExtend2: string;
    private tempCheckExtend2: string;
    // PCIE扩展网口
    private tempPcie: string;
    private countPcie: string;
    private tempCheckPcie: string;
    // OCP扩展网口
    private tempOcp: string;
    private countOcp: string;
    private tempCheckOcp: string;
    // OCP2扩展网口
    private tempOcp2: string;
    private countOcp2: string;
    private tempCheckOcp2: string;

    // 主机名
    public getHostName() {
        return this.hostName;
    }
    public setHostName(value) {
        this.hostName = value;
    }

    // 网口
    get getProductType(): string {
        return this.productType;
    }
    public setProductType(value) {
        this.productType = value;
    }

    // 网口模式
    public getSchemaRadio() {
        return this.schemaRadio;
    }
    public setSchemaRadio(value) {
        this.schemaRadio = value;
    }

    public getNcsiRadio() {
        return this.ncsiRadio;
    }
    public setNcsiRadio(value) {
        this.ncsiRadio = value;
    }

    public getNetworkPortType() {
        return this.networkPortType;
    }
    public setNetworkPortType(value) {
        this.networkPortType = value;
    }

    public getTempDedicated() {
        return this.tempDedicated;
    }
    public setTempDedicated(value) {
        this.tempDedicated = value;
    }

    public getCountDedicated() {
        return this.countDedicated;
    }
    public setCountDedicated(value) {
        this.countDedicated = value;
    }

    public getTempCheckDedicated() {
        return this.tempCheckDedicated;
    }
    public setTempCheckDedicated(value) {
        this.tempCheckDedicated = value;
    }

    public getTempAggregation() {
        return this.tempAggregation;
    }
    public setTempAggregation(value) {
        this.tempAggregation = value;
    }

    public getCountAggregation() {
        return this.countAggregation;
    }
    public setCountAggregation(value) {
        this.countAggregation = value;
    }

    public getTempCheckAggregation() {
        return this.tempCheckAggregation;
    }
    public setTempCheckAggregation(value) {
        this.tempCheckAggregation = value;
    }

    public getTempExtend() {
        return this.tempExtend;
    }
    public setTempExtend(value) {
        this.tempExtend = value;
    }

    public getCountExtend() {
        return this.countExtend;
    }
    public setCountExtend(value) {
        this.countExtend = value;
    }

    public getTempCheckExtend() {
        return this.tempCheckExtend;
    }
    public setTempCheckExtend(value) {
        this.tempCheckExtend = value;
    }

    public getTempExtend2() {
        return this.tempExtend2;
    }
    public setTempExtend2(value) {
        this.tempExtend2 = value;
    }

    public getCountExtend2() {
        return this.countExtend2;
    }
    public setCountExtend2(value) {
        this.countExtend2 = value;
    }

    public getTempCheckExtend2() {
        return this.tempCheckExtend2;
    }
    public setTempCheckExtend2(value) {
        this.tempCheckExtend2 = value;
    }

    public getTempPcie() {
        return this.tempPcie;
    }
    public setTempPcie(value) {
        this.tempPcie = value;
    }

    public getCountPcie() {
        return this.countPcie;
    }
    public setCountPcie(value) {
        this.countPcie = value;
    }

    public getTempCheckPcie() {
        return this.tempCheckPcie;
    }
    public setTempCheckPcie(value) {
        this.tempCheckPcie = value;
    }

    public getTempOcp() {
        return this.tempOcp;
    }
    public setTempOcp(value) {
        this.tempOcp = value;
    }

    public getCountOcp() {
        return this.countOcp;
    }
    public setCountOcp(value) {
        this.countOcp = value;
    }

    public getTempCheckOcp() {
        return this.tempCheckOcp;
    }
    public setTempCheckOcp(value) {
        this.tempCheckOcp = value;
    }
    public getTempOcp2() {
        return this.tempOcp2;
    }
    public setTempOcp2(value) {
        this.tempOcp2 = value;
    }
    public getCountOcp2() {
        return this.countOcp2;
    }
    public setCountOcp2(value) {
        this.countOcp2 = value;
    }
    public getTempCheckOcp2() {
        return this.tempCheckOcp2;
    }
    public setTempCheckOcp2(value) {
        this.tempCheckOcp2 = value;
    }
}
