import { Card } from '../card';
import { ChassisBackplane } from './chassis-back-plane';

export class ChassisBackplaneArray extends Card {

    public chassisBackplanes: ChassisBackplane[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '20%'
            }, {
                title: 'OTHER_POSITION',
                width: '20%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '20%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '20%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '20%'
            }
        ];
        this.chassisBackplanes = [];
    }

    get getChassisBackplanes(): ChassisBackplane[] {
        return this.chassisBackplanes;
    }

    addChassisBackplane(chassisBackplane: ChassisBackplane): void {
        this.chassisBackplanes.push(chassisBackplane);
    }
}
