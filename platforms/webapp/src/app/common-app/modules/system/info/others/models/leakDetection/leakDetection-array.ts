import { Card } from '../card';
import { LeakDetection } from './leakDetection';

export class LeakDetectionArray extends Card {

    private lds: LeakDetection[];

    constructor() {
        super();
        this.title = [
            {
                title: 'OTHER_BOARDNAME',
                width: '25%'
            },
            {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '25%'
            },
            {
                title: 'OTHER_BOARD_ID',
                width: '25%'
            },
            {
                title: 'OTHER_PART_NUMBER',
                width: '25%'
            }
        ];
        this.lds = [];
    }

    get getLDs(): LeakDetection[] {
        return this.lds;
    }

    addLD(ld: LeakDetection): void {
        this.lds.push(ld);
    }
}
