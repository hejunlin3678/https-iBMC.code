import { Card } from '../card';
import { PeripheralCard } from './peripheral-card';

export class PeripheralCardArray extends Card {

    public peripheralCards: PeripheralCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME'
            }, {
                title: 'COMMON_TYPE'
            }, {
                title: 'COMMON_MANUFACTURER'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION'
            }, {
                title: 'OTHER_BOARD_ID'
            }
        ];
        this.peripheralCards = [];
    }

    get getPeripheralCards(): PeripheralCard[] {
        return this.peripheralCards;
    }

    addPeripheralCards(peripheralCard: PeripheralCard) {
        this.peripheralCards.push(peripheralCard);
    }
}
