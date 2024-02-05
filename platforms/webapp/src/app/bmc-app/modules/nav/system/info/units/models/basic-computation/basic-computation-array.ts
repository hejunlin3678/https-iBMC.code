import { Card } from '../../../others/models';
import { BasicComputation } from './basic-computation';


export class BasicComputationArray extends Card {

    private basicComputations: BasicComputation[];

    constructor() {
        super();
        this.title = [
            {
                title: '',
                width: '5%',
                show: false
            }, {
                title: 'COMMON_NAME',
                width: '20%',
                show: true
            }, {
                title: 'OTHER_POSITION',
                width: '20%',
                show: true
            }, {
                title: 'COMMON_MANUFACTURER',
                width: '15%',
                show: true
            }, {
                title: 'OTHER_SLOT_POSITION',
                width: '15%',
                show: true
            }, {
                title: 'COMMON_DESCRIPTION',
                width: '20%',
                show: true
            }, {
                title: 'OTHER_COMPONENT_UID',
                width: '30%',
                show: true
            }
        ];
        this.basicComputations = [];
    }

    get getbasicComputations(): BasicComputation[] {
        return this.basicComputations;
    }

    get getDropdownIconShow(): boolean {
        return this.title[0]['show'];
    }

    set setDropdownIconShow(ishow: boolean) {
        this.title[0]['show'] = ishow;
    }

    addBasicComputation(basicComputation: BasicComputation): void {
        this.basicComputations.push(basicComputation);
    }
}
