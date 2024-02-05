import { AbstractNIC } from '../abstract-nic';
import { NIC } from '../nic';

export class BridgeArr extends AbstractNIC {
    constructor(nicArr: NIC[]) {
        super();
        this.id = 'bridgeCard';
        this.type = 'bridge';
        this.label = 'Bridge';
        this.cards = nicArr;
    }
}
