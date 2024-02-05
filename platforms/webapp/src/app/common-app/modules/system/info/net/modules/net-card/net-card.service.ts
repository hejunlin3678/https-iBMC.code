import { Injectable } from '@angular/core';
import { map } from 'rxjs/internal/operators/map';
import { NetCard } from '../../classes/net-card/net-card';
import { NetExtendAttr } from '../../classes/ports/net/net-extend-Attr';
import { NetVirtualPort } from '../../classes/ports/net/net-virtual-port';
import { NetPhysicalPort } from '../../classes/ports/net/net-physical-port';
import { NetPort } from '../../classes/ports/net/net-port';
import { mergeMap } from 'rxjs/internal/operators/mergeMap';
import { NetPortArr } from '../../classes/ports/net/net-port-Arr';
import { of } from 'rxjs';
import { ConnectView, NetworkAttr, StatisticsAttr, DCBAttr, OpticalInfo, OpticalAttr, SendAttr, RecvAttr, ThresholdAttr } from '../../classes/ports/extend-attr';
import { HttpService } from 'src/app/common-app/service';
import { NetIPv46 } from '../../classes/ports/net/net-ipv46';
import { IIpv4NetInfo, IIpv6NetInfo } from '../../classes/net.dataType';
import { CableInfo } from '../../classes/ports/extend-attr/cable-info';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT, PRODUCTTYPE } from 'src/app/common-app/service/product.type';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';


