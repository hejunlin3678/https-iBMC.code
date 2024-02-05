import { PortArr } from '../port-attr';
import { TeamPort } from './team-port';

export class TeamPortArr extends PortArr {
    constructor() {
        super();
        this.titles = [
            {
                title: 'NET_NAME',
                width:  '23%'
            },
            {
                title: 'IBMC_NETWORK_PORT_NAME',
                width:  '23%'
            },
            {
                title: 'SERVICE_PORT',
                width:  '23%'
            },
            {
                title: 'IBMC_IP_ADDRESS04',
                width:  '23%'
            },
            {
                title: 'STATUS',
                width:  '23%'
            }
        ];
        this.netPorts = [];
    }

    public addPort(teamPort: TeamPort): void {
        this.netPorts.push(teamPort);
    }

}
