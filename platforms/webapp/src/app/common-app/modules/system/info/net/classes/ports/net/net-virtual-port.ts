import { NetPort } from './net-port';
import { NetExtendAttr } from './net-extend-Attr';

export class NetVirtualPort extends NetPort {

    constructor(netExtendAttr: NetExtendAttr) {
        super(netExtendAttr);
        this.portName = '--';
        this.state = '--';
        this.portType = 'NET_VITURL_PORT';
        this.mediaType = '--';
    }

    set setMoreInfoData(data: object) {
        this.moreInfoData = data;
    }

    set setState(state: string) {
        if (state === 'Up') {
            this.state = 'NET_CONNECT';
        } else if (state === 'Down') {
            this.state = 'NET_DISCONNECT';
        }
    }
}
