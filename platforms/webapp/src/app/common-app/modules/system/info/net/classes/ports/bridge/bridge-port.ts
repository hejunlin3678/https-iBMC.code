import { Port } from '../port-attr';
import { BridgeExtendAttr } from './bridge-extend-Attr';

export class BridgePort extends Port {
    netCardName: string;
    netName: string;
    port: string;
    macAddress: string;
    state: string;

    constructor(bridgeExtendAttr: BridgeExtendAttr) {
        super(bridgeExtendAttr);
    }
}
