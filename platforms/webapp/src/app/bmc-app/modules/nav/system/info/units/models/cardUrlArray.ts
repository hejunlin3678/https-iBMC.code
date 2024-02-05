import { Card } from '../../others/models';

export class CardUrlArray {

    private cardUrlArray: string[];
    private cardArray: Card;
    private name: string;
    private componentName: string;
    private active: boolean;
    private maxCount: number;
    private totalCount: number;

    constructor(cardUrlArray: string[], name: string, componentName: string, maxCount: number, totalCount: number) {
        this.cardUrlArray = cardUrlArray;
        this.cardArray = new Card();
        this.name = name;
        this.componentName = componentName;
        this.active = false;
        this.maxCount = maxCount;
        this.totalCount = totalCount;
    }

    set setActive(active: boolean) {
        this.active = active;
    }

    set setCardArray(card: Card) {
        this.cardArray = card;
    }

    get getName() {
        return this.name;
    }

    get getMaxCount() {
        return this.maxCount;
    }

    get getTotalCount() {
        return this.totalCount;
    }

    get getComponentName() {
        return this.componentName;
    }

    get getCardUrlArray() {
        return this.cardUrlArray;
    }

    get getCardArray() {
        return this.cardArray;
    }
}
