import { Card } from '../card';
import { LCD } from './lcd';

export class LCDArray extends Card {

    private lcds: LCD[];

    constructor() {
        super();
        this.title = [
            {
                title: 'OTHER_LCD_FIXED_VERSION'
            }
        ];
        this.lcds = [];
    }

    get getLCDs(): LCD[] {
        return this.lcds;
    }

    addLCD(lcd: LCD): void {
        this.lcds.push(lcd);
    }
}
