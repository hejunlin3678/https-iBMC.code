import { Card } from '../card';
import { PowerBoard } from './power-board';

export class PowerBoardArray extends Card {

    private powerBoards: PowerBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME'
            }, {
                title: 'COMMON_TYPE'
            }, {
                title: 'COMMON_ALARM_PCB_VERSION'
            }, {
                title: 'OTHER_BOARD_ID'
            }
        ];
        this.powerBoards = [];
    }

    get getPowerBoards(): PowerBoard[] {
        return this.powerBoards;
    }

    addPowerBoard(powerBoard: PowerBoard): void {
        this.powerBoards.push(powerBoard);
    }

}
