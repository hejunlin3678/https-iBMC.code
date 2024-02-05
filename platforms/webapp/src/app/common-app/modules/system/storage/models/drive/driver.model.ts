import { IHealthState, IOptions, ISwitch, IRadio } from 'src/app/common-app/models';
import { StorageStaticData, DriveStaticModel } from '../static-data';
import { IStorageRadio, RaidType } from '../storage-interface';

export class Driver {

    private interType: [string, string];
    private healthState: [string, IHealthState];
    private manufacturer: [string, string];
    private model: [string, string];
    private serialNumber: [string, string];
    private firmwareVersion: [string, string];
    private mediaType: [string, string];
    private temperature: [string, string];
    private firmwareStatus: [string, IOptions];
    private sasAddress0: [string, string];
    private sasAddress1: [string, string];
    private capacity: [string, string];
    private capableSpeed: [string, string];
    private negotiatedSpeed: [string, string];
    private powerState: [string, string];
    private hotspareType: [string, IRadio];
    private construction: [string, string];
    private patrolState: [string, string];
    private indicator: [string, ISwitch];
    private powerSupply: [string, string];
    private wearRate: [string, string];
    private partNum: [string, string];
    private resourceOwner: [string, string];
    private diskSpeed: [string, number];
    private isEPD: boolean;
    private bootEnabled: [string, IStorageRadio];
    private bootPriority: IOptions;

    constructor(
        interType: string,
        healthState: string,
        manufacturer: string,
        model: string,
        serialNumber: string,
        firmwareVersion: string,
        mediaType: string,
        temperature: string,
        firmwareStatus: string,
        sasAddress0: string,
        sasAddress1: string,
        capacity: string,
        capableSpeed: string,
        negotiatedSpeed: string,
        powerState: string,
        hotspareType: string,
        construction: string,
        patrolState: string,
        indicator: string,
        powerSupply: string,
        wearRate: string,
        partNum: string,
        resourceOwner: string,
        isRaid: boolean,
        diskSpeed: number,
        isEPD: boolean,
        raidType: string,
        bootEnabled: boolean,
        bootPriority: string
    ) {
        this.interType = ['FDM_INTERFACE_TYPE', interType];
        const state = StorageStaticData.getState(healthState);
        this.healthState = ['NET_HEALTH_STATUS', state];
        this.manufacturer = ['COMMON_MANUFACTURER', manufacturer];
        this.model = ['FDM_ALARM_MODEL', model];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        this.firmwareVersion = ['STORE_FIRMWARE_VERSION', firmwareVersion];
        this.mediaType = ['FDM_MEDIA_TYPE', mediaType];
        this.temperature = ['FDM_TEMPERATURE', temperature];
        this.firmwareStatus = ['FDM_FIRMWARE_STATUS', DriveStaticModel.getInstance().getFirmwareStatus(firmwareStatus)];
        this.sasAddress0 = ['FDM_SAS_ADDRESS0', sasAddress0];
        this.sasAddress1 = ['FDM_SAS_ADDRESS1', sasAddress1];
        this.capacity = ['COMMON_CAPACITY', capacity];
        this.capableSpeed = ['FDM_SUPPORTED_RATE', capableSpeed];
        this.negotiatedSpeed = ['FDM_NEGOTIATED_SPEEDGBS', negotiatedSpeed];
        this.powerState = ['FDM_POWER_STATUS', powerState];
        this.hotspareType = ['FDM_HOT_STATUS', DriveStaticModel.getInstance().getHotspareType(hotspareType)];
        this.indicator = ['STORE_LOCATION_STATUS', DriveStaticModel.getInstance().getIndicator(indicator)];
        this.powerSupply = ['FDM_TOTAL_POWER_TIME', powerSupply];
        this.wearRate = ['WEAR_RATE', wearRate];
        this.partNum = ['OTHER_PART_NUMBER', partNum];
        this.resourceOwner = ['COMMON_RESOURCE_OWNER', resourceOwner];
        this.diskSpeed = ['FAN_HEAT_ROTATIONAL_SPEED', diskSpeed];
        this.bootEnabled = ['STORE_BOOT_DISK', DriveStaticModel.getIsBootDisk(bootEnabled)];
        this.bootPriority = DriveStaticModel.getInstance().getBootPriority(bootPriority);
        // 只有Raid卡下面才会显示的属性
        if (isRaid) {
            this.construction = ['FDM_CONSTRUCTION_STATE', construction];
            this.patrolState = ['STORE_PATROL_STATE', raidType === RaidType.BRCM || raidType === RaidType.ARIES ?
                StorageStaticData.setPatrolState(patrolState) : null];
        }
        this.isEPD = isEPD;
    }

    get getIndicator(): ISwitch {
        return this.indicator[1];
    }

    get getHotspareType(): IRadio {
        return this.hotspareType[1];
    }

    get getFirmwareStatus(): IOptions {
        return this.firmwareStatus[1];
    }
    get getBootDisk(): IStorageRadio {
        return this.bootEnabled[1];
    }
    get getIsEPD(): boolean {
        return this.isEPD;
    }
    get getBootPriority(): IOptions {
        return this.bootPriority;
    }
    get getMediaType(): string {
        return this.mediaType[1];
    }
    get getPatrolState(): IStorageRadio {
        return DriveStaticModel.getPatrol(StorageStaticData.getPatrolState(this.patrolState[1]));
    }
}
