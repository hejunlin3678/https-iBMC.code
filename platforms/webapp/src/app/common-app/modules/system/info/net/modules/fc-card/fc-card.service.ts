import { Injectable } from '@angular/core';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { mergeMap } from 'rxjs/operators';
import { FcCard } from '../../classes/fc-card/fc-card';
import { FcExtendAttr } from '../../classes/ports/fc/fc-extend-data';
import { FcPhysicalPort } from '../../classes/ports/fc/fc-physical-port';
import { FcPort } from '../../classes/ports/fc/fc-port';
import { FcPortArr } from '../../classes/ports/fc/fc-port-Arr';

@Injectable({
    providedIn: 'root'
})
export class FcCardService {
    constructor() {}
    factory(fcCardInfo, name: string) {
        return of('').pipe(
            map(() => {
                const cardName = fcCardInfo.CardName;
                const model = fcCardInfo.CardModel || null;
                const chipModel = fcCardInfo.ChipModel || null;
                const manufacturer = fcCardInfo.CardManufacturer || null;
                const chipManufacturer = fcCardInfo.ChipManufacturer || null;
                const boardId = fcCardInfo.BoardID || null;
                const partNumber = fcCardInfo.PartNumber || null;
                const pCBVersion = fcCardInfo.PCBVersion || null;
                const associatedResource = fcCardInfo.AssociatedResource || null;
                const serialNumber = fcCardInfo.SerialNumber || null;
                const pcieSlotId = fcCardInfo.PcieSlotId || null;

                // fc卡端口集合
                const fcPorts = fcCardInfo.PortProperties || [];
                return [
                    new FcCard(
                        name,
                        'fc',
                        fcCardInfo,
                        true,
                        cardName,
                        model,
                        chipModel,
                        manufacturer,
                        chipManufacturer,
                        boardId,
                        partNumber,
                        pCBVersion,
                        associatedResource,
                        serialNumber,
                        pcieSlotId
                    ),
                    fcPorts
                ];
            }),
            mergeMap(([fcCard, fcPorts]) => {
                if (fcPorts && fcPorts.length === 0) {
                    return of('').pipe(map(() => fcCard));
                }
                const results = [];
                fcPorts.map((port: any) => {
                    results.push(this.initPort(port));
                });
                return of('').pipe(
                    map(() => {
                        const ports = new FcPortArr();
                        results.map((port: FcPort) => {
                            if (port) {
                                ports.addPort(port);
                            }
                        });
                        fcCard.setPort = ports;
                        return fcCard;
                    })
                );
            })
        );
    }

    /**
     * 判断值是否有效并且不为'00:00:00:00:00:00' 或者 --
     * @param value 需要判断得值
     */
    private _checkValid(value) {
        return Boolean(value) && value !== '00:00:00:00:00:00' && value !== '--';
    }

    /**
     * 判断值是否有效并且不为 -- 和 length大于0
     * @param value 需要判断得值
     */
    private _checkValid2(value) {
        return Boolean(value) && value !== '--' && value.length > 0;
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
    public initPort(portInfo) {
        let physicalPort = null;
        let macIsValid = false;
        const basicAttribute = portInfo.BasicAttribute;
        const networkAttribute = portInfo.NetworkAttribute;
        // 判断是否为IB卡
        macIsValid = this._checkValid(networkAttribute.MAC);
        const ipv4IsValid = this._checkValid2(networkAttribute.IPv4Addresses);
        const ipv6IsValid = this._checkValid2(networkAttribute.IPv6Addresses);
        const isOptical = portInfo.OpticalModule ? true : false;
        const isBDF = Boolean(networkAttribute.BusInfo) && networkAttribute.BusInfo !== '--';
        const flag = macIsValid || ipv4IsValid || ipv6IsValid || isOptical || isBDF;
        const isDisabled = networkAttribute.State === 'Disabled' ? false : true;
        if (flag || (!flag && isDisabled)) {
            const portName = basicAttribute.Name || '--';
            const fcId = basicAttribute.FC_ID || '--';
            physicalPort = new FcPhysicalPort(new FcExtendAttr(), portName, fcId, basicAttribute.InterfaceType, basicAttribute.LinkStatus);
            physicalPort.setMoreInfoData = portInfo;
        }
        return physicalPort;
    }
}
