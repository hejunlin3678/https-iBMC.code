import { Card } from '../card';
import { MezzCard } from './mezz-card';

export class MezzCardArray extends Card {
    private mezzCards: MezzCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '6%'
            }, {
                title: 'COMMON_DESCRIPTION',
                width: '10%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '6%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '6%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '6%'
            }, {
                title: 'COMMON_RESOURCE_OWNER',
                width: '6%'
            }, {
                title: 'OTHER_CURRENT_BROAD_BAND',
                width: '10%'
            }, {
                title: 'OTHER_CURRENT_RATE',
                width: '10%'
            }, {
                title: 'OTHER_BOARD_SPECIFICATIONS',
                width: '10%'
            }, {
                title: 'OTHER_RATE_SPECIFICATIONS',
                width: '10%'
            }, {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '10%'
            }
        ];
        this.mezzCards = [];
    }

    get getMezzCards(): MezzCard[] {
        return this.mezzCards;
    }

    addMezzCard(mezzCard: MezzCard): void {
        this.mezzCards.push(mezzCard);
    }
}