@Injectable({
    providedIn: 'root'
})
export class NetCardService {
    constructor(
        private http: HttpService,
        private globalData: GlobalDataService
    ) {}
    factory(netCardInfo, name: string) {
        return of('').pipe(
            map(() => {
                const cardName = netCardInfo.CardName;
                const cardModel = netCardInfo.CardModel || null;
                const manufacturer = netCardInfo.Manufacturer || null;
                let boardId = (netCardInfo.BoardID === 0 || netCardInfo.BoardID) ?
                    '0x0000'.substring(0, 6 - netCardInfo.BoardID.toString(16).length) +
                    netCardInfo.BoardID.toString(16) : null;
                const componentUniqueID = netCardInfo.ComponentUniqueID || null;
                const cardManufacturer = netCardInfo.CardManufacturer || null;
                const pcbVersion = netCardInfo.PCBVersion || null;
                const associatedResource = (this.globalData.productType === PRODUCTTYPE.PANGEA ||
                                        CommonData.productType === PRODUCT.PANGEA) ?
                                        this.getPangeaResource(netCardInfo.AssociatedResource) : (netCardInfo.AssociatedResource || null);
                const pcieSlotId = netCardInfo.PcieSlotId;
                const model = netCardInfo.ChipModel || null;
                const rootBDF = netCardInfo.BusInfo || null;
                const firemwareVersion = netCardInfo.FirmwarePackageVers || null;
                const chipManufacturer = netCardInfo.ChipManufacturer;
                const hotPlugCtrlStatus = netCardInfo.OrderlyHotplugCtrlStatus;
                const hotPlugSupported = netCardInfo.HotPlugSupported;
                const AdapterId = netCardInfo.AdapterId;
                const hotPlugAttention = netCardInfo.HotPlugAttention;
                // 虚拟端口和物理端口合一
                const netPorts = netCardInfo.PortProperties || [];
                if (boardId === '0x00' || boardId === '0xFFFF') {
                    boardId = '--';
                }
                return [new NetCard(
                    name,
                    'net',
                    netCardInfo,
                    true,
                    cardName,
                    cardModel,
                    manufacturer,
                    chipManufacturer,
                    boardId,
                    componentUniqueID,
                    firemwareVersion,
                    cardManufacturer,
                    model,
                    pcbVersion,
                    associatedResource,
                    rootBDF,
                    pcieSlotId,
                    hotPlugSupported,
                    hotPlugCtrlStatus,
                    AdapterId,
                    hotPlugAttention,
                ), netPorts];
            }),
            mergeMap(([netCard, netPorts]) => {
                if (netPorts && netPorts.length === 0) {
                    return of('').pipe(map(() => netCard));
                }
                const results = [];
                netPorts.map((port: any) => {
                    if (port.BasicAttribute.InterfaceType === 'Physical') {
                        results.push(this.getPhysical(port));
                    } else if (port.BasicAttribute.InterfaceType === 'Virtual') {
                        results.push(this.getVirtualPort(port));
                    }
                });
                return of('').pipe(
                    map(() => {
                        const ports = new NetPortArr();
                        results.map((port: NetPort) => {
                            if (port) {
                                ports.addPort(port);
                            }
                        });
                        netCard.setPort = ports;
                        return netCard;
                    })
                );
            })
        );
    }
    // 盘古的资源归属比返回的字符串少1，如返回CPU1页面需显示CPU0
    public getPangeaResource(data: string) {
        let resourceData = '';
        const cpuStr = data.substring(0, 3);
        const indexStr = Number(data.substring(3));
        resourceData = cpuStr + (indexStr - 1);
        return resourceData;
    }
    /**
     * 网口的显示逻辑：
     * 1、Mac地址有效。
     * 2、Ip地址有效。
     * 3、有光模块数据。
     * 4、BDF属性有效。
     * 5、在禁用的场景下，即使Mac地址无效，也依然显示。
     * 6、满足上述任一条件，则显示。
     */
    public getPhysical(portInfo) {
        let physicalPort = null;
        let macIsValid = false;
        const basicAttribute = portInfo.BasicAttribute;
        const networkAttribute = portInfo.NetworkAttribute;
        // 判断是否为IB卡
        macIsValid = this._checkValid(networkAttribute.MAC);
        const ipv4IsValid = this._checkValid2(networkAttribute.IPv4Addresses);
        const ipv6IsValid = this._checkValid2(networkAttribute.IPv6Addresses);
        const isOptical = portInfo.OpticalModule ? true : false;
        const isBDF = Boolean(networkAttribute.BusInfo) && (networkAttribute.BusInfo !== '--');
        const flag = macIsValid || ipv4IsValid || ipv6IsValid || isOptical || isBDF;
        const isDisabled = networkAttribute.State === 'Disabled' ? false : true; // 缺失
        if (flag || (!flag && isDisabled)) {
            const numCheck = /^[0-9]+.?[0-9]*/;
            let portName = 'Port' + basicAttribute.PhysicalPortNumber;
            if (!numCheck.test(basicAttribute.PhysicalPortNumber)) {
                portName = '--';
            }
            const mediaType = basicAttribute.MediaType ? (
                basicAttribute.MediaType === 'Electrical' ? 'NET_ELETRICITY' : 'NET_LIGHTEST'
            ) : '--';
            physicalPort = new NetPhysicalPort(new NetExtendAttr(), portName, mediaType);
            if (networkAttribute.Name === 'System Ethernet Interface') {
                physicalPort.setState(basicAttribute.LinkStatus, 'disabled');
            } else {
                physicalPort.setState(basicAttribute.LinkStatus, isDisabled);
            }
            physicalPort.setMoreInfoData = portInfo;
        }
        return physicalPort;
    }
    /**
     *
     * @param portInfo 虚拟端口信息
     * @returns 虚拟端口信息
     */
    public getVirtualPort(portInfo) {
        let virtualPort = null;
        const basicAttribute = portInfo.BasicAttribute;
        const networkAttribute = portInfo.NetworkAttribute;
        const macIsValid = Boolean(networkAttribute.MAC) &&
            networkAttribute.MAC !== '00:00:00:00:00:00' &&
            networkAttribute.MAC !== 'N/A';
        const ipv4IsValid = Boolean(networkAttribute.IPv4Addresses) &&
            networkAttribute.IPv4Addresses !== 'N/A' &&
            networkAttribute.IPv4Addresses.length > 0;
        const ipv6IsValid =
            Boolean(networkAttribute.IPv6Addresses) &&
            networkAttribute.IPv6Addresses !== 'N/A' &&
            networkAttribute.IPv6Addresses.length > 0;
        const isBDF = Boolean(networkAttribute.BusInfo) && networkAttribute.BusInfo !== 'N/A';
        const flag = macIsValid || ipv4IsValid || ipv6IsValid || isBDF;

        if (flag) {
            virtualPort = new NetVirtualPort(new NetExtendAttr());
            virtualPort.setState = basicAttribute.LinkStatus;
            virtualPort.setMoreInfoData = portInfo;
        }
        return virtualPort;
    }

