import { Card } from '../card';
import { CPUBoard } from './cpu-board';

export class CPUBoardArray extends Card {

    private cPUBoards: CPUBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                show: true
            }, {
                title: 'COMMON_MANUFACTURER',
                show: true
            }, {
                title: 'OTHER_SLOT_POSITION',
                show: true
            }, {
                title: 'COMMON_TYPE',
                show: true
            }, {
                title: 'COMMON_ALARM_PCB_VERSION',
                show: true
            }, {
                title: 'COMMIN_CPLD_VERSION',
                show: true
            }, {
                title: 'OTHER_BOARD_ID',
                show: true
            }, {
                title: 'OTHER_POWER',
                show: true
            }
        ];
        this.cPUBoards = [];
    }

    get getCPUBoards(): CPUBoard[] {
        return this.cPUBoards;
    }

    addCPUBoard(cpuBoard: CPUBoard): void {
        this.cPUBoards.push(cpuBoard);
    }
}
