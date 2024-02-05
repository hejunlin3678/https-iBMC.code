import { Card } from '../card';
import { PCIERiserCard } from './pcie-riser-card';

export class PCIERiserCardArray extends Card {

    private pCIERiserCards: PCIERiserCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '20%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '12%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '10%'
            }, {
                title: 'COMMON_TYPE',
                width: '16%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '9%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '8%'
            }, {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '15%'
            }
        ];
        this.pCIERiserCards = [];
    }

    get getPCIERiserCards(): PCIERiserCard[] {
        return this.pCIERiserCards;
    }

    addPCIERiserCard(pCIERiserCard: PCIERiserCard): void {
        this.pCIERiserCards.push(pCIERiserCard);
    }

}