    /**
     * 子组件点击事件
     * 生成一系列的扩展属性（如果存在的话）
     * 网络属性
     * 连接视图
     * dcb连接信息
     * 发送和接收统计信息
     * 光模块信息
     * vlan信息
     * @param row 当前行的数据
     */
    extendAttrFactory(row: any, speed: 'Mbps' | 'Gbps') {
        const currentData = row;
        const vlans = row.moreInfoData.NetworkAttribute.VLAN || {};
        let vlan = '';
        if (vlans && vlans.length > 0) {
            vlans.forEach(v => {
                vlan += `${this.getVlan(v)} `;
            });
        } else {
            vlan = null;
        }
        // 增加OpticalModule.SerialNumber的判断，避免驱动被卸载后数据为null时显示标题
        const hasOptical = (
            row.moreInfoData.BasicAttribute.MediaType === 'Optical'
            && row.moreInfoData.OpticalModule && row.moreInfoData.OpticalModule?.SerialNumber
        )
            ? row.moreInfoData.OpticalModule
            : null;
        const optical = this.optical(hasOptical, speed);
        return of('').pipe(map(() => {
            return this.creatAttr(currentData, vlan, optical);
        }));
    }
    /**
     *
     * @param currentData 当前网卡基本属性
     * @param vlans vlan信息
     * @param optical 光模块信息
     * @returns 当前网卡所有扩展属性
     */
    public creatAttr(currentData, vlans, optical) {
        const portAttr = this.assemblingPort(currentData, vlans);
        const connect = this.connectView(currentData);
        const dcbInfo = this.dcbInfo(currentData);
        const staticInfo = this.sendAndRevInfo(currentData);
        const net = new NetExtendAttr();
        if (portAttr) {
            net.network = portAttr;
        }
        if (connect) {
            net.connect = connect;
        }
        if (staticInfo) {
            net.statistics = staticInfo;
        }
        if (dcbInfo) {
            net.dCB = dcbInfo;
        }
        if (optical) {
            net.optical = optical;
        }
        if (portAttr || connect || staticInfo || dcbInfo || optical) {
            net.init = true;
        }
        return net;
    }
    // 端口属性构造
    public assemblingPort(row, vlans?) {
        const baseAttr = row.moreInfoData.NetworkAttribute;

        const name = (baseAttr.Name === 'System Ethernet Interface' ? null : baseAttr.Name);
        const firmwarePackageVersion = baseAttr.FirmwareVersion ? baseAttr.FirmwareVersion : '--';
        const driverName = baseAttr.DriverName;
        const driverVersion = baseAttr.DriverVersion;
        const speedMbps = this._isInvalid(baseAttr.SpeedMbps) ? null : baseAttr.SpeedMbps + 'Mbps';
        const autoNeg = this._isInvalid(baseAttr.AutoNeg) ? null : (baseAttr.AutoNeg ? 'Enabled' : 'Disabled');
        const fullDuplex = this._isInvalid(baseAttr.FullDuplex) ? null : (baseAttr.FullDuplex ? 'True' : 'False');
        const bDF = baseAttr.BusInfo;
        const mACAddress = baseAttr.MAC;
        const permanentMACAddress = baseAttr.PermanentPhysicalAddress;
        const ipv4NetInfo = this._checkValid2(baseAttr.IPv4Addresses) ? this._joinIpv4Arr(baseAttr.IPv4Addresses) : null;
        const ipv6NetInfo = this._checkValid2(baseAttr.IPv6Addresses) ? this._joinIpv6Arr(baseAttr.IPv6Addresses) : null;
        // 以下属性可能不存在
        const linkSpeedGbps = baseAttr.LinkSpeedGbps ? baseAttr.LinkSpeedGbps + 'Gbps' : null;
        const wWPN = baseAttr.WWPN ? baseAttr.WWPN : null;
        const wWNN = baseAttr.WWNN ? baseAttr.WWNN : null;
        const workMode = this._dealLoop(baseAttr.WorkMode);
        const peerDeviceCredit = baseAttr.PeerDeviceCredit ? baseAttr.PeerDeviceCredit : null;
        const localDevconstCredit = baseAttr.LocalDeviceCredit ? baseAttr.LocalDeviceCredit : null;
        const tXRateGbps = baseAttr.TXRateGbps ? baseAttr.TXRateGbps + 'Gbps' : null;
        const rXRateGbps = baseAttr.RXRateGbps ? baseAttr.RXRateGbps + 'Gbps' : null;
        const rateNegotiationStage = this._dealRate(baseAttr.RateNegotiationStage);
        const portStatus = this._dealPort(baseAttr.PortStatus);
        const workingRate = baseAttr.WorkingRate ? baseAttr.WorkingRate + 'Gpbs' : null;
        const vlan = vlans;
        const netIPv46 = ipv4NetInfo || ipv6NetInfo ? new NetIPv46(
            ipv4NetInfo, ipv6NetInfo
        ) : null;
        if (!this._isNull([name, firmwarePackageVersion, driverName, driverVersion, speedMbps, autoNeg, fullDuplex, bDF, mACAddress,
            permanentMACAddress, netIPv46, linkSpeedGbps, wWPN, wWNN, workMode, peerDeviceCredit,
            localDevconstCredit, tXRateGbps, rXRateGbps, rateNegotiationStage, portStatus, workingRate, vlan])) {
            return null;
        }
        const net: NetworkAttr = new NetworkAttr(
            name, firmwarePackageVersion, driverName, driverVersion, speedMbps, autoNeg, fullDuplex, bDF, mACAddress,
            permanentMACAddress, netIPv46, linkSpeedGbps, wWPN, wWNN, workMode, peerDeviceCredit,
            localDevconstCredit, tXRateGbps, rXRateGbps, rateNegotiationStage, portStatus, workingRate, vlan
        );

        return net;
    }

