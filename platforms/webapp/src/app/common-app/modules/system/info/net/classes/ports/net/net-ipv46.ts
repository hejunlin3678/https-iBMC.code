import { ITitle } from 'src/app/common-app/models';
import { IIpv4NetInfo, IIpv6NetInfo } from '../../net.dataType';

export class NetIPv46 {
    private ipv4Titles: ITitle[];
    private ipv6Titles: ITitle[];

    private ipv4MaskGateway: IIpv4NetInfo[];
    private ipv6PrefixGateway: IIpv6NetInfo[];

    constructor(ipv4MaskGateway: IIpv4NetInfo[], ipv6PrefixGateway: IIpv6NetInfo[]) {
        this.ipv4MaskGateway = ipv4MaskGateway;

        this.ipv6PrefixGateway = ipv6PrefixGateway;

        this.ipv4Titles = [{
            title: 'NET_PORT_IPV4'
        }, {
            title: 'IBMC_IP_ADDRESS02'
        }, {
            title: 'NET_GATEWAY'
        }];

        this.ipv6Titles = [{
            width: '40%',
            title: 'NET_PORT_IPV6'
        }, {
            width: '20%',
            title: 'IBMC_IP_ADDRESS11'
        }, {
            width: '40%',
            title: 'NET_GATEWAY'
        }];
    }

    get getIpv4Titles() {
        return this.ipv4Titles;
    }

    get getIpv6Titles() {
        return this.ipv6Titles;
    }

    get getIpv4MaskGateway() {
        return this.ipv4MaskGateway;
    }

    get getIpv6PrefixGateway() {
        return this.ipv6PrefixGateway;
    }
}
