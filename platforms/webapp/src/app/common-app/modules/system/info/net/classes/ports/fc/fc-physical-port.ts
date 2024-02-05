import { FcPort } from './fc-port';
import { FcExtendAttr } from './fc-extend-data';

export class FcPhysicalPort extends FcPort {

    constructor(netExtendAttr: FcExtendAttr, portName: string, fcId: string, portType: string, linkStatus: string) {
        super(netExtendAttr);
        this.portName = portName;
        this.fcId = fcId;
        this.portType = portType || '--';
        this.state = linkStatus || '--';
    }

    set setMoreInfoData(data: object) {
        this.moreInfoData = data;
    }
}