    // 连接视图构造 纯函数
    public connectView(row) {
        if (!row.moreInfoData.LinkView) {
            return null;
        }
        const oem = row.moreInfoData.LinkView ? row.moreInfoData.LinkView : {};
        const switchName = oem.SwitchName;
        const switchConnectionID = oem.SwitchConnectionID;
        const switchConnectionPortID = oem.SwitchPortID;
        const switchPortVlanID = oem.SwitchVLANID;
        if (!this._isNull([switchName, switchConnectionID, switchConnectionPortID, switchPortVlanID])) {
            return null;
        }
        return new ConnectView(
            switchName,
            switchConnectionID,
            switchConnectionPortID,
            switchPortVlanID
        );
    }
    // DCB信息构造 纯函数
    public dcbInfo(row) {
        if (row.state === '--' || row.state === 'NET_DISCONNECT') {
            return null;
        }
        if (!row.moreInfoData.DCB) {
            return null;
        }
        const oem = row.moreInfoData.DCB ? row.moreInfoData.DCB : {};
        const up = this._formatPfcup(oem.Pfcup) !== null ? [0, 1, 2, 3, 4, 5, 6, 7] : null;
        const pfcup = this._formatPfcup(oem.Pfcup);
        const up2cos = oem.Up2cos;
        const pgid = oem.Pgid;
        const pGPCT = this._formatPGPCT(oem.PGPCT);
        const pgStrict = oem.PgStrict;
        if (!this._isNull([up, pfcup, up2cos, pgid, pGPCT, pgStrict])) {
            return null;
        }
        return new DCBAttr(
            up, pfcup, up2cos, pgid, pGPCT, pgStrict
        );
    }

