import { RAID, Chassis, CPU, DIMM, DiskBP, Disk, Fans, LOM, Mainboard, PCIECard, PSU, RiserCard, CaseItem, DeviceCase } from '../classes';
import { isEmptyProperty } from 'src/app/common-app/utils/common';
import { TranslateService } from '@ngx-translate/core';

interface IMainBoard {
    liveTime: string;
    ibmcVersion: string;
    biosVersion: string;
    mainBoot: string;
    backupUboot: string;
}

export class FDMFactory {
    public static readonly isEmpty: string = '--';
    public static createRaid(data: any, liveTime: string): RAID {
        const productName = isEmptyProperty(data.ProductName) || this.isEmpty;
        const raidLevels = isEmptyProperty(data.SupportedRAIDLevels) || this.isEmpty;
        const location = isEmptyProperty(data.Location) || this.isEmpty;
        const pCBVersion = isEmptyProperty(data.PCBVersion) || this.isEmpty;
        const version = isEmptyProperty(data.Manufacturer) || this.isEmpty;
        const cPLDVersion = isEmptyProperty(data.CPLDVersion) || this.isEmpty;
        const cardNo = isEmptyProperty(data.CardNo) || this.isEmpty;
        const boardId = isEmptyProperty(data.BoardId) || this.isEmpty;
        const description = isEmptyProperty(data.Description) || this.isEmpty;
        const associatedResource = isEmptyProperty(data.AssociatedResource) || this.isEmpty;

        return new RAID(
            productName,
            raidLevels,
            liveTime,
            pCBVersion,
            location,
            cPLDVersion,
            version,
            boardId,
            cardNo,
            associatedResource,
            description
        );
    }

    public static createCPUData(bodyData: any, liveTime: string): CPU {
        const name = isEmptyProperty(bodyData.DeviceName) || this.isEmpty;
        const manufacturer = isEmptyProperty(bodyData.Manufacturer) || this.isEmpty;
        const model = isEmptyProperty(bodyData.Model) || this.isEmpty;
        const processorId = isEmptyProperty(bodyData.IdentificationRegisters) || this.isEmpty;
        const otherParameters = isEmptyProperty(bodyData.OtherParameters) || this.isEmpty;
        const frequencyMHz = isEmptyProperty(bodyData.FrequencyMHz) ? isEmptyProperty(bodyData.FrequencyMHz) + ' MHz' : this.isEmpty;
        const state = isEmptyProperty(bodyData.State) || this.isEmpty;
        const totalCores = (isEmptyProperty(bodyData.TotalCores) || '--') + '/' + (isEmptyProperty(bodyData.TotalThreads) || '--');
        const cacheKiB = (isEmptyProperty(bodyData.L1CacheKiB) || '--')
            + '/'
            + (isEmptyProperty(bodyData.L2CacheKiB) || '--')
            + '/'
            + (isEmptyProperty(bodyData.L3CacheKiB) || '--')
            + 'KB';
        const partNumber = isEmptyProperty(bodyData.PartNumber) || this.isEmpty;
        const serialNumber = isEmptyProperty(bodyData.SerialNumber) || this.isEmpty;
        return new CPU(
            name,
            frequencyMHz,
            liveTime,
            state,
            manufacturer,
            totalCores,
            model,
            cacheKiB,
            processorId,
            partNumber,
            otherParameters,
            serialNumber
        );
    }

    public static createMainboard(data: any, params: IMainBoard): Mainboard {
        const liveTime = isEmptyProperty(params) || this.isEmpty;
        const ibmcVersion = isEmptyProperty(data.PMEVersion) || this.isEmpty;
        const biosVersion = isEmptyProperty(data.BiosVersion) || this.isEmpty;
        const cPLDVersion = isEmptyProperty(data.CPLDVersion) || this.isEmpty;
        const mainBoot = isEmptyProperty(data.UbootVersion) || this.isEmpty;
        const backupUboot = isEmptyProperty(data.UbootBackupVer) || this.isEmpty;
        const partNumber = isEmptyProperty(data.PartNumber) || this.isEmpty;
        const boardId = isEmptyProperty(data.BoardId) || this.isEmpty;
        const manufacturer = isEmptyProperty(data.Manufacturer) || this.isEmpty;
        const pCBVersion = isEmptyProperty(data.PCBVersion) || this.isEmpty;
        const pchModel = isEmptyProperty(data.PchModel) || this.isEmpty;
        const deviceType = isEmptyProperty(data.BoardModel) || this.isEmpty;
        const serialNumber = isEmptyProperty(data.SerialNumber) || this.isEmpty;
        return new Mainboard(
            liveTime,
            boardId,
            ibmcVersion,
            manufacturer,
            biosVersion,
            pCBVersion,
            cPLDVersion,
            pchModel,
            mainBoot,
            deviceType,
            backupUboot,
            serialNumber,
            partNumber
        );
    }

