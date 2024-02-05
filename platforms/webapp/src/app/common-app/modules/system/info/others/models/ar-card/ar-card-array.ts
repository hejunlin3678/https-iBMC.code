import { Card } from '../card';
import { ArCard } from './ar-card';

export class ArCardArray extends Card {

    public arCard: ArCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'OTHER_BOARDNAME',
                width: '15%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '15%'
            }, {
                title: 'COMMON_TYPE',
                width: '15%'
            }, {
                title: 'OTHER_POSITION',
                width: '15%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '15%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '15%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '15%'
            }
        ];
        this.arCard = [];
    }

    get getarCard(): ArCard[] {
        return this.arCard;
    }
    addArCard(arCard: ArCard): void {
        this.arCard.push(arCard);
    }
}
