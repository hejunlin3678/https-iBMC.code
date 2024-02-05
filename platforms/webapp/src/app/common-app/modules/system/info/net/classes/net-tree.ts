import { AbstractNIC } from './abstract-nic';

export class NetTree {

    public cardArray: AbstractNIC[];

    constructor() {
        this.cardArray = [];
    }

    get getCardArray(): AbstractNIC[] {
        return this.cardArray;
    }

    public addAbstractNIC(abstractNIC: AbstractNIC): void {
        this.cardArray.push(abstractNIC);
    }
}
