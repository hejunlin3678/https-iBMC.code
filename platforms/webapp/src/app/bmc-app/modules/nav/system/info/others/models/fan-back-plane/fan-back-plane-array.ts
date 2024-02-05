import { Card } from '../card';
import { FanBackPlane } from './fan-back-plane';

export class FanBackPlaneArray extends Card {
    private fanBackPlanes: FanBackPlane[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME'
            }, {
                title: 'OTHER_POSITION'
            }, {
                title: 'COMMON_MANUFACTURER'
            }, {
                title: 'COMMON_TYPE'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION'
            }, {
                title: 'OTHER_BOARD_ID'
            }
        ];
        this.fanBackPlanes = [];
    }

    get getFanBackPlanes(): FanBackPlane[] {
        return this.fanBackPlanes;
    }

    addFanBackPlane(fanBackPlane: FanBackPlane): void {
        this.fanBackPlanes.push(fanBackPlane);
    }
}