    public static createPSU(data: any, liveTime: string, deviceInfoUrl: string): PSU {
        const manufacturer = isEmptyProperty(data.Manufacturer) || this.isEmpty;
        const model = isEmptyProperty(data.Model) || this.isEmpty;
        const serialNumber = isEmptyProperty(data.SerialNumber) || this.isEmpty;
        const firmwareVersion = isEmptyProperty(data.FirmwareVersion) || this.isEmpty;
        const powerCapacityWatts = isEmptyProperty(data.PowerCapacityWatts) ? data.PowerCapacityWatts + ' W' : this.isEmpty;
        const powerSupplyType = isEmptyProperty(data.PowerSupplyType) || this.isEmpty;
        const partNumber = isEmptyProperty(data.PartNumber) || this.isEmpty;
        const slot = isEmptyProperty(data.MemberId + 1) || this.isEmpty;
        return new PSU(
            liveTime,
            firmwareVersion,
            slot,
            powerCapacityWatts,
            manufacturer,
            powerSupplyType,
            model,
            partNumber,
            serialNumber
        );
    }

    public static createDIMM(bodyData: any, liveTime: string): DIMM {
        const name = isEmptyProperty(bodyData.DeviceName) || this.isEmpty;
        const manufacturer = isEmptyProperty(bodyData.Manufacturer) || this.isEmpty;
        const location = isEmptyProperty(bodyData.Position) || this.isEmpty;
        const capacity = isEmptyProperty(bodyData.CapacityMiB) ? bodyData.CapacityMiB + ' MB' : this.isEmpty;
        const speed = isEmptyProperty(bodyData.OperatingSpeedMhz) ? bodyData.OperatingSpeedMhz + ' MT/S' : this.isEmpty;
        const rankCount = isEmptyProperty(bodyData.RankCount) || this.isEmpty;
        const serialNumber = isEmptyProperty(bodyData.SerialNumber) || this.isEmpty;
        const deviceType = isEmptyProperty(bodyData.MemoryDeviceType) || this.isEmpty;
        const voltage = isEmptyProperty(bodyData.MinVoltageMillivolt) ? bodyData.MinVoltageMillivolt + ' mV' : this.isEmpty;
        const partNumber = isEmptyProperty(bodyData.PartNumber) || this.isEmpty;
        const widthBits = isEmptyProperty(bodyData.DataWidthBits) ? bodyData.DataWidthBits + ' bit' : this.isEmpty;
        const technology = isEmptyProperty(bodyData.Technology) || this.isEmpty;

        return new DIMM(
            name,
            serialNumber,
            liveTime,
            deviceType,
            manufacturer,
            voltage,
            location,
            partNumber,
            capacity,
            widthBits,
            speed,
            technology,
            rankCount
        );
    }

    public static createFans(bodyData, liveTime, i18n: TranslateService): Fans {

        let nameFan = '';
        if (bodyData.Speed.indexOf('/') > 0) {
            nameFan = i18n.instant('FDM_FAN_NAME1') + bodyData.Name.substr(3, bodyData.Name.length - 1)
                + ' ' + i18n.instant('FAN_LOCATION_NAME');
        } else {
            nameFan = i18n.instant('FDM_FAN_NAME1') + bodyData.Name.substr(3, bodyData.Name.length - 1);
        }
        const name = nameFan;
        const partNumber = isEmptyProperty(bodyData.PartNumber) || this.isEmpty;
        const reading = isEmptyProperty(bodyData.Speed) || this.isEmpty;
        const model = isEmptyProperty(bodyData.Model) || this.isEmpty;
        const speedRatio = isEmptyProperty(bodyData.SpeedRatio) || this.isEmpty;
        const single = isEmptyProperty(bodyData.single) || this.isEmpty;
        const onlineTime = liveTime;
        return new Fans(
            name,
            partNumber,
            onlineTime,
            reading,
            model,
            speedRatio
        );
    }

