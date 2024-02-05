import { M2TransformCard } from './m2-transform-card';
import { Card } from '../card';

export class M2TransformCardArray extends Card {
    private m2TransformCards: M2TransformCard[];

    constructor() {
        super();
        this.title = [
            {
                label: 'COMMON_NAME',
                width: '10%'
            }, {
                label: 'COMMON_DESCRIPTION',
                width: '30%'
            }, {
                label: 'OTHER_SLOT_POSITION',
                width: '5%'
            }, {
                label: 'COMMON_ALARM_PCB_VERSION',
                width: '10%'
            }, {
                label: 'OTHER_BOARD_ID',
                width: '10%'
            }, {
                label: 'OTHER_PART_NUMBER',
                width: '15%'
            }, {
                label: 'COMMON_SERIALNUMBER',
                width: '20%'
            }
        ];
        this.m2TransformCards = [];
    }

    get getM2TransformCards(): M2TransformCard[] {
        return this.m2TransformCards;
    }

    addM2TransFormCard(m2TransformCard: M2TransformCard): void {
        this.m2TransformCards.push(m2TransformCard);
    }
}
