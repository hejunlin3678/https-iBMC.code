import { Card } from '../card';
import { MemoryBoard } from './memory-board';

export class MemoryBoardArray extends Card {

    private memoryBoards: MemoryBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '15%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '25%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '15%'
            }, {
                title: 'COMMON_TYPE',
                width: '15%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '15%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '15%'
            }
        ];
        this.memoryBoards = [];
    }

    get getMemoryBoards(): MemoryBoard[] {
        return this.memoryBoards;
    }

    addMemoryBoard(memoryBoard: MemoryBoard) {
        this.memoryBoards.push(memoryBoard);
    }
}
