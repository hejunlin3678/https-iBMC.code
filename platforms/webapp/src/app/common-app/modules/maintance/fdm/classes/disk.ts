import { FatherObj } from './fatherObj';

export class Disk extends FatherObj {

    private liveTime: [string, string];
    private alarmModel: [string, string];
    private interfaceType: [string, string];
    private firmwareVersion: [string, string];
    private alarmVersion: [string, string];
    private temp: [string, string];
    private serialNum: [string, string];
    private sasAddress0: [string, string];
    private mediaType: [string, string];
    private capacity: [string, string];
    private sasAddress1: [string, string];
    private speedgbs: [string, string];
    private supportedRate: [string, string];
    private patrolState: [string, string];
    private powerStatus: [string, string];
    private firmwareStatus: [string, string];
    private hotStatus: [string, string];
    private powerTime: [string, string];
    private locationStatus: [string, string];
    private constructionState: [string, string];
    private wearRate: [string, string];
    private bomNum: [string, string];
    private resourceOwner: [string, string];

    constructor(
        liveTime: string,
        alarmModel: string,
        interfaceType: string,
        firmwareVersion: string,
        alarmVersion: string,
        temp: string,
        serialNum: string,
        sasAddress0: string,
        mediaType: string,
        capacity: string,
        sasAddress1: string,
        speedgbs: string,
        supportedRate: string,
        patrolState: string,
        powerStatus: string,
        firmwareStatus: string,
        hotStatus: string,
        powerTime: string,
        locationStatus: string,
        constructionState: string,
        wearRate: string,
        bomNum: string,
        resourceOwner: string,
    ) {
        super();
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.alarmModel = ['FDM_ALARM_MODEL', alarmModel];
        this.interfaceType = ['FDM_INTERFACE_TYPE', interfaceType];
        this.firmwareVersion = ['COMMON_FIXED_VERSION', firmwareVersion];
        this.alarmVersion = ['FDM_ALARM_VENDOR', alarmVersion];
        this.temp = ['FDM_TEMPERATURE', temp];
        this.serialNum = ['COMMON_SERIALNUMBER', serialNum];
        this.sasAddress0 = ['FDM_SAS_ADDRESS0', sasAddress0];
        this.mediaType = ['FDM_MEDIA_TYPE', mediaType];
        this.capacity = ['COMMON_CAPACITY', capacity];
        this.sasAddress1 = ['FDM_SAS_ADDRESS1', sasAddress1];
        this.speedgbs = ['FDM_NEGOTIATED_SPEEDGBS', speedgbs];
        this.supportedRate = ['FDM_SUPPORTED_RATE', supportedRate];
        this.patrolState = ['FDM_PATROL_STATE', patrolState];
        this.powerStatus = ['FDM_POWER_STATUS', powerStatus];
        this.firmwareStatus = ['FDM_FIRMWARE_STATUS', firmwareStatus];
        this.hotStatus = ['FDM_HOT_STATUS', hotStatus];
        this.powerTime = ['FDM_TOTAL_POWER_TIME', powerTime];
        this.locationStatus = ['FDM_LOCATION_STATUS', locationStatus];
        this.constructionState = ['FDM_CONSTRUCTION_STATE', constructionState];
        this.wearRate = ['WEAR_RATE', wearRate];
        this.bomNum = ['OTHER_PART_NUMBER', bomNum];
        this.resourceOwner = ['COMMON_RESOURCE_OWNER', resourceOwner];
    }

}
