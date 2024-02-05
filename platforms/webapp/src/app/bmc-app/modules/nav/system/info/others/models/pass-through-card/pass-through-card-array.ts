import { PassThroughCard } from './pass-through-card';
import { Card } from '../card';

export class PassThroughCardArray extends Card {

    private passThroughCards: PassThroughCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '10%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '10%'
            }, {
                title: 'COMMON_DESCRIPTION',
                width: '20%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '10%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '10%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '10%'
            }, {
                title: 'OTHER_PART_NUMBER',
                width: '12%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '18%'
            }
        ];
        this.passThroughCards = [];
    }

    get getPassThroughCards(): PassThroughCard[] {
        return this.passThroughCards;
    }

    addPassThroughCard(passThroughCard: PassThroughCard): void {
        this.passThroughCards.push(passThroughCard);
    }

}