    // 发送&接收统计信息构造 纯函数
    public sendAndRevInfo(row) {
        if (row.state === '--' || row.state === 'NET_DISCONNECT') {
            return null;
        }
        let oem: IStatisticsTrans;
        let recive: IStatisticsRecv;
        if (row.moreInfoData.Statistics) {
            oem = row.moreInfoData.Statistics.Transmitted ? row.moreInfoData.Statistics.Transmitted : {};
            recive = row.moreInfoData.Statistics.Received ? row.moreInfoData.Statistics.Received : {};
        } else {
            return null;
        }
        // 发送
        const totalBytesTransmitted = oem.TotalBytesTransmitted;
        const unicastPacketsTransmitted = oem.UnicastPacketsTransmitted;
        const multicastPacketsTransmitted = oem.MulticastPacketsTransmitted;
        const broadcastPacketsTransmitted = oem.BroadcastPacketsTransmitted;
        const singleCollisionTransmitFrames = oem.SingleCollisionTransmitFrames;
        const multipleCollisionTransmitFrames = oem.MultipleCollisionTransmitFrames;
        const lateCollisionFrames = oem.LateCollisionFrames;
        const excessiveCollisionFrames = oem.ExcessiveCollisionFrames;
        // 接收
        const totalBytesReceived = recive.TotalBytesReceived;
        const unicastPacketsReceived = recive.UnicastPacketsReceived;
        const multicastPacketsReceived = recive.MulticastPacketsReceived;
        const broadcastPacketsReceived = recive.BroadcastPacketsReceived;
        const fcsReceiveErrors = recive.FcsReceiveErrors;
        const alignmentErrors = recive.AlignmentErrors;
        const runtPacketsReceived = recive.RuntPacketsReceived;
        const jabberPacketsReceived = recive.JabberPacketsReceived;
        if (!this._isNull([
            totalBytesTransmitted,
            unicastPacketsTransmitted,
            multicastPacketsTransmitted,
            broadcastPacketsTransmitted,
            singleCollisionTransmitFrames,
            multipleCollisionTransmitFrames,
            lateCollisionFrames,
            excessiveCollisionFrames
        ])) {
            return null;
        }
        return new StatisticsAttr(
            new SendAttr(totalBytesTransmitted,
                unicastPacketsTransmitted,
                multicastPacketsTransmitted,
                broadcastPacketsTransmitted,
                singleCollisionTransmitFrames,
                multipleCollisionTransmitFrames,
                lateCollisionFrames,
                excessiveCollisionFrames
            ),
            new RecvAttr(
                totalBytesReceived,
                unicastPacketsReceived,
                multicastPacketsReceived,
                broadcastPacketsReceived,
                fcsReceiveErrors,
                alignmentErrors,
                runtPacketsReceived,
                jabberPacketsReceived
            )
        );
    }

    /**
     * 获取单个vlan信息
     * @param vlanInfo 单个vlan信息
     */
    private getVlan(vlanInfo) {
        const portData = {
            vlanId: vlanInfo.ID !== null ? vlanInfo.ID : '--',
            vlanEnable: vlanInfo.Enable !== null ? (vlanInfo.Enable ? 'Enable' : 'Disable') : '--',
            prEnable: vlanInfo.Priority !== null ? (vlanInfo.Priority) : '--'
        };
        return `${portData.vlanId}|${portData.vlanEnable}|${portData.prEnable}`;
    }

    /**
     * DeviceType类型
     *  1： Opticial,Electric,Aoc,null,其他未知的场景
     *  2： Copper
     *  3：Interface,Baset,Unknow    显示为光模块的原则：除3,2之外的所有场景
     *
     *  1：web显示为光模块信息
     *  2：web显示为电缆信息，不支持 波长、温度、温度门限值以及Indentifier的显示
     *  3：web不显示
     * 获取光模块信息
     * @param optical 光模块信息
     */
    public optical(optical, speed: 'Mbps' | 'Gbps') {
        if (!optical) {
            return null;
        }
        const deviceType = optical.DeviceType;
        const { type, moduleName, moduleTitle } = this._generateInfo(deviceType);
        const opticalInfo = this.opticalInfo(deviceType, optical, speed);
        if (type === 1) {
            const thresholdInfo = this.thresholdInfo(deviceType, optical);
            return new OpticalInfo(moduleTitle, opticalInfo, thresholdInfo, moduleName);
        } else if (type === 2) {
            return new CableInfo(moduleTitle, opticalInfo, moduleName);
        }
    }

