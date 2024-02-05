import { Injectable } from '@angular/core';
import { map, mergeMap } from 'rxjs/operators';
import { of } from 'rxjs';
import { BridgeCard } from '../../classes/bridge-card/bridge-card';
import { BridgePortArr } from '../../classes/ports/bridge/bridge-port-Arr';
import { BridgePort } from '../../classes/ports/bridge/bridge-port';
import { BridgeExtendAttr } from '../../classes/ports/bridge/bridge-extend-Attr';
import { _dealIpInfo, _dealUsefulString } from 'src/app/common-app/utils';

@Injectable({
    providedIn: 'root'
})
export class BridgeService {

    constructor() { }

    /**
     * 生成设备信息、端口信息
     */
    factory(bridgeCardInfo: any, name: string) {
        return of('').pipe(
            mergeMap(() => {
                const cardName = bridgeCardInfo.Name;
                const linkStatus = bridgeCardInfo.LinkStatus || null;
                const iPv4Addresses = _dealIpInfo(bridgeCardInfo.IPv4Addresses, 'ipv4');
                const iPv6Addresses = _dealIpInfo(bridgeCardInfo.IPv6Addresses, 'ipv6');
                const macAddress = bridgeCardInfo.MAC || '--';
                const workModel = bridgeCardInfo.WorkMode;

                const bridgePort = bridgeCardInfo.PortProperties || [];
                const vlansInfo = this._dealVlans(bridgeCardInfo.VLAN);

                return of('').pipe(
                    map(() => {
                        return [new BridgeCard(
                            name,
                            'bridge',
                            bridgeCardInfo,
                            true,
                            cardName,
                            linkStatus,
                            workModel,
                            iPv4Addresses,
                            iPv6Addresses,
                            macAddress,
                            vlansInfo
                        ), bridgePort];
                    }),
                    mergeMap(([bridgeCard, bridgePorts]) => {
                        if (bridgePorts && bridgePorts.length === 0) {
                            return of('').pipe(map(() => bridgeCard));
                        }
                        const results = [];
                        bridgePorts.forEach((port: any, index) => {
                            results.push(this._getPort(port, index));
                        });
                        return of('').pipe(
                            map(() => {
                                const ports = new BridgePortArr();
                                results.map((port: BridgePort) => {
                                    if (port) {
                                        ports.addPort(port);
                                    }
                                });
                                bridgeCard.setPort = ports;
                                return bridgeCard;
                            })
                        );
                    }),
                );
            })
        );
    }
    /**
     *
     * @param portInfo 获取当前端口信息
     * @param index 当前端口下表
     * @returns 当前端口的一条信息
     */
    private _getPort(portInfo: any, index: any) {
        let portData;
        portData = new BridgePort(new BridgeExtendAttr());
        portData.netCardName = _dealUsefulString(portInfo?.NetCardName);
        portData.netName = _dealUsefulString(portInfo?.Name);
        portData.port = `Port${portInfo?.PhysicalPortNumber}`;
        portData.macAddress = _dealUsefulString(portInfo?.MAC);
        portData.state = _dealUsefulString(portInfo?.LinkStatus);
        return portData;
    }
    /**
     *  处理后端返回的VLAN数组
     */
    private _dealVlans(vlans: any) {
        if (vlans && vlans.length > 0) {
            let str = '';
            vlans.forEach((vlan) => {
                str += this.getVlan(vlan);
            });
            return str;
        }
        return '';
    }
    /**
     * 获取单个vlan信息
     * @param vlan 单个vlan信息
     * @returns 组装的vlan信息
     */
    private getVlan(vlan: any) {
        const portData = {};
        portData['vlanId'] = vlan.ID !== null ? vlan.ID : '--';
        portData['vlanEnable'] = vlan.Enable !== null ? (vlan.Enable ? 'Enable' : 'Disable') : '--';
        portData['prEnable'] = vlan.Priority !== null ? (vlan.Priority) : '--';
        return `${portData['vlanId']}|${portData['vlanEnable']}|${portData['prEnable']}`;
    }
}
