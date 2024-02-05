import { FatherObj } from './fatherObj';

export class PCIECard extends FatherObj {

    private name: [string, string];
    private versionId: [string, string];
    private liveTime: [string, string];
    private deviceId: [string, string];
    private description: [string, string];
    private subSysVersionId: [string, string];
    private version: [string, string];
    private subSysId: [string, string];
    private slot: [string, string];
    private resourceOwner: [string, string];

    constructor(
        name: string,
        versionId: string,
        liveTime: string,
        deviceId: string,
        description: string,
        subSysVersionId: string,
        version: string,
        subSysId: string,
        slot: string,
        resourceOwner: string
    ) {
        super();
        this.name = ['COMMON_NAME', name];
        this.versionId = ['FDM_VENDOR_ID', versionId || '--'];
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.deviceId = ['FDM_DEVICEID', deviceId || '--'];
        this.description = ['COMMON_DESCRIPTION', description];
        this.subSysVersionId = ['OTHER_SON_VENDOR_ID', subSysVersionId || '--'];
        this.version = ['FDM_ALARM_VENDOR', version];
        this.subSysId = ['FDM_SUBSYSTEM_ID', subSysId || '--'];
        this.slot = ['FDM_SLOT', slot];
        this.resourceOwner = ['COMMON_RESOURCE_OWNER', resourceOwner];
    }
}
