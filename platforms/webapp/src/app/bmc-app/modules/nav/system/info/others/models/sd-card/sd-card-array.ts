import { Card } from '../card';
import { SDCard } from './sd-card';

export class SDCardArray extends Card {

    private sDCards: SDCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'OTHER_SLOT_POSITION'
            }, {
                title: 'COMMON_MANUFACTURER'
            }, {
                title: 'COMMON_SERIALNUMBER'
            }, {
                title: 'COMMON_CAPACITY'
            }
        ];
        this.sDCards = [];
    }

    get getSDCards(): SDCard[] {
        return this.sDCards;
    }

    addSDCard(sDCard: SDCard): void {
        this.sDCards.push(sDCard);
    }
}
