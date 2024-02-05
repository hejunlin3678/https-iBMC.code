import { Card } from '../card';
import { ECUBoard } from './ecu-board';

export class ECUBoardArray extends Card {

    private ecuBoards: ECUBoard[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '33%'
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '33%'
            }, {
                title: 'COMMON_SERIALNUMBER',
                width: '34%'
            }
        ];
        this.ecuBoards = [];
    }

    get getECUBoards(): ECUBoard[] {
        return this.ecuBoards;
    }

    addECUBoard(ecuBoards: ECUBoard): void {
        this.ecuBoards.push(ecuBoards);
    }
}