    public static createLom(bodyData: any, liveTime: string): LOM {
        const name = isEmptyProperty(bodyData.Name) || this.isEmpty;
        const cardModel = isEmptyProperty(bodyData.CardModel) || this.isEmpty;
        const boardId = isEmptyProperty(bodyData.BoardId) || this.isEmpty;
        const model = isEmptyProperty(bodyData.Model) || this.isEmpty;
        const cardManufacturer = isEmptyProperty(bodyData.CardManufacturer) || this.isEmpty;
        const pCBVersion = isEmptyProperty(bodyData.PCBVersion) || this.isEmpty;
        const manufacturer = isEmptyProperty(bodyData.Manufacturer) || this.isEmpty;

        return new LOM(
            name,
            model,
            liveTime,
            cardManufacturer,
            cardModel,
            pCBVersion,
            boardId,
            manufacturer
        );
    }

    public static createRiserCard(bodyData: any, liveTime: string): RiserCard {
        const name = isEmptyProperty(bodyData.Name) || this.isEmpty;
        const manufacturer = isEmptyProperty(bodyData.Manufacturer) || this.isEmpty;
        const cardNo = isEmptyProperty(bodyData.Slot) || this.isEmpty;
        const description = isEmptyProperty(bodyData.Description) || this.isEmpty;
        const pCBVersion = isEmptyProperty(bodyData.PCBVersion) || this.isEmpty;
        const boardId = isEmptyProperty(bodyData.BoardId) || this.isEmpty;
        return new RiserCard(
            name,
            description,
            liveTime,
            pCBVersion,
            manufacturer,
            boardId,
            cardNo
        );
    }

    public static createDiskBP(bodyData: any, liveTime: string): DiskBP {
        const location = isEmptyProperty(bodyData.Location) || this.isEmpty;
        const manufacturer = isEmptyProperty(bodyData.Manufacturer) || this.isEmpty;
        const cardNo = isEmptyProperty(bodyData.CardNo) || this.isEmpty;
        const description = isEmptyProperty(bodyData.Description) || this.isEmpty;
        const pCBVersion = isEmptyProperty(bodyData.PCBVersion) || this.isEmpty;
        const cPLDVersion = isEmptyProperty(bodyData.CPLDVersion) || this.isEmpty;
        const boardId = isEmptyProperty(bodyData.BoardId) || this.isEmpty;

        return new DiskBP(
            liveTime,
            description,
            location,
            pCBVersion,
            manufacturer,
            cPLDVersion,
            cardNo,
            boardId
        );
    }