    // 光模块信息 speed，速率的单位，以太网卡是Mbps，FC卡是Gbps
    public opticalInfo(dType, body, speed: 'Mbps' | 'Gbps') {
        const { type } = this._generateInfo(dType);
        const manufacturer = body.Manufacturer;
        const serialNumber = body.SerialNumber;
        const productionDate = body.ProductionDate;
        const partNumber = body.PartNumber;
        const packagingType = body.PackagingType;
        const mediumMode = body.MediumMode;
        let transceiverType = null;
        let supportedSpeedsMbps = null;
        if (body.TransceiverType && body.TransceiverType.length > 0) {// 需确认类型
            transceiverType = '';
            for (const key of body.TransceiverType) {
                transceiverType += key + ';';
            }
            transceiverType = transceiverType.substring(
                0,
                transceiverType.length - 1
            );
        }
        if (body.SupportedSpeedsMbps.length > 0) { // 需确认类型
            supportedSpeedsMbps = '';
            for (const key of body.SupportedSpeedsMbps) {
                supportedSpeedsMbps += `${key}${speed},`;
            }
            supportedSpeedsMbps = supportedSpeedsMbps.substring(
                0,
                supportedSpeedsMbps.length - 1
            );
        }

        const deviceType = body.DeviceType;
        const connectorType = body.ConnectorType;
        const transferDistance = body.TransferDistance;
        const rxLosState = this._dealRx(body.RxLosState);
        const txFaultState = this._dealRx(body.TxFaultState);
        let waveLengthNanometer = null;
        let indentifier = null;
        if (type === 1) {
            if (body.WaveLengthNanometer !== null) {
                waveLengthNanometer = body.WaveLengthNanometer;
            }
            indentifier = body.Indentifier;
        }
        if (!this._isNull([manufacturer, serialNumber, productionDate, partNumber, packagingType,
            mediumMode, transceiverType, supportedSpeedsMbps, deviceType, connectorType, transferDistance,
            rxLosState, txFaultState, waveLengthNanometer, indentifier
        ])) {
            return null;
        }
        return new OpticalAttr(manufacturer, serialNumber, productionDate,
            partNumber, packagingType, mediumMode, transceiverType, supportedSpeedsMbps, deviceType,
            connectorType, transferDistance, rxLosState, txFaultState, waveLengthNanometer, indentifier
        );
    }

