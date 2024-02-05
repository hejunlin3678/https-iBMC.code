import { CardUrlArray } from './cardUrlArray';
export class NavTree {

    private cardUrlArray: CardUrlArray[];

    constructor() {
        this.cardUrlArray = [];
    }
    get getCardUrlArray() {
        return this.cardUrlArray;
    }

    public addCardUrl(cardUrlArray: CardUrlArray): void {
        this.cardUrlArray.push(cardUrlArray);
    }

}
