import { Card } from '../card';
import { PeripheralFirmware } from './peripheral-firmware';

export class PeripheralFirmwareArray extends Card {

    private peripheralFirmwares: PeripheralFirmware[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME'
            },
            {
                title: 'COMMON_MANUFACTURER'
            },
            {
                title: 'OTHER_VERSION'
            }
        ];
        this.peripheralFirmwares = [];
    }
    get getPeripheralFirmwares(): PeripheralFirmware[] {
        return this.peripheralFirmwares;
    }

    addPeripheralFirmware(peripheralFirmware: PeripheralFirmware): void {
        this.peripheralFirmwares.push(peripheralFirmware);
    }

}
