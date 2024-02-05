import { IPAddressControlPrefix } from './data.types';

export class IPV4Address {
    constructor(param: {
        type: string,
        slotNum: string,
        address: string,
        subnetMask: string,
        gateWay: string,
        dhcpModel: boolean,
        url: string;
    }) {
        Object.keys(param).forEach((key) => {
            this[key] = param[key];
        });
        this.gateControlName = `${IPAddressControlPrefix.GATEWAY}${param.slotNum}`;
        this.ipControlName = `${IPAddressControlPrefix.IPADRESS}${param.slotNum}`;
        this.maskControlName = `${IPAddressControlPrefix.SUBNETMASK}${param.slotNum}`;
        this.dhcpControlName = `${IPAddressControlPrefix.DHCP}${param.slotNum}`;
    }

    public type: string;
    public slotNum: string;
    public address: string;
    public subnetMask: string;
    public gateWay: string;
    public dhcpModel: boolean;
    public url: string;
    public gateControlName: string = '';
    public ipControlName: string = '';
    public maskControlName: string = '';
    public dhcpControlName: string = '';
}
