import { NetworkAttr, ConnectView, DCBAttr, StatisticsAttr, OpticalInfo } from '../extend-attr';
import { ExtendAttr } from '../port-attr';

export class NetExtendAttr extends ExtendAttr {
    public init: boolean = false;
    public networkAttr = null;
    public connectView = null;
    public dCBAttr = null;
    public statisticsAttr = null;
    public opticalInfo = null;
    constructor(
    ) {
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

    set connect(value: ConnectView) {
        this.connectView = ['NET_CONNECTION_VIEW', value];
    }
    get connect() {
        return this.connectView;
    }

    set dCB(value: DCBAttr) {
        this.dCBAttr = ['NET_DCB_INFO', value];
    }
    get dCB() {
        return this.dCBAttr;
    }

    set optical(value: OpticalInfo) {
        this.opticalInfo = [value.attr[0].name, value];
    }
    get optical() {
        return this.opticalInfo;
    }

    set statistics(value: StatisticsAttr) {
        this.statisticsAttr = ['NET_STATIC_INFO', value];
    }
    get statistics() {
        return this.statisticsAttr;
    }
}
