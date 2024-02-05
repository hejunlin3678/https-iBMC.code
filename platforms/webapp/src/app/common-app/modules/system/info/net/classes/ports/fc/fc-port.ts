import { Port } from '../port-attr';
import { FcExtendAttr } from './fc-extend-data';

export class FcPort extends Port {
    portName: string;
    fcId: string;
    portType: string;
    state: string;

    moreInfoData: object;

    constructor(fcExtendAttr: FcExtendAttr) {
        super(fcExtendAttr);
    }
}
