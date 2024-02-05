import { Card } from '../card';
import { RaidCard } from './raid-card';

export class RaidCardArray extends Card {

    private raidCards: RaidCard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '10%'
            }, {
                title: 'OTHER_POSITION',
                width: '8%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '8%'
            }, {
                title: 'COMMON_NUMBER',
                width: '5%'
            }, {
                title: 'COMMON_TYPE',
                width: '10%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '7%'
            }, {
                title: 'COMMIN_CPLD_VERSION',
                width: '9%'
            }, {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '15%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '8%'
            }, {
                title: 'COMMON_RESOURCE_OWNER',
                width: '10%'
            }
        ];
        this.raidCards = [];
    }

    get getRaidCards(): RaidCard[] {
        return this.raidCards;
    }

    addRaidCard(raidCard: RaidCard): void {
        this.raidCards.push(raidCard);
    }
}
