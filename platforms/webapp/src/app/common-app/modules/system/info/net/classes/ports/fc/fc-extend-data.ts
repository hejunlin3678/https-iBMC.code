import { NetworkAttr, OpticalInfo } from '../extend-attr';
import { ExtendAttr } from '../port-attr';

export class FcExtendAttr extends ExtendAttr {
    public init: boolean = false;
    public networkAttr = null;
    public opticalInfo = null;
    constructor() {
        super();
    }

    set setInit(flag: boolean) {
        this.init = flag;
    }
    get setInit() {
        return this.init;
    }
    set network(value: NetworkAttr) {
        this.networkAttr = ['NET_WORK_ATTR', value];
    }
    get network() {
        return this.networkAttr;
    }

    set optical(value: OpticalInfo) {
        this.opticalInfo = [value.attr[0].name, value];
    }
    get optical() {
        return this.opticalInfo;
    }
}
