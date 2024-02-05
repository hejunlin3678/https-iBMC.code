import { Injectable } from '@angular/core';
import { IPV4Address, IPV6Address } from './model';
import { forkJoin } from 'rxjs';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { map, pluck } from 'rxjs/operators';
import { HttpErrorResponse, HttpResponse } from '@angular/common/http';
import { Observable } from 'rxjs/internal/Observable';

@Injectable({
    providedIn: 'root'
})

export class NetworkService {
    constructor(
        private httpService: HttpService
    ) {

    }

    public getHMM1MainBoard(): Observable<any> {
        return this.httpService.get('/redfish/v1/Managers/HMM1').pipe(
            pluck('body')
        );
    }

    public getCurrentDeviceIp(param: { board: string, type: 'ipv4' | 'ipv6', ipType: 'static' | 'float' }) {
        let url = '';
        if (param.ipType === 'static') {
            url = `/redfish/v1/Managers/${param.board}/EthernetInterfaces/StaticIp`;
        } else {
            url = `/redfish/v1/Managers/${param.board}/EthernetInterfaces/FloatIp`;
        }
        return this.httpService.get(url).pipe(
            pluck('body'),
            map((res: any) => {
                if (param.type === 'ipv4') {
                    return [res.IPv4Addresses[0].Address];
                } else {
                    const ips = res.IPv6Addresses.filter(item => {
                        return item.Address !== null;
                    }).map(item => {
                        return item.Address;
                    });
                    return ips;
                }
            })
        );
    }

    public getOverViewInfo(): Observable<any> {
        return this.httpService.get('/redfish/v1/chassisoverview').pipe(
            pluck('body'),
            map((res) => {
                return this.parseOverView(res);
            })
        );
    }

    private parseOverView(data): { ips: number[], mms: number[] } {
        const members = data.Components;
        const slotsArr = {
            ips: [],
            mms: []
        };
        members.forEach(item => {
            if (item.Id.indexOf('HMM') === -1 && item.ManagedByMM && item.HotswapState !== 7 && item.HotswapState !== 0) {
                let slotId = -1;
                if (item.Id.indexOf('Blade') > -1) {
                    slotId = parseInt(item.Id.substr(5), 10);
                }
                if (item.Id.indexOf('Swi') > -1) {
                    slotId = parseInt(item.Id.substr(3), 10) + 32;
                }
                slotsArr.ips.push(slotId);
            }

            if (item.Id.indexOf('HMM') > -1) {
                slotsArr.mms.push(item.Id);
            }
        });
        return slotsArr;
    }

    // 获取IPV4得地址信息
    public getIPV4Info(slotIds: number[]): Observable<any> {
        return new Observable(observe => {
            forkJoin(slotIds.map(slotId => {
                return this.httpService.get('/redfish/v1/Chassis/Ipv4Config?slot=' + slotId).pipe(pluck('body'));
            })).subscribe(ipv4Arr => {
                const tempIps = [];
                ipv4Arr.forEach(item => {
                    const ipInfo = item.IPv4Addresses[0];
                    const tempIp = new IPV4Address({
                        type: 'ipv4',
                        slotNum: ipInfo.Id,
                        address: ipInfo.Address,
                        subnetMask: ipInfo.SubnetMask,
                        gateWay: ipInfo.GateWay,
                        dhcpModel: ipInfo.AddressOrigin === 'DHCP',
                        url: ipInfo['@odata.id']
                    });
                    if (ipInfo.AddressOrigin === 'DHCP' || ipInfo.AddressOrigin === 'Static') {
                        tempIps.push(tempIp);
                    }
                });
                observe.next(tempIps);
                observe.complete();
            });
        });
    }

    // 获取IPV6得地址信息
    public getIPV6Info(slotIds: number[]): Observable<any> {
        return new Observable(observe => {
            forkJoin(slotIds.map(slotId => {
                return this.httpService.get('/redfish/v1/Chassis/Ipv6Config?slot=' + slotId).pipe(pluck('body'));
            })).subscribe(ipv4Arr => {
                const tempIps = [];
                ipv4Arr.forEach(item => {
                    const ipInfo = item.IPv6Addresses[0];
                    const tempIp = new IPV6Address({
                        type: 'ipv6',
                        slotNum: ipInfo.Id,
                        address: ipInfo.Address || '',
                        prefix: ipInfo.PrefixLength + '' || '0',
                        gateWay: ipInfo.IPv6DefaultGateway || '',
                        dhcpModel: ipInfo.AddressOrigin === 'DHCPv6',
                        slaacQuantity: ipInfo.SLACCIPv6Addresses,
                        url: ipInfo['@odata.id']
                    });
                    if (ipInfo.AddressOrigin === 'DHCPv6' || ipInfo.AddressOrigin === 'Static') {
                        tempIps.push(tempIp);
                    }
                });
                observe.next(tempIps);
                observe.complete();
            });
        });
    }

    public querySlotEtag(url): Observable<any> {
        return this.httpService.get(url).pipe(
            map((res: HttpResponse<any>) => {
                return res.headers.get('etag') || '';
            })
        );
    }

    // 保存IPV4地址
    public saveIPConfig(url, param: any): Observable<any> {
        return new Observable(observe => {
            this.querySlotEtag(url).subscribe({
                next: (etag: string) => {
                    this.httpService.patch(url, param, { type: 'redfish', headers: { 'If-Match': etag } })
                        .pipe(pluck('body')).subscribe({
                            next: (resArr: HttpResponse<any>[]) => {
                                observe.next(resArr);
                                observe.complete();
                            },
                            error: (error: HttpErrorResponse) => {
                                observe.next(error);
                                observe.complete();
                            }
                        });
                },
                error: (error) => {
                    observe.next(error);
                    observe.complete();
                }
            });
        });
    }
}
