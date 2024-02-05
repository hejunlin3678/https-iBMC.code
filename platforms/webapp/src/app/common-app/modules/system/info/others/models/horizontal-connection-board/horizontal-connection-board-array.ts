import { Card } from '../card';
import { HorizontalAdapter } from './horizontal-connection-board';

export class HorizontalAdapterArr extends Card {

    private horizontalAdapters: HorizontalAdapter[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION'
            }, {
                title: 'OTHER_BOARD_ID'
            }
        ];
        this.horizontalAdapters = [];
    }

    get getHorizontalAdapters(): HorizontalAdapter[] {
        return this.horizontalAdapters;
    }

    addHorizontalAdapter(horizontalAdapter: HorizontalAdapter) {
        this.horizontalAdapters.push(horizontalAdapter);
    }

}
