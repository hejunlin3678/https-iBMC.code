import { NIC } from './nic';

export abstract class AbstractNIC {
    id: string;
    type: string;
    label: string;
    cards: NIC[];
}
