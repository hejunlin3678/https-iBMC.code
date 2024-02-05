import { NetPort } from './net-port';
import { NetExtendAttr } from './net-extend-Attr';

export class NetPhysicalPort extends NetPort {

    constructor(netExtendAttr: NetExtendAttr, portName: string, mediaType: string) {
        super(netExtendAttr);
        this.portName = portName;
        this.state = '--';
        this.portType = 'NET_PHYSICAL_PORT';
        this.mediaType = mediaType;
    }

    set setMoreInfoData(data: object) {
        this.moreInfoData = data;
    }

    public setState(state, disable: string): void {
        if (state) {
            if (state === 'Up') {
                this.state = 'NET_CONNECT';
            } else if (state === 'Down') {
                this.state = 'NET_DISCONNECT';
            } else {
                this.state = state;
            }
        }
        if (state && disable === 'Disabled') {
            this.state = 'NET_DISABLE';
        }
    }

}
