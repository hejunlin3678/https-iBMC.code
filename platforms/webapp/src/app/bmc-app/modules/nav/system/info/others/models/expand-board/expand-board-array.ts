import { ExpandBoard } from './expand-board';
import { Card } from '../card';

export class ExpandBoardArray extends Card {

    private expandBoard: ExpandBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '10%'
            },
            {
                title: 'OTHER_BOARD_ID',
                width: '5%'
            },
            {
                title: 'COMMIN_CPLD_VERSION',
                width: '8%'
            },
            {
                title: 'COMMON_NUMBER',
                width: '8%'
            },
            {
                title: 'COMMON_DESCRIPTION',
                width: '10%'
            },
            {
                title: 'OTHER_POSITION',
                width: '8%'
            },
            {
                title: 'FDM_ALARM_VENDOR',
                width: '12%'
            },
            {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '10%'
            },
            {
                title: 'OTHER_PART_NUMBER',
                width: '10%'
            },
            {
                title: 'COMMON_SERIALNUMBER',
                width: '10%'
            }
        ];

        this.expandBoard = [];
    }

    get getExpandBoard(): ExpandBoard[] {
        return this.expandBoard;
    }

    addExpandBoard(expandBoard: ExpandBoard) {
        this.expandBoard.push(expandBoard);
    }
}
