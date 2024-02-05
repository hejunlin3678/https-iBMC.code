
export class IPAddress {
    constructor(param: {
        type: string,
        slotNum: string,
        address?: string,
        subnetMask?: string,
        gateWay: string,
        dhcpModel: boolean,
        prefix?: string,
        startAddress?: string,
        slaacQuantity?: string[]
    }) {
        Object.keys(param).forEach((key) => {
            this[key] = param[key];
        });
    }
    public type: 'ipv4' | 'ipv6';
    public slotNum: string;
    public address: string;
    public subnetMask: string;
    public gateWay: string;
    public dhcpModel: boolean;
    public prefix: string;
    public startAddress: string;
    public slaacQuantity: string;
}
