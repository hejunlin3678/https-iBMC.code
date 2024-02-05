import { Injectable } from '@angular/core';
import {
    PCIeCardArray,
    PCIeCard,
    NetCard,
    GPUCard,
    FPGACard,
    StorageCard,
    SDICard,
    AccelerateCard,
    SDIExtendCard,
    Ports,
    IPort,
    DpuCard
} from '../../models';
import { TranslateService } from '@ngx-translate/core';
import { OthersService } from '../../others.service';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class PcieCardService {
    constructor(private othersService: OthersService, private http: HttpService, private i18n: TranslateService) {}

    public sendNmiData(slotId: number) {
        return this.http.post('/UI/Rest/System/PCIeDevices/SetSDICardNMI', { SlotId: slotId });
    }

    factory(activeCard: CardUrlArray): Observable<PCIeCardArray> {
        const pCIeCardArray = new PCIeCardArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(
                    this.othersService.getData(url).pipe(map((bodyDatas) => {
                        bodyDatas.forEach((bodyData) => {
                            const description = isOtherEmptyProperty(bodyData.Description) || ' ';
                            const position = isOtherEmptyProperty(bodyData.Position) || ' ';
                            const id = bodyData.Id == null ? '--' : bodyData.Id.substring(8);
                            const manufacturer = isOtherEmptyProperty(bodyData.Manufacturer) || ' ';
                            const vendorId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.VendorId) || ' ';
                            const deviceId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.DeviceId) || ' ';
                            const partNumber = isOtherEmptyProperty(bodyData.PartNumber) || ' ';
                            const subsystemVendorId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.SubsystemVendorId) || ' ';
                            const subsystemId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.SubsystemId) || ' ';
                            const associatedResource = isOtherEmptyProperty(bodyData.PCIeFunctionDate.AssociatedResource) || ' ';
                            const pCIeCard = new PCIeCard(
                                description,
                                position,
                                manufacturer,
                                id,
                                vendorId,
                                deviceId,
                                partNumber,
                                subsystemVendorId,
                                subsystemId,
                                associatedResource
                            );
                            pCIeCardArray.addPCIeCards(pCIeCard);
                            const type = bodyData.FunctionType;
                            switch (type) {
                                case 'Net Card':
                                    pCIeCard.setExtendAttr = this.initNetRAIDCard(bodyData);
                                    break;
                                case 'RAID Card':
                                    pCIeCard.setExtendAttr = this.initNetRAIDCard(bodyData);
                                    break;
                                case 'Storage DPU Card':
                                    pCIeCard.setExtendAttr = this.initDPUCard(bodyData);
                                    break;
                                case 'GPU Card':
                                    pCIeCard.setExtendAttr = this.initGPUCard(bodyData);
                                    break;
                                case 'FPGA Card':
                                case 'NPU Card':
                                    pCIeCard.setExtendAttr = this.initFPGACard(bodyData);
                                    break;
                                case 'SDI Card':
                                    pCIeCard.setExtendAttr = this.initSDICard(bodyData);
                                    break;
                                case 'Storage Card':
                                    pCIeCard.setExtendAttr = this.initStorageCard(bodyData);
                                    break;
                                case 'Accelerate Card':
                                    pCIeCard.setExtendAttr = this.initAccelerateCard(bodyData);
                                    break;
                                default:
                                    pCIeCard.setExtendAttr = this.initNetRAIDCard(bodyData);
                                    break;
                            }
                        });
                        })
                    )
                );
            });
        }
        return forkJoin(post$).pipe(map(() => pCIeCardArray));
    }

    initNetRAIDCard(bodyData: any): NetCard {
        const productName = isOtherEmptyProperty(bodyData.ExtendInfo.ProductName) || ' ';
        const boardId = isOtherEmptyProperty(bodyData.ExtendInfo.BoardId) || ' ';
        const pcbVersion = isOtherEmptyProperty(bodyData.ExtendInfo.PCBVersion) || ' ';
        const cpldVersion = isOtherEmptyProperty(bodyData.ExtendInfo.CPLDVersion) || ' ';
        const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion) || ' ';
        const linkWidth = isOtherEmptyProperty(bodyData.PCIeFunctionDate.LinkWidth) || ' ';
        const linkWidthAbility = isOtherEmptyProperty(bodyData.PCIeFunctionDate.LinkWidthAbility) || ' ';
        const linkSpeed = isOtherEmptyProperty(bodyData.PCIeFunctionDate.LinkSpeed) || ' ';
        const linkSpeedAbility = isOtherEmptyProperty(bodyData.PCIeFunctionDate.LinkSpeedAbility) || ' ';
        const subsystemVendorId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.SubsystemVendorId) || ' ';
        const subsystemId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.SubsystemId) || ' ';

        return new NetCard(
            productName,
            boardId,
            pcbVersion,
            cpldVersion,
            firmwareVersion,
            linkWidth,
            linkWidthAbility,
            linkSpeed,
            linkSpeedAbility,
            subsystemVendorId,
            subsystemId
        );
    }

    initDPUCard(bodyData: any): DpuCard {
        const productName = isOtherEmptyProperty(bodyData.ExtendInfo.ProductName) || ' ';
        const storageManagementIP = isOtherEmptyProperty(bodyData.ExtendInfo.StorageManagementIP) || ' ';
        const storageManagementIPv6 = isOtherEmptyProperty(bodyData.ExtendInfo.StorageManagementIPv6) || ' ';
        const linkWidthAbility = isOtherEmptyProperty(bodyData.PCIeFunctionDate.LinkWidthAbility) || ' ';
        const subsystemVendorId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.SubsystemVendorId) || ' ';
        const subsystemId = isOtherEmptyProperty(bodyData.PCIeFunctionDate.SubsystemId) || ' ';
        const pcbVersion = isOtherEmptyProperty(bodyData.ExtendInfo.PCBVersion) || ' ';
        const portStatus = isOtherEmptyProperty(bodyData.ExtendInfo.StorageManagementPortStatus) || ' ';
        const vrdInfo = isOtherEmptyProperty(bodyData.ExtendInfo.VrdInfo) || null;

        return new DpuCard(
            productName,
            storageManagementIP,
            storageManagementIPv6,
            linkWidthAbility,
            subsystemVendorId,
            subsystemId,
            pcbVersion,
            portStatus,
            vrdInfo
        );
    }

    initGPUCard(bodyData: any): GPUCard {
        const productName = isOtherEmptyProperty(bodyData.ExtendInfo?.ProductName);
        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
        const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion) || ' ';
        return new GPUCard(productName, serialNumber, firmwareVersion);
    }

    initFPGACard(bodyData: any): FPGACard {
        const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion) || ' ';
        let id = null;
        if (firmwareVersion === 0 || firmwareVersion) {
            const firmwareVersions = firmwareVersion.split(' ');
            if (firmwareVersions.length > 1) {
                // 版本 与ID
                id = Number(firmwareVersions[1].substring(1, firmwareVersions[1].length - 1));
            }
        }

        const power = this.getPower(bodyData.Power);
        const productName = isOtherEmptyProperty(bodyData.ExtendInfo.ProductName) || ' ';
        const boardId = isOtherEmptyProperty(bodyData.ExtendInfo.BoardId) || ' ';
        const pcbVersion = isOtherEmptyProperty(bodyData.ExtendInfo.PCBVersion) || ' ';
        const mcuFirmwareVersion = isOtherEmptyProperty(bodyData.McuFirmwareVersion) || ' ';
        const memoryCapacity = isOtherEmptyProperty(bodyData.MemoryCapacityMB) || ' ';
        const serialNumber = isOtherEmptyProperty(bodyData.SerialNumber) || ' ';
        return new FPGACard(
            productName,
            boardId,
            pcbVersion,
            mcuFirmwareVersion,
            id,
            power,
            memoryCapacity,
            serialNumber,
            firmwareVersion
        );
    }

    initSDICard(bodyData: any): SDICard {
        const productName = isOtherEmptyProperty(bodyData.ExtendInfo.ProductName) || ' ';
        const boardId = isOtherEmptyProperty(bodyData.ExtendInfo.BoardId) || ' ';
        const pcbVersion = isOtherEmptyProperty(bodyData.ExtendInfo.PCBVersion) || ' ';
        const storageManageIP = isOtherEmptyProperty(bodyData.ExtendInfo.StorageManagementIP) || ' ';
        const vlanId = isOtherEmptyProperty(bodyData.ExtendInfo.StorageManagementVlanId);
        const bootOption = isOtherEmptyProperty(bodyData.ExtendInfo.BootOption) || ' ';
        const extendCard = bodyData.ExtendInfo.ExtendCardInfo;
        const ports = getPorts(bodyData.ExtendInfo.SDIPfMacInfo);
        const mcuFirmwareVersion = isOtherEmptyProperty(bodyData.McuFirmwareVersion) || ' ';
        const power = bodyData.Power === null || bodyData.Power === undefined ? null : this.getPower(bodyData.Power);
        const sDICard = new SDICard(
            productName,
            boardId,
            pcbVersion,
            storageManageIP,
            vlanId,
            bootOption,
            mcuFirmwareVersion,
            power,
            ports
        );

        let extendCardNum = null;
        const m2Device1Presence: [string, string] = [null, null];
        m2Device1Presence[1] = (bodyData.ExtendInfo.M2Device1Presence ? 'Presence' : 'Absence')
            + '/' + (bodyData.ExtendInfo.M2Device2Presence ? 'Presence' : 'Absence');
        if (boardId === '0x005e') {
            m2Device1Presence[0] = this.i18n.instant('OTHER_M2_INPOSITION_0_1');
            if (extendCard != null && extendCard.length > 0) {
                extendCardNum = extendCard.length + '/2';
            } else {
                extendCardNum = '0/2';
            }
        } else {
            m2Device1Presence[0] = this.i18n.instant('OTHER_M2_INPOSITION_0_0');
            if (boardId === '0x00fd' || bodyData?.Description === 'SDI V5.1' ||
                bodyData?.Description === 'SDI V5.0') {
                m2Device1Presence[0] = this.i18n.instant('OTHER_M2_INPOSITION_1_2');
            }
            if (
                bodyData.ExtendInfo?.SODIMM00Presence !== undefined &&
                bodyData.ExtendInfo?.SODIMM10Presence !== undefined
            ) {
                const sODimm =
                    (bodyData.ExtendInfo.SODIMM00Presence ? 'Presence' : 'Absence') +
                    '/' +
                    (bodyData.ExtendInfo.SODIMM10Presence ? 'Presence' : 'Absence');
                sDICard.setSODimm = sODimm;
            }
            if (extendCard != null && extendCard.length > 0) {
                extendCardNum = extendCard.length + '/2';
            } else {
                extendCardNum = '0/1';
            }
            if (extendCard != null && extendCard.length > 0) {
                extendCardNum = extendCard.length + '/2';
            } else {
                extendCardNum = '0/1';
            }
        }
        sDICard.setM2Device1Presence = m2Device1Presence;
        sDICard.setExtendCardNum = extendCardNum;
        if (extendCard != null && extendCard.length > 0) {
            extendCard.forEach((extendCardInfo) => {
                if (extendCardInfo !== null && extendCardInfo !== []) {
                    const title = this.i18n.instant('OTHER_EXTEND_CARD_INFO')
                        + '(' + this.i18n.instant('OTHER_SLOT_POSITION')
                        + extendCardInfo.Slot + ')';
                    const extProductName = isOtherEmptyProperty(extendCardInfo.ProductName) || ' ';
                    const extPcbVersion = isOtherEmptyProperty(extendCardInfo.PCBVersion) || ' ';
                    const boardName = isOtherEmptyProperty(extendCardInfo.BoardName) || ' ';
                    const extBoardId = isOtherEmptyProperty(extendCardInfo.BoardId) || ' ';
                    const deviceId = isOtherEmptyProperty(extendCardInfo.DeviceId) || ' ';
                    const vendorId = isOtherEmptyProperty(extendCardInfo.VendorId) || ' ';
                    const subsystemVendorId = isOtherEmptyProperty(extendCardInfo.SubsystemId) || ' ';
                    const subsystemId = isOtherEmptyProperty(extendCardInfo.SubsystemVendorId) || ' ';
                    const manufacturer = isOtherEmptyProperty(extendCardInfo.Manufacturer) || ' ';
                    const description = isOtherEmptyProperty(extendCardInfo.Description) || ' ';
                    const chipManufacturer = isOtherEmptyProperty(extendCardInfo.ChipManufacturer) || ' ';
                    const chipModel = isOtherEmptyProperty(extendCardInfo.Model) || ' ';
                    const extendPorts = getPorts(extendCardInfo.PfMacInfo);
                    const sDIExtendCard = new SDIExtendCard(
                        title,
                        extProductName,
                        extPcbVersion,
                        boardName,
                        extBoardId,
                        deviceId,
                        vendorId,
                        subsystemVendorId,
                        subsystemId,
                        manufacturer,
                        description,
                        chipManufacturer,
                        chipModel,
                        extendPorts
                    );
                    sDICard.addExtendCards(sDIExtendCard);
                }
            });
        }
        return sDICard;
    }

    initStorageCard(bodyData: any): StorageCard {
        const productName = bodyData.ExtendInfo.ProductName;
        const boardId = bodyData.BoardID;
        const pcbVersion = bodyData.PCBVersion;
        const serialNumber = bodyData.SerialNumber;
        const storageCard = new StorageCard(productName, boardId, pcbVersion, serialNumber);
        if (bodyData.SSD1Presence === undefined) {
            const model = bodyData.Model;
            const firmwareVersion = bodyData.FirmwareVersion;
            const remainingWearRatePercent = bodyData.ExtendInfo.RemainingWearRatePercent;
            storageCard.setModel = model;
            storageCard.setFirmwareVersion = firmwareVersion;
            storageCard.setResidualWearRate = remainingWearRatePercent;
        } else {
            const prodPCBVersionuctName = bodyData.ProdPCBVersionuctName;
            const ssd1 = bodyData.SSD1Presence ? 'Presence' : 'Absence';
            const ssd2 = bodyData.SSD2Presence ? 'Presence' : 'Absence';
            storageCard.setPcbVersionuctName = prodPCBVersionuctName;
            storageCard.setSsd1Presence = ssd1;
            storageCard.setSsd2Presence = ssd2;
        }
        return storageCard;
    }

    initAccelerateCard(bodyData: any): AccelerateCard {
        const productName = bodyData.ExtendInfo.ProductName;
        const boardId = bodyData.ExtendInfo.BoardID;
        const pcbVersion = bodyData.ExtendInfo.PCBVersion;
        const serialNumber = bodyData.SerialNumber;
        const firmwareVersion = bodyData.FirmwareVersion;
        const bomId = bodyData.BomID;
        return new AccelerateCard(productName, boardId, pcbVersion, serialNumber, firmwareVersion, bomId);
    }

    private getPower(powerNum: number) {
        // 判断是否存在，且是否为小数，然后保留两位小数
        const ifPower = powerNum && powerNum?.toString()?.split('.');
        let power = '';
        if (ifPower && ifPower[1]) {
            if (ifPower[1].length > 2) {
                power = isOtherEmptyProperty(powerNum?.toFixed(2)) || '--';
            } else {
                power = isOtherEmptyProperty(powerNum) || '--';
            }
        } else {
            power = isOtherEmptyProperty(powerNum);
        }
        return power;
    }
}

function getPorts(extendPorts: any): Ports {
    const extendPortArray: IPort[] = [];
    if (!extendPorts || extendPorts.length < 1) {
        return null;
    }
    extendPorts.forEach(port => {
        extendPortArray.push({
            Port: 'Port' + port.Port,
            PfId: 'PF' + port.PfId,
            PermanentMac: port.PermanentMac
        });
    });
    return new Ports(extendPortArray);
}
