import { ExtendAttr } from '../extend-attribute';


export class CICExtendAttr extends ExtendAttr {
    // M2 Dist1
    private m2Device1Presence: [string, string];
    // M2 Dist2
    private m2Device2Presence: [string, string];

    constructor(
        m2Device1Presence: number,
        m2Device2Presence: number,
    ) {
        super();
        this.m2Device1Presence = ['M.2 Disk1', m2Device1Presence === 0 ? 'STORE_NO_IN_POSITION' : 'STORE_IN_POSITION'];
        this.m2Device2Presence = ['M.2 Disk2', m2Device2Presence === 0 ? 'STORE_NO_IN_POSITION' : 'STORE_IN_POSITION'];
    }
}
