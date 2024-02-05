import { NIC } from '../nic';
import { PortArr } from '../ports/port-attr';

export class TeamCard extends NIC {
    public cardName: [string, string];
    public model: [string, string];
    public linkStatus: [string, boolean];
    public workModel: [string, string];
    public iPv4Addresses: [string, string];
    public iPv6Addresses: [string, string];
    public macAddress: [string, string];
    public vlans: [string, string];
    public ports: PortArr;
    constructor(
        name: string,
        id: string,
        moreInfo: object,
        init: boolean,
        cardName: string,
        linkStatus: boolean,
        workModel: string,
        iPv4Addresses: string,
        iPv6Addresses: string,
        macAddress: string,
        vlans: string
    ) {
        super(name, id, moreInfo, init);
        this.cardName = ['COMMON_NAME', cardName];
        this.linkStatus = ['NET_CONNENT_STATUS', linkStatus];
        this.workModel = ['COMMON_WORKING_MODE', workModel];
        this.iPv4Addresses = ['NET_IPV4', iPv4Addresses];
        this.iPv6Addresses = ['NET_IPV6', iPv6Addresses];
        this.macAddress = ['IBMC_IP_ADDRESS04', macAddress];
        this.vlans = ['NET_VLANS', vlans];
    }

    set setPort(ports: PortArr) {
        this.ports = ports;
    }

    get getPort(): PortArr {
        return this.ports;
    }
}
