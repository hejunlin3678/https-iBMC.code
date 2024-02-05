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
                width: '10%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '10%'
            }, {
                title: 'COMMON_NUMBER',
                width: '10%'
            }, {
                title: 'COMMON_TYPE',
                width: '10%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '10%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '10%'
            }, {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '15%'
            }
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
