import { AbstractNIC } from '../abstract-nic';
import { NIC } from '../nic';

export class TeamArr extends AbstractNIC {
    constructor(nicArr: NIC[]) {
        super();
        this.id = 'teamCard';
        this.type = 'team';
        this.label = 'Team';
        this.cards = nicArr;
    }
}
