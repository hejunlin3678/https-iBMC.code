import { FcPort } from './fc-port';
import { FcExtendAttr } from './fc-extend-data';

export class FcVirtualPort extends FcPort {

    constructor(netExtendAttr: FcExtendAttr) {
        super(netExtendAttr);
        this.portName = '--';
        this.fcId = '--';
        this.portType = 'NET_VITURL_PORT';
        this.state = '--';
    }

    set setMoreInfoData(data: object) {
        this.moreInfoData = data;
    }

    set setState(state: string) {
        if (['Up', 'Online', 'LinkUp'].indexOf(state) >= 0) {
            this.state = 'NET_CONNECT';
        } else if (['Down', 'LinkDown'].indexOf(state) >= 0) {
            this.state = 'NET_DISCONNECT';
        } else {
            this.state = state;
        }
    }
}
