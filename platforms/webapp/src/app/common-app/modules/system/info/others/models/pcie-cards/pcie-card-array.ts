import { Card } from '../card';
import { PCIeCard } from './pcie-card';

export class PCIeCardArray extends Card {

    private pCIeCards: PCIeCard[];

    constructor() {
        super();
        this.title = [
            {
                title: '',
                width: '5%'
            },
            {
                title: 'COMMON_DESCRIPTION',
                width: '16%'
            },
            {
                title: 'OTHER_POSITION',
                width: '10%'
            },
            {
                title: 'COMMON_MANUFACTURER',
                width: '15%'
            },
            {
                title: 'OTHER_SLOT_POSITION',
                width: '8%'
            },
            {
                title: 'OTHER_MANUFACTURER_ID',
                width: '10%'
            },
            {
                title: 'OTHER_DEVICE_ID',
                width: '10%'
            },
            {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            },
            {
                title: 'OTHER_SON_VENDOR_ID',
                width: '10%'
            },
            {
                title: 'OTHER_SON_DEVICE_ID',
                width: '10%'
            },
            {
                title: 'COMMON_RESOURCE_OWNER',
                width: '10%'
            }
        ];
        this.pCIeCards = [];
    }

    set setPCIeCards(pCIeCards: PCIeCard[]) {
        this.pCIeCards = pCIeCards;
    }

    get getPCIeCards(): PCIeCard[] {
        return this.pCIeCards;
    }

    get getTitle(): object[] {
        return this.title;
    }

    addPCIeCards(pCIeCard: PCIeCard): void {
        this.pCIeCards.push(pCIeCard);
    }

}
