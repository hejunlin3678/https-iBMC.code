import { PortArr } from '../port-attr';
import { FcPort } from './fc-port';

export class FcPortArr extends PortArr {
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
                title: 'FC_ID',
                width:  '23%'
            },
            {
                title: 'NET_PORT_TYPE',
                width:  '23%'
            },
            {
                title: 'STATUS',
                width:  '23%'
            }
        ];
        this.netPorts = [];
    }
    addPort(port: FcPort): void {
        this.netPorts.push(port);
    }
}
