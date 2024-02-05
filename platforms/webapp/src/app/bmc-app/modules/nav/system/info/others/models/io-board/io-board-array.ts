import { Card } from '../card';
import { IOBoard } from './io-board';

export class IOBoardArray extends Card {

    private iOBoards: IOBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '10%'
            }, {
                title: 'OTHER_POSITION',
                width: '6%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '10%'
            }, {
                title: 'COMMON_TYPE',
                width: '10%'
            }, {
                title: 'COMMON_FIXED_VERSION',
                width: '10%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '10%'
            }, {
                title: 'COMMIN_CPLD_VERSION',
                width: '10%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '6%'
            }, {
                title: 'OTHER_POWER',
                width: '10%'
            }, {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '12%'
            },
        ];
        this.iOBoards = [];
    }

    get getIOBoards(): IOBoard[] {
        return this.iOBoards;
    }

    addIOBoard(iOBoard: IOBoard): void {
        this.iOBoards.push(iOBoard);
    }
}
