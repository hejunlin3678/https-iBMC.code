import { AbstractNIC } from '../abstract-nic';
import { NIC } from '../nic';

export class FcCardArr extends AbstractNIC {
    constructor(nicArr: NIC[]) {
        super();
        this.id = 'fcCard';
        this.type = 'fc';
        this.label = 'FC';
        this.cards = nicArr;
    }
}
