import { Injectable } from '@angular/core';
import { map } from 'rxjs/internal/operators/map';
import { mergeMap } from 'rxjs/internal/operators/mergeMap';
import { TeamCard } from '../../classes/team-card/team-card';
import { of } from 'rxjs';
import { _dealIpInfo, getVlan, _dealUsefulString } from 'src/app/common-app/utils';
import { TeamPortArr } from '../../classes/ports/team/team-port-Arr';
import { TeamPort } from '../../classes/ports/team/team-port';
import { TeamExtendAttr } from '../../classes/ports/team/team-extend-Attr';

@Injectable({
    providedIn: 'root'
})
export class TeamService {

    constructor() { }
    /**
     * 生成设备信息、端口信息
     */
    factory(teamCardInfo: any, name: string) {
        return of('').pipe(
            mergeMap(() => {
                const cardName = teamCardInfo.Name;
                const linkStatus = teamCardInfo.LinkStatus || null;
                const iPv4Addresses = _dealIpInfo(teamCardInfo.IPv4Addresses, 'ipv4');
                const iPv6Addresses = _dealIpInfo(teamCardInfo.IPv6Addresses, 'ipv6');
                const macAddress = teamCardInfo.MAC || '--';
                const workModel = teamCardInfo.WorkMode;

                const teamPort = teamCardInfo.PortProperties || [];
                const vlansInfo = this._dealVlans(teamCardInfo.VLAN);
                return of('').pipe(
                    map(() => {
                        return [new TeamCard(
                            name,
                            'team',
                            teamCardInfo,
                            true,
                            cardName,
                            linkStatus,
                            workModel,
                            iPv4Addresses,
                            iPv6Addresses,
                            macAddress,
                            vlansInfo
                        ), teamPort];
                    }),
                    mergeMap(([teamCard, teamPorts]) => {
                        if (teamPorts && teamPorts.length === 0) {
                            return of('').pipe(map(() => teamCard));
                        }
                        const results = [];
                        teamPorts.forEach((port: any) => {
                            results.push(this._getPort(port));
                        });
                        return of('').pipe(
                            map(() => {
                                const ports = new TeamPortArr();
                                results.map((port: TeamPort) => {
                                    if (port) {
                                        ports.addPort(port);
                                    }
                                });
                                teamCard.setPort = ports;
                                return teamCard;
                            })
                        );
                    }),
                );
            })
        );
    }

    private _getPort(portInfo: any) {
        let portData;
        portData = new TeamPort(new TeamExtendAttr());
        portData.netCardName = _dealUsefulString(portInfo?.NetCardName);
        portData.netName = _dealUsefulString(portInfo?.Name);
        portData.port = `Port${portInfo?.PhysicalPortNumber}`;
        portData.macAddress = _dealUsefulString(portInfo?.MAC);
        portData.state = _dealUsefulString(portInfo?.LinkStatus);
        return portData;
    }
    private _dealVlans(vlans: any) {
        if (vlans && vlans.length > 0) {
            let str = '';
            vlans.forEach((vlan) => {
                str += getVlan(vlan);
            });
            return str;
        }
        return '';
    }
}
