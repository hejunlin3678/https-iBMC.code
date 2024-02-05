import { AbstractNIC } from '../abstract-nic';
import { NIC } from '../nic';

export class NetCardArr extends AbstractNIC {
    constructor(nicArr: NIC[]) {
        super();
        this.id = 'netCard';
        this.type = 'net';
        this.label = 'NET_NIC';
        this.cards = nicArr;
    }
}
