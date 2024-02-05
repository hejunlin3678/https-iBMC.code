import { PortArr } from '../port-attr';
import { NetPort } from './net-port';

export class NetPortArr extends PortArr {
    constructor() {
        super();
        this.titles = [
            {
                title: '',
                width:  '8%'
            },
            {
                title: 'SERVICE_PORT',
                width:  '23%'
            },
            {
                title: 'SERVICE_STATUS',
                width:  '23%'
            },
            {
                title: 'IBMC_NETWORK_PORT_TYPE',
                width:  '23%'
            },
            {
                title: 'NET_MEDIUM_TYPE',
                width:  '23%'
            }
        ];
        this.netPorts = [];
    }

    public addPort(netPort: NetPort): void {
        this.netPorts.push(netPort);
    }

}
