import { Card } from '../card';
import { OCPCard } from './ocp-card';

export class OCPCardArray extends Card {

    private oCPCards: OCPCard[];

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
        this.oCPCards = [];
    }

    get getOCPCards(): OCPCard[] {
        return this.oCPCards;
    }

    addOCPCard(oCPCard: OCPCard): void {
        this.oCPCards.push(oCPCard);
    }
}
