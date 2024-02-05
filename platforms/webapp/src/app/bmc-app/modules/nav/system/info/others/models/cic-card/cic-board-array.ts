import { CicBoard } from './cic-board';
import { Card } from '../card';

export class CicBoardArray extends Card {

    private expandBoard: CicBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: '',
                width: '5%'
            },
            {
                title: 'OTHER_BOARDNAME',
                width: '15%'
            },
            {
                title: 'COMMON_MANUFACTURER',
                width: '12%'
            },
            {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '8%'
            },
            {
                title: 'OTHER_BOARD_ID',
                width: '8%'
            },
            {
                title: 'COMMON_DESCRIPTION',
                width: '20%'
            },
            {
                title: 'COMMON_SERIALNUMBER',
                width: '20%'
            },
            {
                title: 'OTHER_PART_NUMBER',
                width: '12%'
            },
        ];

        this.expandBoard = [];
    }

    get getExpandBoard(): CicBoard[] {
        return this.expandBoard;
    }

    addExpandBoard(expandBoard: CicBoard) {
        this.expandBoard.push(expandBoard);
    }
}
