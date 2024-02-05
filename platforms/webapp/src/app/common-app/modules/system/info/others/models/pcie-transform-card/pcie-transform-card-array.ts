import { PCIETransformCard } from './pcie-transform-card';
import { Card } from '../card';

export class PCIETransformCardArray extends Card {

    private pCIETransformCards: PCIETransformCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '20%'
            }, {
                title: 'COMMON_DESCRIPTION',
                width: '38%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '14%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '14%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '14%'
            },
        ];
        this.pCIETransformCards = [];
    }

    get getPCIETransformCards(): PCIETransformCard[] {
        return this.pCIETransformCards;
    }

    addPCIETransformCard(pCIETransformCard: PCIETransformCard): void {
        this.pCIETransformCards.push(pCIETransformCard);
    }

}