    // 光模块中 告警门限信息
    public thresholdInfo(deviceType, body) {
        const { type } = this._generateInfo(deviceType);
        const data = {};
        let low = body.Temperature.LowerThresholdCritical;
        let up = body.Temperature.UpperThresholdCritical;
        let temperature = null;
        let voltage = null;
        let opticalAlarmT = null;
        let opticalAlarmV = null;
        if (type === 1) {
            temperature = body.Temperature.ReadingCelsius;
            low = this._getValidValue(low, up);
            up = this._getValidValue(up, low);
            opticalAlarmT = (low !== null && up !== null) ? up + ' | ' + low : null;
        }
        voltage = body.Voltage.ReadingVolts;
        const vLow = body.Voltage.LowerThresholdCritical;
        const vUp = body.Voltage.UpperThresholdCritical;
        opticalAlarmV = (vLow !== null && vUp !== null) ? vUp + ' | ' + vLow : null;
        if (body.TXBiasCurrent && body.TXBiasCurrent.length > 0) {
            for (const [i, v] of body.TXBiasCurrent.entries()) {
                let attrName = '';
                let attrName2 = '';
                if (body.TXBiasCurrent.length > 1) {
                    attrName = 'TX_BIAS_CURRENT' + (i + 1);
                    attrName2 = 'OPTICAL_ALARM_TXB' + (i + 1);
                } else {
                    attrName = 'TX_BIAS_CURRENT';
                    attrName2 = 'OPTICAL_ALARM_TXB';
                }
                data[attrName] = v.ReadingMilliAmperes;
                let cLow = v.LowerThresholdCritical;
                let cUp = v.UpperThresholdCritical;
                cLow = this._getValidValue(cLow, cUp);
                cUp = this._getValidValue(cUp, cLow);
                data[attrName2] = (cLow !== null && cUp !== null) ? cUp + ' | ' + cLow : null;
            }
        }
        if (body.TXPower && body.TXPower.length > 0) {
            for (const [i, v] of body.TXPower.entries()) {
                let attrName = '';
                let attrName2 = '';
                if (body.TXPower.length > 1) {
                    attrName = 'TXPOWER' + (i + 1);
                    attrName2 = 'OPTICAL_ALARM_TXP' + (i + 1);
                } else {
                    attrName = 'TXPOWER';
                    attrName2 = 'OPTICAL_ALARM_TXP';
                }
                data[attrName] = v.ReadingMilliWatts;
                let lLow = v.LowerThresholdCritical;
                let lUp = v.UpperThresholdCritical;
                lLow = this._getValidValue(lLow, lUp);
                lUp = this._getValidValue(lUp, lLow);
                data[attrName2] = lLow !== null && lUp !== null ? lUp + ' | ' + lLow : null;
            }
        }

        if (body.RXPower && body.RXPower.length > 0) {
            for (const [i, v] of body.RXPower.entries()) {
                let attrName = '';
                let attrName2 = '';
                if (body.RXPower.length > 1) {
                    attrName = 'RXPOWER' + (i + 1);
                    attrName2 = 'OPTICAL_ALARM_RXP' + (i + 1);
                } else {
                    attrName = 'RXPOWER';
                    attrName2 = 'OPTICAL_ALARM_RXP';
                }
                data[attrName] = v.ReadingMilliWatts;
                let tLow = v.LowerThresholdCritical;
                let tUp = v.UpperThresholdCritical;
                tLow = this._getValidValue(tLow, tUp);
                tUp = this._getValidValue(tUp, tLow);
                data[attrName2] = (tLow !== null && tUp !== null) ? tUp + ' | ' + tLow : null;
            }
        }
        if (JSON.stringify(data) === '{}') {
            if (!this._isNull([temperature, voltage, opticalAlarmT, opticalAlarmV])) {
                return null;
            } else {
                return new ThresholdAttr(temperature, voltage, opticalAlarmT, opticalAlarmV);
            }
        }
        // 四个值均为null即表示没有告警门限信息，需隐藏标题
        if (temperature === null && voltage === null && opticalAlarmT === null && opticalAlarmV === null) {
            return null;
        }
        return new ThresholdAttr(temperature, voltage, opticalAlarmT, opticalAlarmV, data);
    }

    private _getValidValue(low, up) {
        /**
         * 下限取值：undefined, null为无效值, 0为有效值
         * 1. low, up为无效值时，返回null,
         * 2. low为无效值，up为有效值时，返回0
         * 3. low为有效值返回low
         */
        if (!low && !up && low !== 0) {
            return null;
        } else if (!low) {
            return 0;
        } else {
            return low;
        }
    }
    private _dealRx(state) {
        let str = null;
        if (state === null || state === undefined) {
            return str;
        }
        if (state) {
            str = 'True';
        } else {
            str = 'False';
        }
        return str;
    }
    private _generateInfo(deviceType) {
        let type = 0;
        let moduleName = '';
        let moduleTitle = '';
        switch (deviceType) {
            case 'Interface':
                break;
            case 'Baset':
                break;
            case 'Unknown':
                moduleTitle = '';
                moduleName = '';
                type = 3;
                break;
            case 'Copper':
                moduleTitle = 'NET_CABLE_INFO';
                moduleName = 'NET_CABLE';
                type = 2;
                break;
            default:
                moduleTitle = 'NET_OPTICAL_INFO';
                moduleName = 'NET_OPTICAL';
                type = 1;
                break;
        }
        return { type, moduleName, moduleTitle };
    }
    /**
     * 判断值是否有效并且不为'00:00:00:00:00:00' 或者 --
     * @param value 需要判断得值
     */
    private _checkValid(value) {
        return Boolean(value)
            && value !== '00:00:00:00:00:00'
            && value !== '--';
    }
    /**
     * 判断值是否有效并且不为 -- 和 length大于0
     * @param value 需要判断得值
     */
    private _checkValid2(value) {
        return Boolean(value)
            && value !== '--'
            && value.length > 0;
    }