    public static createDisk(bodyData: any, liveTime: string, i18n: TranslateService): Disk {
        const rebuildState = isEmptyProperty(bodyData.RebuildState);

        let tmpRebuildState = '';
        if (rebuildState === 'DoneOrNotRebuilt') {
            tmpRebuildState = i18n.instant('FDM_STOPPED');
        } else if (rebuildState === 'Rebuilding') {
            const rebuildProgress = isEmptyProperty(bodyData.RebuildProgress) ? bodyData.RebuildProgress : '';
            tmpRebuildState = i18n.instant('FDM_RECONSTRUCTION') + '  ' + rebuildProgress;
        }

        // 定位状态
        let tmpIndicatorLED = '';
        if (bodyData.indicatorLED === 'Off') {
            tmpIndicatorLED = i18n.instant('FDM_DISABLED');
        } else if (bodyData.indicatorLED === 'Blinking') {
            tmpIndicatorLED = i18n.instant('FDM_ENABLED');
        } else if (!bodyData.indicatorLED) {
            tmpIndicatorLED = i18n.instant('FDM_DISABLED');
        }

        // 容量
        let capacity = null;
        if (bodyData.CapacityBytes === 0 || bodyData.CapacityBytes) {
            capacity = bodyData.CapacityBytes / 1024 / 1024 / 1024 / 1024;
            capacity = capacity.toFixed(3) + ' TB';
        }

        // 巡检状态
        const patrolState = bodyData.PatrolState;
        let tmpPatrolState = '';
        if (patrolState === 'DoneOrNotPatrolled') {
            tmpPatrolState = i18n.instant('FDM_STOPPED');
        } else if (patrolState === 'Patrolling') {
            tmpPatrolState = i18n.instant('FDM_CHECKING');
        }
        return new Disk(
            liveTime,
            isEmptyProperty(bodyData.Model) || this.isEmpty,
            isEmptyProperty(bodyData.Protocol) || this.isEmpty,
            isEmptyProperty(bodyData.Revision) || this.isEmpty,
            isEmptyProperty(bodyData.Manufacturer) || this.isEmpty,
            isEmptyProperty(bodyData.TemperatureCelsius, ' ℃') || this.isEmpty,
            isEmptyProperty(bodyData.SerialNumber) || this.isEmpty,
            isEmptyProperty(bodyData.SASAddress[0]) || this.isEmpty,
            isEmptyProperty(bodyData.MediaType) || this.isEmpty,
            capacity || this.isEmpty,
            isEmptyProperty(bodyData.SASAddress[1]) || this.isEmpty,
            isEmptyProperty(bodyData.NegotiatedSpeedGbs, ' Gbps') || this.isEmpty,
            isEmptyProperty(bodyData.CapableSpeedGbs, ' Gbps') || this.isEmpty,
            tmpPatrolState || this.isEmpty,
            isEmptyProperty(bodyData.PowerState) || this.isEmpty,
            isEmptyProperty(bodyData.FirmwareStatus) || this.isEmpty,
            isEmptyProperty(bodyData.HotspareType) || this.isEmpty,
            isEmptyProperty(bodyData.HoursOfPoweredUp, ' h') || this.isEmpty,
            tmpIndicatorLED || this.isEmpty,
            tmpRebuildState || this.isEmpty,
            isEmptyProperty(bodyData.PredictedMediaLifeLeftPercent, '%') || this.isEmpty,
            isEmptyProperty(bodyData.PartNum) || this.isEmpty,
            isEmptyProperty(bodyData.AssociatedResource) || this.isEmpty,
        );
    }

    public static createPcieCard(data: any, liveTime: string): PCIECard {
        const slot = isEmptyProperty(data.PcieCardSlot) || this.isEmpty;
        const name = isEmptyProperty(data.ProductName) || this.isEmpty;
        const description = isEmptyProperty(data.Description) || this.isEmpty;
        const manufacturer = isEmptyProperty(data.Manufacturer) || this.isEmpty;
        const vendorId = isEmptyProperty(data.VendorId) || this.isEmpty;
        const deviceId = isEmptyProperty(data.DeviceId) || this.isEmpty;
        const sysVendorId = isEmptyProperty(data.SubsystemVendorId) || this.isEmpty;
        const sysId = isEmptyProperty(data.SubsystemId) || this.isEmpty;
        const resource = isEmptyProperty(data.AssociatedResource) || this.isEmpty;
        return new PCIECard(
            name,
            vendorId,
            liveTime,
            deviceId,
            description,
            sysVendorId,
            manufacturer,
            sysId,
            slot,
            resource,
        );
    }

    public static createChassis(liveTime: string): Chassis {
        return new Chassis(liveTime);
    }

    public static createCaseItem(item): CaseItem {
        const no = isEmptyProperty(item.no) || this.isEmpty;
        const createTime = isEmptyProperty(item.createTime) || this.isEmpty;
        const type = isEmptyProperty(item.type) || this.isEmpty;
        const level = isEmptyProperty(item.level) || this.isEmpty;
        const info = isEmptyProperty(item.info) || this.isEmpty;
        return new CaseItem(
            no,
            type,
            level,
            createTime,
            info
        );
    }
    public static createDeviceCase(i18n: TranslateService, arr) {
        return new DeviceCase(
            i18n,
            arr
        );
    }

}
