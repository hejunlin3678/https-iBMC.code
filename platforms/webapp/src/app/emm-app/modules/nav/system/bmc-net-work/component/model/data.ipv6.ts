import { SLACCIPv6Addresses } from './data.interface';
import { IPAddressControlPrefix } from './data.types';

export class IPV6Address {
    constructor(param: {
        type: string,
        slotNum: string,
        address: string,
        prefix: string,
        gateWay: string,
        dhcpModel: boolean,
        slaacQuantity: string[],
        url: string
    }) {
        Object.keys(param).forEach((key) => {
            this[key] = param[key];
        });
        this.gateControlName = `${IPAddressControlPrefix.GATEWAY}${param.slotNum}`;
        this.ipControlName = `${IPAddressControlPrefix.IPADRESS}${param.slotNum}`;
        this.prefixControlName = `${IPAddressControlPrefix.PREFIX}${param.slotNum}`;
        this.dhcpControlName = `${IPAddressControlPrefix.DHCP}${param.slotNum}`;
    }

    public type: string;
    public slotNum: string;
    public address: string;
    public prefix: string;
    public gateWay: string;
    public dhcpModel: boolean;
    public slaacQuantity: SLACCIPv6Addresses[];
    public url: string;
    public gateControlName: string;
    public ipControlName: string;
    public prefixControlName: string;
    public dhcpControlName: string;
}
