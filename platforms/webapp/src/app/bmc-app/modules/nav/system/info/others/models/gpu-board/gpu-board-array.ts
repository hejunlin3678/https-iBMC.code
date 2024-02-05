import { Card } from '../card';
import { GPUBoard } from './gpu-board';

export class GPUBoardArray extends Card {
    private gPUBoards: GPUBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'OTHER_BOARDNAME',
                width: '20%'
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '15%'
            }, {
                title: 'OTHER_BOARD_ID',
                width: '15%'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '15%'
            }, {
                title: 'COMMIN_CPLD_VERSION',
                width: '35%'
            }
        ];
        this.gPUBoards = [];
    }

    get getGPUBoards(): GPUBoard[] {
        return this.gPUBoards;
    }

    addGPUBoard(gPUBoard: GPUBoard) {
        this.gPUBoards.push(gPUBoard);
    }
}
