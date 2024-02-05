import { Port } from '../port-attr';
import { NetExtendAttr } from './net-extend-Attr';

export abstract class NetPort extends Port {

    portName: string;
    state: string;
    portType: string;
    mediaType: string;

    moreInfoData: object;

    constructor(netExtendAttr: NetExtendAttr) {
        super(netExtendAttr);
    }

    abstract set setMoreInfoData(data: object);

}
