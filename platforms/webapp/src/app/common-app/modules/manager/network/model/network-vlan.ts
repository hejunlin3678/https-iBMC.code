export class NetworkVlan {
    constructor() { }
    // vlan
    private vlanEnable: string;
    private vlanId: string;
    private productTypeVlan: string;

    // vlan
    public getVlanEnable() {
        return this.vlanEnable;
    }

    public setVlanEnable(value) {
        this.vlanEnable = value;
    }

    public getVlanId() {
        return this.vlanId;
    }

    public setVlanId(value) {
        this.vlanId = value;
    }
    // 产品差异区分标识
    get getProductTypeVlan(): string {
        return this.productTypeVlan;
    }
    public setProductTypeVlan(value) {
        this.productTypeVlan = value;
    }
}