    private _dealLoop(workMode) {
        let str = null;
        if (workMode === 'loop') {
            str = 'WORK_MODE_LOOP';
        } else if (workMode === 'NonLoop') {
            str = 'WORK_MODE_NON_LOOP';
        }
        return str;

    }

    private _dealRate(rate) {
        let str = null;
        if (rate === 'WaitSignal') {
            str = 'WAIT_SIGNAL_LABEL';
        } else if (rate === 'Negotiating') {
            str = 'NEGOTIATING_LABEL';
        } else if (rate === 'NegotiationCompleted') {
            str = 'NEGOTIATION_COMPLETED_LABEL';
        }
        return str;
    }

    private _dealPort(rate) {
        let str = null;
        if (rate === 'Disabled') {
            str = 'DISABLED';
        } else if (rate === 'LinkUp') {
            str = 'NET_CONNECTION';
        } else if (rate === 'WaitSignal') {
            str = 'NEGOTIATING_LABEL';
        }
        return str;
    }

    private _formatPGPCT(value) {
        if (!value) {
            return value;
        }
        const newArr = [];
        value.forEach(item => {
            newArr.push(item + '%');
        });
        return newArr;
    }

    private _formatPfcup(value) {
        if (!value) {
            return value;
        }
        const tmpCpu = parseInt(value, 10).toString(2);
        if (tmpCpu.length >= 8) {
            return tmpCpu.substr(0, 8).split('').reverse();
        } else {
            let zero = '';
            for (let i = 0; i < 8 - tmpCpu.length; i++) {
                zero += '0';
            }
            return (zero + tmpCpu).split('').reverse();
        }
    }
    /**
     * 判断如果有一个参数有值，非null，非undefined即返回true，否则全为空返回fasle
     * @param value 需要判断的值
     */
    private _isNull(value) {
        for (const item of value) {
            if (!(item instanceof Array && item.length === 0)) {
                if (item && item !== null && item !== undefined) {
                    return true;
                }
            }
        }
        return false;
    }

    private _isInvalid(value) {
        if (value === undefined || value === null) {
            return true;
        }
        return false;
    }

    private _joinIpv4Arr(arr: any[]): IIpv4NetInfo[] {
        return arr.map((key) => { return {
            ipv4: key.Address,
                mask: key.Mask,
                gateway: key.Gateway !== null ? key.Gateway : '--'
            };
        });
    }

    private _joinIpv6Arr(arr: any[]): IIpv6NetInfo[] {
        return arr.map((key) => {
            return {
                ipv6: key.Address,
                prefix: key.Prefix,
                gateway: key.Gateway !== null ? key.Gateway : '--'
            };
        });
    }
}
interface IStatisticsRecv {
    TotalBytesReceived: string;
    UnicastPacketsReceived: string;
    MulticastPacketsReceived: string;
    BroadcastPacketsReceived: string;
    FcsReceiveErrors: string;
    AlignmentErrors: string;
    RuntPacketsReceived: string;
    JabberPacketsReceived: string;
}
interface IStatisticsTrans {
    TotalBytesTransmitted: string;
    UnicastPacketsTransmitted: string;
    MulticastPacketsTransmitted: string;
    BroadcastPacketsTransmitted: string;
    SingleCollisionTransmitFrames: string;
    MultipleCollisionTransmitFrames: string;
    LateCollisionFrames: string;
    ExcessiveCollisionFrames: string;
}
